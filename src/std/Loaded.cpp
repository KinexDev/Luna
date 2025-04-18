#include "../../include/std/Loaded.h"

void Loaded::Load(lua_State* L)
{
    Lib::Register(L);

	lua_newtable(L);
	lua_newtable(L);
	lua_setfield(L, -2, "libs");
	lua_newtable(L);
	lua_setfield(L, -2, "modules");
	lua_setglobal(L, "__loaded");
}

void Loaded::Unload(lua_State* L)
{
    lua_getglobal(L, "__loaded");

    if (lua_istable(L, -1))
    {
        lua_getfield(L, -1, "libs");

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


void Loaded::AddLib(lua_State* L, const char* name, void* lib)
{
    lua_getglobal(L, "__loaded");

    if (lua_istable(L, -1)) 
    {
        lua_getfield(L, -1, "libs");

        if (lua_istable(L, -1)) 
        {
            int count = lua_objlen(L, -1);
            lua_pushlightuserdata(L, lib);
            lua_rawseti(L, -2, count + 1);
        }

        lua_pop(L, 1);
    }
    lua_pop(L, 1);
}