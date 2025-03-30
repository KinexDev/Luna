#pragma once
#include "lualib.h"
#include <luacode.h>
#include <iostream> 
#include <functional>
#include <fstream>
#include <sstream>
#include "File.h"

class LuauVM {
public:
	int DoString(const std::string &source, int results = 0);
	int DoFile(const std::string filePath, int results = 0);
	void PushGlobalFunction(const std::string &name, const lua_CFunction &function);
	int Execute(int nargs = 0, int results = 0);
	void PushFunction(const lua_CFunction& function);
	LuauVM();
	~LuauVM();
	static std::filesystem::path directory;
	static std::unordered_map<std::string, std::vector<int>> cachedRequires;
	lua_State* L;
	static void UserdataDestructor(void* userdata);
	static int Require(lua_State* L);
	static int System(lua_State* L);
	static int IndexFunction(lua_State* L);
	static int NewIndexFunction(lua_State* L);
	static int AddFunction(lua_State* L);
	static int SubFunction(lua_State* L);
	static int MulFunction(lua_State* L);
	static int DivFunction(lua_State* L);
	static int ToStringFunction(lua_State* L);
	static int ConcatFunction(lua_State* L);
	static int EqFunction(lua_State* L);
	static int LtFunction(lua_State* L);
	static int LeFunction(lua_State* L);
};

#define PUSH_FUNCTION_AS_TABLE_KEY(L, func, name) \
    PUSH_FUNCTION(L, func); \
    lua_setfield(L, -2, name)

#define PUSH_USERDATA(L, T, ...) \
	{ \
		void* luaUserdata = lua_newuserdatadtor(L, sizeof(T), &LuauVM::UserdataDestructor); \
		new (luaUserdata) T(__VA_ARGS__); \
		lua_newtable(L); \
		PUSH_FUNCTION_AS_TABLE_KEY(L, &LuauVM::IndexFunction, "__index"); \
		PUSH_FUNCTION_AS_TABLE_KEY(L, &LuauVM::NewIndexFunction, "__newindex"); \
		PUSH_FUNCTION_AS_TABLE_KEY(L, &LuauVM::AddFunction, "__add"); \
		PUSH_FUNCTION_AS_TABLE_KEY(L, &LuauVM::SubFunction, "__sub"); \
		PUSH_FUNCTION_AS_TABLE_KEY(L, &LuauVM::MulFunction, "__mul"); \
		PUSH_FUNCTION_AS_TABLE_KEY(L, &LuauVM::DivFunction, "__div"); \
		PUSH_FUNCTION_AS_TABLE_KEY(L, &LuauVM::ToStringFunction, "__tostring"); \
		PUSH_FUNCTION_AS_TABLE_KEY(L, &LuauVM::ConcatFunction, "__concat"); \
		PUSH_FUNCTION_AS_TABLE_KEY(L, &LuauVM::EqFunction, "__eq"); \
		PUSH_FUNCTION_AS_TABLE_KEY(L, &LuauVM::LtFunction, "__lt"); \
		PUSH_FUNCTION_AS_TABLE_KEY(L, &LuauVM::LeFunction, "__le"); \
		lua_setmetatable(L, -2);\
	} 

#define PUSH_FUNCTION(L, func) \
	lua_pushcclosurek(L, func, "", 0, nullptr);

#define PUSH_GLOBAL_USERDATA(L, name, T, ...) \
	lua_pushvalue(L, LUA_GLOBALSINDEX); \
	PUSH_USERDATA(L, T, __VA_ARGS__); \
	lua_setglobal(L, name.c_str()); \
	lua_pop(L, 1); \