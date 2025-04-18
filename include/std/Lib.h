#pragma once
extern "C" {
#include "lualib.h"
}
#include "iostream"
#ifdef _WIN32
#include "Windows.h"
#else
#include <dlfcn.h>
#endif
#include <fstream>
#include "Loaded.h"

#ifdef _WIN32
#define dylib HMODULE
#define loadlib(path) LoadLibraryA(path)
#define closelib FreeLibrary
#else
#define dylib void*
#define loadlib(path) dlopen(path, RTLD_LAZY)
#define closelib dlclose
#endif


namespace Lib {
	void Register(lua_State* L);
	int Load(lua_State* L);
	int Extern(lua_State* L);
	int Index(lua_State* L);
}