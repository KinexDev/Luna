#pragma once

extern "C" {
#include "lualib.h"
#include "luacode.h"
}
#include <iostream> 
#include <functional>
#include <fstream>
#include <sstream>
#include <unordered_map>

class Runtime {
public:
#ifndef LUAU_IS_BUILD
	int DoString(const std::string& source, int results = 0);
	int DoFile(const std::string filePath, int results = 0);
#endif
	int DoBytecode(const char* bytecode, int results = 0);
	int Execute(int nargs = 0, int results = 0);
	Runtime(bool useRequire = true);
	~Runtime();
	lua_State* L;
};