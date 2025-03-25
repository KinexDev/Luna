#pragma once
#include "lualib.h"
#include "iostream"
#include <vector>
#include "Windows.h"
#include "Userdata.h"

class Lib : public Userdata {
public:
	Lib(HMODULE lib);
	static std::vector<HMODULE> LoadedLibs;
	static void Register(lua_State* L);
	static int Load(lua_State* L);
	static int Extern(lua_State* L);
	int Index(lua_State* L) override;
	HMODULE loadedLib;
};