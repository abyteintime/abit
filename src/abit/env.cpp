#include "abit/env.hpp"

#include "abit/error.hpp"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

std::wstring
abit::GetEnv(const wchar_t* var)
{
	DWORD length = GetEnvironmentVariableW(var, nullptr, 0);
	std::wstring content(length, '\0');
	if (GetEnvironmentVariableW(var, content.data(), content.size()) == 0) {
		throw abit::Error::System("cannot get environment variable");
	}
	return content;
}

void
abit::SetEnv(const wchar_t* var, const std::wstring& data)
{
	if (!SetEnvironmentVariableW(var, data.c_str())) {
		throw abit::Error::System("cannot set environment variable");
	}
}

std::vector<std::wstring>
abit::ParseEnv(const wchar_t* env)
{
	std::vector<std::wstring> out;

	while (*env != '\0') {
		const wchar_t* start = env;
		while (*env != '\0') {
			++env;
		}
		size_t length = env - start;
		++env; // skip null byte
		std::wstring_view varView{ start, length };
		std::wstring var{ varView };
		out.push_back(std::move(var));
	}

	return out;
}

std::wstring
abit::MakeEnv(const std::vector<std::wstring>& vars)
{
	std::wstring result;

	for (const std::wstring& var : vars) {
		result += var;
		result += L'\0';
	}
	result += L'\0';

	return result;
}

std::vector<std::wstring>
abit::GetAllEnvVars()
{
	LPWCH env = GetEnvironmentStringsW();
	if (env == nullptr) {
		return {};
	}
	std::vector<std::wstring> envVector = ParseEnv(env);
	FreeEnvironmentStringsW(env);
	return envVector;
}
