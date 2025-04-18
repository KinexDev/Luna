#pragma once
extern "C" {
#include "lualib.h"
}
#include <fstream>
#include <sstream>
#include <filesystem>

namespace Filesystem {
	void Register(lua_State* L);
	int Read(lua_State* L);
	int ReadDir(lua_State* L);
	int Write(lua_State* L);
	int WriteDir(lua_State* L);
	int IsFile(lua_State* L);
	int IsDir(lua_State* L);
	int Remove(lua_State* L);
	int RemoveDir(lua_State* L);
	int Move(lua_State* L);
	int Copy(lua_State* L);
}