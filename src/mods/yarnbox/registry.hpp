#pragma once

#include <memory>
#include <string>
#include <string_view>
#include <unordered_map>

#include "abit/map.hpp"

#include "abit/ue/UClass.hpp"
#include "abit/ue/UFunction.hpp"

namespace yarn {

struct Chunk
{
	ue::UStruct* ustruct;

	std::string pathName;
	std::string name;
	abit::AsciiCaseInsensitiveMap<std::shared_ptr<Chunk>> functions;

	Chunk(ue::UStruct* ustruct);

	static std::string GetPathName(ue::UStruct* ustruct);
};

class Registry
{
	abit::AsciiCaseInsensitiveMap<std::shared_ptr<Chunk>> chunksByName;
	std::unordered_map<ue::UStruct*, std::shared_ptr<Chunk>> chunksByPointer;

public:
	std::shared_ptr<Chunk> RegisterChunk(std::shared_ptr<Chunk>&& chunk);

	const Chunk* GetChunkByName(const std::string& name) const;
	Chunk* GetChunkByName(const std::string& name);

	const Chunk* GetChunkByPointer(ue::UStruct* pointer) const;
	Chunk* GetChunkByPointer(ue::UStruct* pointer);
};

}
