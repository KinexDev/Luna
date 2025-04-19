#include "../include/Runtime.h"
#include "../include/std/Lib.h"
#include "../include/std/std.h"
#include "../include/Require.h"

#ifndef LUAU_IS_BUILD
int Runtime::DoString(const std::string& source, int results)
{
	size_t bytecodeSize = 0;

	const char* sourceCstr = source.c_str();
	char* bytecode = luau_compile(sourceCstr, strlen(sourceCstr), nullptr, &bytecodeSize);

	if (!luau_load(L, "Main", bytecode, bytecodeSize, 0))
	{
		if (lua_pcall(L, 0, results, 0))
		{
			std::string error = std::string(lua_tostring(L, 1));
			lua_pop(L, 1);
			free(bytecode);
			throw std::runtime_error(error);
		}
	}
	else
	{
		size_t len;
		const char* msg = lua_tolstring(L, -1, &len);

		std::string error(msg, len);
		lua_pop(L, 1);
		free(bytecode);
		throw std::runtime_error(error);
	}

	free(bytecode);
	return 0;
}

int Runtime::DoFile(const std::string filePath, int results)
{
	std::ifstream file(filePath);

	std::stringstream buffer;
	buffer << file.rdbuf();
	std::string scriptContent = buffer.str();
	return DoString(scriptContent, results);
}
#endif

int Runtime::DoBytecode(const char* bytecode, int results)
{
	if (!luau_load(L, "Main", bytecode, strlen(bytecode), 0))
	{
		if (lua_pcall(L, 0, results, 0))
		{
			std::string error = std::string(lua_tostring(L, 1));
			lua_pop(L, 1);
			throw std::runtime_error(error);
		}
	}
	else
	{
		size_t len;
		const char* msg = lua_tolstring(L, -1, &len);

		std::string error(msg, len);
		lua_pop(L, 1);
		throw std::runtime_error(error);
	}

	return 0;
}

int Runtime::Execute(int nargs, int results)
{
	int res = lua_pcall(L, nargs, results, 0);

	if (res)
	{
		const char* error = lua_tostring(L, 1);
		std::cout << error << "\n";
	}

	return res;
}

Runtime::Runtime(bool useRequire)
{
	L = luaL_newstate();
	luaL_openlibs(L);
	std::Load(L);
	
	if (useRequire) 
	{
		lua_pushcfunction(L, &Require::Require, "require");
		lua_setglobal(L, "require");
	}

	lua_pushvalue(L, LUA_GLOBALSINDEX);
#ifdef _WIN32
	lua_pushstring(L, "windows");
#elif __linux__ 
	lua_pushstring(L, "linux");
#elif __APPLE__
	lua_pushstring(L, "apple");
#endif
	lua_setglobal(L, "platform");
	lua_pop(L, 1);

	lua_pushvalue(L, LUA_GLOBALSINDEX);

	lua_pop(L, 1);
}

Runtime::~Runtime()
{
	std::Unload(L);
	lua_close(L);
}