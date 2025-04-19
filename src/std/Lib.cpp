#include "../../include/std/Lib.h"

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
	dylib loadedLib = loadlib(library);

	if (loadedLib == nullptr)
	{
		std::string msg = "failed to load library ";
		msg += library;
#if __linux__ || __APPLE__
		msg += ", reason : ";
		msg += dlerror();
#endif
		lua_pushstring(L, msg.c_str());
		lua_error(L);
	}

#if __linux__ || __APPLE__
	dlerror();
#endif

	std::AddLib(L, library, loadedLib);
	lua_pushlightuserdata(L, loadedLib);
	lua_newtable(L);
	lua_pushcfunction(L, &Index, "index");
	lua_setfield(L, -2, "__index");
	lua_setmetatable(L, -2);
	return 1;
}

int Lib::Extern(lua_State* L)
{
	if (!lua_isuserdata(L, 1) && !lua_isstring(L, 2))
	{
		lua_pushstring(L, "incorrect arguments!");
		lua_error(L);
	}

	dylib lib = (dylib)lua_touserdata(L, 1);
	const char* function = lua_tostring(L, 2);

	if (lib == nullptr)
	{
		lua_pushstring(L, "lib is null!");
		lua_error(L);
	}
#ifdef _WIN32
	lua_CFunction CFunction = (lua_CFunction)GetProcAddress(lib, function);
	if (CFunction == nullptr)
	{
		std::string msg = "function doesn't exist! ";
		msg += function;
		lua_pushstring(L, msg.c_str());
		lua_error(L);
	}
#elif __linux__ || __APPLE__
	lua_CFunction CFunction = (lua_CFunction)dlsym(lib, function);
	const char* error = dlerror();
	if (error != nullptr) 
	{
		lua_pushstring(L, error);
		lua_error(L);
	}
#endif

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
