#pragma once
#include "lualib.h"
#include <fstream>
#include <sstream>
#include <filesystem>

namespace File {
	void Register(lua_State* L);
	int ReadAllText(lua_State* L);
	int WriteAllText(lua_State* L);
	int Exists(lua_State* L);
	int Delete(lua_State* L);
	int CreateDirectory(lua_State* L);
	int DeleteDirectory(lua_State* L);
	int ExistsDirectory(lua_State* L);
}