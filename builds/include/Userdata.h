#pragma once
#include "lualib.h"
#include "iostream"
#include "LuauVM.h"

class LuauVM;

/// <summary>
/// template class for all userdata's, used to inherit metamethods.
/// </summary>
class Userdata {
public:
    virtual int Index(lua_State* L);
    virtual int NewIndex(lua_State* L);
    virtual int Add(lua_State* L);
    virtual int Sub(lua_State* L);
    virtual int Mul(lua_State* L);
    virtual int Div(lua_State* L);
    virtual int ToString(lua_State* L);
    virtual int Concat(lua_State* L);
    virtual int Eq(lua_State* L);
    virtual int Lt(lua_State* L);
    virtual int Le(lua_State* L);
    virtual ~Userdata();
};