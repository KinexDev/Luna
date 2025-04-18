#pragma once
#include <filesystem>
extern "C" {
#include "lualib.h"
#include "luacode.h"
}
#include <fstream>
#include <sstream>
#include "iostream"
#include <unordered_map>

namespace Require {
#ifdef LUAU_IS_BUILD
	extern std::unordered_map<std::string, std::string> requiredScripts;
#endif 
	extern std::unordered_map<std::string, std::vector<int>> cachedRequires;
	extern std::filesystem::path directory;
	int Require(lua_State* L);
}