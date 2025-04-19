#include "../../include/std/std.h"

void std::Load(lua_State* L)
{
    Lib::Register(L);
    Filesystem::Register(L);

    lua_pushcfunction(L, &System, "system");
    lua_setglobal(L, "system");

    lua_newtable(L);
    lua_newtable(L);
    lua_setfield(L, -2, "loadedLibs");
    lua_newtable(L);
    lua_setfield(L, -2, "loadedModules");
    lua_setglobal(L, "std");
}

void std::Unload(lua_State* L)
{
    lua_getglobal(L, "std");

    if (lua_istable(L, -1))
    {
        lua_getfield(L, -1, "loadedLibs");

        if (lua_istable(L, -1))
        {
            lua_pushnil(L);

            while (lua_next(L, -2) != 0)
            {
                if (lua_islightuserdata(L, -1))
                {
                    closelib((dylib)lua_tolightuserdata(L, -1));
                }

                lua_pop(L, 1);
            }
        }

        lua_pop(L, 1);
    }

    lua_pop(L, 1);
}


void std::AddLib(lua_State* L, const char* name, void* lib)
{
    lua_getglobal(L, "std");

    if (lua_istable(L, -1)) 
    {
        lua_getfield(L, -1, "loadedLibs");

        if (lua_istable(L, -1)) 
        {
            lua_pushlightuserdata(L, lib);
            lua_setfield(L, -2, name);
        }

        lua_pop(L, 1);
    }
    lua_pop(L, 1);
}

int std::System(lua_State* L)
{	
    const char* command = lua_tostring(L, 1);
	return system(command);
}
