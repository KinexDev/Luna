#pragma once
#include "lualib.h"
#include "iostream"
#include <vector>
#include "Windows.h"
#include "Userdata.h"
#include <fstream>

#ifdef _WIN32
#define dylib HMODULE
#define loadlib(path) LoadLibraryA(path)
#define closelib FreeLibrary
#elif __linux__ || __APPLE__
#define dylib void*
#define loadlib(path) dlopen(path, RTLD_LAZY)
#define closelib dlclose
#endif


class Lib : public Userdata {
public:
	Lib(dylib lib);
	static std::vector<dylib> LoadedLibs;
	static void Register(lua_State* L);
	static int Load(lua_State* L);
	static int Extern(lua_State* L);
	int Index(lua_State* L) override;
	dylib loadedLib;
};