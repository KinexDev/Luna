#pragma once
extern "C" {
#include "lualib.h"
}
#include "iostream"
#include "Lib.h"
#include "Filesystem.h"

namespace std {
	void Load(lua_State* L);
	void Unload(lua_State* L);
	void AddLib(lua_State* L, const char* name, void* lib);
	void AddModule(lua_State* L, const char* name);
	int System(lua_State* L);
}