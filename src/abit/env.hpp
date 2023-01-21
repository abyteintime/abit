#pragma once

#include <string>
#include <string_view>
#include <vector>

namespace abit {

std::wstring
GetEnv(const wchar_t* var);

void
SetEnv(const wchar_t* var, const std::wstring& data);

std::vector<std::wstring>
ParseEnv(const wchar_t* env);

std::wstring
MakeEnv(const std::vector<std::wstring>& vars);

std::vector<std::wstring>
GetAllEnvVars();

}
