#include "abit/io.hpp"

#include <fstream>

#include "abit/error.hpp"

std::vector<char>
abit::ReadFile(const std::filesystem::path& path)
{
	std::ifstream stream{ path };
	if (!stream.is_open()) {
		throw Error{ std::string("could not open stream for reading file (") + path.string() +
					 ")" };
	}

	stream.seekg(0, std::ios_base::end);
	size_t fileSize = stream.tellg();
	stream.seekg(0);

	std::vector<char> bytes;
	bytes.resize(fileSize);
	stream.read(&bytes[0], fileSize);

	return bytes;
}
