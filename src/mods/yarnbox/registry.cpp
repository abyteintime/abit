#include "yarnbox/registry.hpp"

#include "abit/loader/logging.hpp"

#include "fmt/core.h"

#include "yarnbox/ue/UObject/fmt.hpp"
#include "yarnbox/ue/cast.hpp"

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
