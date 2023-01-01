#pragma once

#include <cstdint>
#include <filesystem>
#include <optional>
#include <string>
#include <variant>
#include <vector>

namespace yarn {

struct Patch
{
	enum class Type
	{
		Replacement,
		Injection,
	};

	struct Replacement
	{
		std::string className;
		std::string function;
	};

	struct Injection
	{
		// TBD
	};

	std::string comment;
	std::variant<Replacement, Injection> data;
};

struct ModConfig
{
	uint32_t version;
	std::vector<Patch> patches;

	static std::optional<std::filesystem::path> PathInModDirectory(
		const std::filesystem::path& modDirectory
	);
	static ModConfig Load(const std::filesystem::path& path);
};

}
