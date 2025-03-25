#include "../include/LuauVM.h"
#include "../include/Lib.h"
#include "../include/Userdata.h"

int LuauVM::DoString(const std::string &source, int results)
{
	size_t bytecodeSize = 0;

	const char* sourceCstr = source.c_str();
	char* bytecode = luau_compile(sourceCstr, strlen(sourceCstr), nullptr, &bytecodeSize);

	if (!luau_load(L, "Script", bytecode, bytecodeSize, 0))
	{
		if (lua_pcall(L, 0, results, 0))
		{
			const char* error = lua_tostring(L, 1);
			std::cout << error << "\n";
			delete[] bytecode;
			return 1;
		}
	}
	else
	{
		std::cerr << "Error loading the script, this could be due to bad syntax.";
		delete[] bytecode;
		return 1;
	}

	delete[] bytecode;
	return 0;
}

int LuauVM::DoFile(const std::string filePath, int results)
{
	std::ifstream file(filePath);

	std::stringstream buffer;
	buffer << file.rdbuf();
	std::string scriptContent = buffer.str();
	return DoString(scriptContent, results);
}

void LuauVM::PushGlobalFunction(const std::string &name, const lua_CFunction& function)
{
	const char* nameCstr = name.c_str();

	lua_pushvalue(L, LUA_GLOBALSINDEX);
	PushFunction(function);
	lua_setglobal(L, nameCstr);
	lua_pop(L, 1);
}

int LuauVM::Execute(int nargs, int results)
{
	int res = lua_pcall(L, nargs, results, 0);

	if (res)
	{
		const char* error = lua_tostring(L, 1);
		std::cout << error << "\n";
	}

	return res;
}

void LuauVM::PushFunction(const lua_CFunction& function)
{
	lua_pushcclosurek(L, function, "", 0, nullptr);
}

LuauVM::LuauVM()
{
	L = luaL_newstate();
	luaL_openlibs(L);
	Lib::Register(L);
	File::Register(L);
	PushGlobalFunction("require", &Require);
	PushGlobalFunction("system", &System);
}

LuauVM::~LuauVM()
{
	lua_close(L);

	for (auto lib : Lib::LoadedLibs)
	{
		FreeLibrary(lib);
	}

	Lib::LoadedLibs.clear();
}

void LuauVM::UserdataDestructor(void* userdata)
{
	Userdata* userdataPtr = (Userdata*)(userdata);
	userdataPtr->~Userdata();
}

int LuauVM::Require(lua_State* L)
{
	std::string filePath = std::string(lua_tostring(L, 1));
	
	if (filePath.find(".luau") == std::string::npos) {
		filePath += ".luau";
	}

	lua_settop(L, 0);
	std::ifstream file(filePath);
	
	if (!file.good())
	{
		lua_pushnil(L);
		return 0;
	}

	std::stringstream buffer;
	buffer << file.rdbuf();
	std::string scriptContent = buffer.str();

	size_t bytecodeSize = 0;

	const char* sourceCstr = scriptContent.c_str();
	char* bytecode = luau_compile(sourceCstr, strlen(sourceCstr), nullptr, &bytecodeSize);

	if (!luau_load(L, "", bytecode, bytecodeSize, 0))
	{
		if (lua_pcall(L, 0, LUA_MULTRET, 0))
		{
			const char* error = lua_tostring(L, 1);
			std::cout << error << "\n";
			delete[] bytecode;
			return 1;
		}
	}
	else
	{
		std::cerr << "Error loading the script!";
		delete[] bytecode;
		return 1;
	}

	delete[] bytecode;

	return lua_gettop(L);
}

int LuauVM::System(lua_State* L)
{
	const char* command = lua_tostring(L, 1);
	return system(command);
}

int LuauVM::IndexFunction(lua_State* L)
{
	Userdata* userdata = (Userdata*)(lua_touserdata(L, 1));
	if (userdata == nullptr)
	{
		return 0;
	}
	return userdata->Index(L);
}

int LuauVM::NewIndexFunction(lua_State* L)
{
	Userdata* userdata = (Userdata*)(lua_touserdata(L, 1));
	if (userdata == nullptr)
	{
		return 0;
	}
	return userdata->NewIndex(L);
}

int LuauVM::AddFunction(lua_State* L)
{
	Userdata* userdata = (Userdata*)(lua_touserdata(L, 1));
	if (userdata == nullptr)
	{
		return 0;
	}
	return userdata->Add(L);
}

int LuauVM::SubFunction(lua_State* L)
{
	Userdata* userdata = (Userdata*)(lua_touserdata(L, 1));
	if (userdata == nullptr)
	{
		return 0;
	}
	return userdata->Sub(L);
}

int LuauVM::MulFunction(lua_State* L)
{
	Userdata* userdata = (Userdata*)(lua_touserdata(L, 1));
	if (userdata == nullptr)
	{
		return 0;
	}
	return userdata->Mul(L);
}

int LuauVM::DivFunction(lua_State* L)
{
	Userdata* userdata = (Userdata*)(lua_touserdata(L, 1));
	if (userdata == nullptr)
	{
		return 0;
	}
	return userdata->Div(L);
}

int LuauVM::ToStringFunction(lua_State* L)
{
	Userdata* userdata = (Userdata*)(lua_touserdata(L, 1));
	if (userdata == nullptr)
	{
		return 0;
	}
	return userdata->ToString(L);
}

int LuauVM::ConcatFunction(lua_State* L)
{
	Userdata* userdata = (Userdata*)(lua_touserdata(L, 1));
	if (userdata == nullptr)
	{
		return 0;
	}
	return userdata->Concat(L);
}

int LuauVM::EqFunction(lua_State* L)
{
	Userdata* userdata = (Userdata*)(lua_touserdata(L, 1));
	if (userdata == nullptr)
	{
		return 0;
	}
	return userdata->Eq(L);
}

int LuauVM::LtFunction(lua_State* L)
{
	Userdata* userdata = (Userdata*)(lua_touserdata(L, 1));
	if (userdata == nullptr)
	{
		return 0;
	}
	return userdata->Lt(L);
}

int LuauVM::LeFunction(lua_State* L)
{
	Userdata* userdata = (Userdata*)(lua_touserdata(L, 1));
	if (userdata == nullptr)
	{
		return 0;
	}
	return userdata->Le(L);
}