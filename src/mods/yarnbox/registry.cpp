#include "yarnbox/registry.hpp"

#include "fmt/core.h"

#include "abit/loader/logging.hpp"

#include "abit/ue/UObject/fmt.hpp"
#include "abit/ue/cast.hpp"

#include "yarnbox/bytecode/disassembler.hpp"

using namespace yarn;
using namespace ue;

Chunk::Chunk(UStruct* ustruct)
	: ustruct(ustruct)
	, name(ustruct->GetName().ToString())
	, pathName(GetPathName(ustruct))
{
}

static std::string
GetObjectPathName(UObject* object)
{
	std::string ownName = object->GetName().ToString();
	if (object->outer != nullptr) {
		return fmt::format("{}.{}", GetObjectPathName(object->outer), ownName);
	} else {
		return ownName;
	}
}

std::string
Chunk::GetPathName(ue::UStruct* ustruct)
{
	return GetObjectPathName(ustruct);
}

const Chunk::Disassembly*
Chunk::GetOrPerformDisassembly()
{
	if (!disassembly.has_value()) {
		Disassembly disasm;
		if (Disassembler::IsBytecodeTooLarge(ustruct->bytecode.length)) {
			spdlog::error(
				"Cannot disassemble function '{}' because its bytecode is too large ({} > {})",
				pathName,
				ustruct->bytecode.length,
				std::numeric_limits<uint16_t>::max()
			);
			return nullptr;
		}
		if (std::optional<BytecodeTree::NodeIndex> rootNode =
				yarn::Disassemble(&ustruct->bytecode[0], ustruct->bytecode.length, disasm.tree)) {
			disasm.rootNode = *rootNode;
			this->disassembly = std::move(disasm);
		} else {
			spdlog::error(
				"Function '{}' had errors in its disassembly and will not be patchable", pathName
			);
		}
	}
	return disassembly.has_value() ? &disassembly.value() : nullptr;
}

std::pair<const Chunk::Disassembly*, const Chunk::Analysis*>
Chunk::GetOrPerformAnalysis()
{
	if (!analysis.has_value()) {
		if (const Disassembly* disasm = GetOrPerformDisassembly()) {
			Analysis analysis;
			analysis.jumps.Analyze(disasm->tree, disasm->rootNode);
			this->analysis = std::move(analysis);
		} else {
			spdlog::error(
				"Cannot analyze function '{}' for patching because its bytecode tree is not "
				"available",
				pathName
			);
		}
	}
	return std::make_pair(
		disassembly.has_value() ? &disassembly.value() : nullptr,
		analysis.has_value() ? &analysis.value() : nullptr
	);
}

std::shared_ptr<Chunk>
Registry::RegisterChunk(std::shared_ptr<Chunk>&& chunk)
{
	if (chunksByPointer.count(chunk->ustruct) > 0) {
		return chunksByPointer[chunk->ustruct];
	}

	if (auto* outer = Cast<UStruct>(chunk->ustruct->outer)) {
		auto* outerChunk = GetChunkByPointer(outer);
		if (outerChunk == nullptr) {
			// Sometimes a class's functions are registered before the class itself.
			// Thus, we need to handle that case and register the class here.
			// This process may also cascade onto the class's enclosing package.
			auto newOuterChunk = std::make_shared<Chunk>(outer);
			spdlog::trace(
				"Outer chunk '{}' needs to be registered before '{}'",
				newOuterChunk->pathName,
				chunk->pathName
			);
			outerChunk = RegisterChunk(std::move(newOuterChunk)).get();
		}
		outerChunk->functions[chunk->name] = chunk;
	}

	chunksByName[chunk->pathName] = chunk;
	chunksByPointer[chunk->ustruct] = chunk;

	spdlog::trace("Registered chunk {} as '{}'", UObjectFmt{ chunk->ustruct }, chunk->pathName);

	return chunk;
}

const Chunk*
Registry::GetChunkByName(const std::string& name) const
{
	return chunksByName.count(name) > 0 ? &*chunksByName.at(name) : nullptr;
}

Chunk*
Registry::GetChunkByName(const std::string& name)
{
	return chunksByName.count(name) > 0 ? &*chunksByName.at(name) : nullptr;
}

const Chunk*
Registry::GetChunkByPointer(ue::UStruct* pointer) const
{
	return chunksByPointer.count(pointer) > 0 ? &*chunksByPointer.at(pointer) : nullptr;
}

Chunk*
Registry::GetChunkByPointer(ue::UStruct* pointer)
{
	return chunksByPointer.count(pointer) > 0 ? &*chunksByPointer.at(pointer) : nullptr;
}
