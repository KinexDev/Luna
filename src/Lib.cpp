#include "../include/Lib.h"

std::vector<HMODULE> Lib::LoadedLibs;

Lib::Lib(HMODULE lib)
{
	loadedLib = lib;
}

void Lib::Register(lua_State* L)
{
	lua_newtable(L);
	lua_pushcfunction(L, &Load, "load");
	lua_setfield(L, -2, "load");
	lua_setglobal(L, "lib");
}

int Lib::Load(lua_State* L)
{
	if (!lua_isstring(L, 1))
	{
		lua_pushstring(L, "did not specify library location!");
		lua_error(L);
	}

	const char* library = lua_tostring(L, 1);

	HMODULE loadedLib = LoadLibraryA(library);

	if (loadedLib == nullptr)
	{
		lua_pushstring(L, "failed to load library");
		lua_error(L);
	}

	LoadedLibs.push_back(loadedLib);

	PUSH_USERDATA(L, Lib, loadedLib);
	return 1;
}

int Lib::Extern(lua_State* L)
{
	if (!lua_isuserdata(L, 1) && !lua_isstring(L, 2))
	{
		lua_pushstring(L, "incorrect arguments!");
		lua_error(L);
	}

	Lib* lib = (Lib*)lua_touserdata(L, 1);
	const char* function = lua_tostring(L, 2);

	if (lib == nullptr)
	{
		lua_pushstring(L, "lib is null!");
		lua_error(L);
	}

	lua_CFunction CFunction = (lua_CFunction)GetProcAddress(lib->loadedLib, function);
	
	if (CFunction == nullptr)
	{
		lua_pushstring(L, "function doesn't exist!");
		lua_error(L);
	}

	lua_pushcfunction(L, CFunction, function);
	return 1;
}

int Lib::Index(lua_State* L)
{
	const char* name = lua_tostring(L, 2);

	if (strcmp(name, "extern") == 0)
	{
		lua_pushcfunction(L, &Extern, "extern");
		return 1;
	}

	return 0;
}
