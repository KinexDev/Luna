#include "../include/LuauVM.h"
#include "../include/Lib.h"
#include "../include/Userdata.h"

std::unordered_map<std::string, std::vector<int>> LuauVM::cachedRequires;
std::filesystem::path LuauVM::directory = std::filesystem::current_path();

int LuauVM::DoString(const std::string& source, int results)
{
	size_t bytecodeSize = 0;

	const char* sourceCstr = source.c_str();
	char* bytecode = luau_compile(sourceCstr, strlen(sourceCstr), nullptr, &bytecodeSize);

	if (!luau_load(L, "Script", bytecode, bytecodeSize, 0))
	{
		if (lua_pcall(L, 0, results, 0))
		{
			std::string error = std::string(lua_tostring(L, 1));
			lua_pop(L, 1);
			delete[] bytecode;
			throw std::runtime_error(error);
		}
	}
	else
	{
		size_t len;
		const char* msg = lua_tolstring(L, -1, &len);

		std::string error(msg, len);
		lua_pop(L, 1);
		delete[] bytecode;
		throw std::runtime_error(error);
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
}

LuauVM::~LuauVM()
{
	lua_close(L);

	for (auto lib : Lib::LoadedLibs)
	{
		closelib(lib);
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

	auto relativePath = std::filesystem::path(filePath);
	auto abspath = directory / relativePath;
	
	lua_settop(L, 0);

	if (cachedRequires.find(abspath.string()) != cachedRequires.end())
	{
		auto refVector = cachedRequires[abspath.string()];

		for (int ref : refVector)
		{
			lua_getref(L, ref);
			lua_pushvalue(L, -1);
		}

		return lua_gettop(L);
	}

	std::ifstream file(abspath);
	if (!file.good())
	{
		std::cout << abspath.string() << std::endl;
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

	int numResults = lua_gettop(L);

	lua_newtable(L);

	// ran out of ideas ;(
	std::vector<int> refMap;
	for (int i = 1; i < lua_gettop(L); i++)
	{
		int ref = lua_ref(L, i);
		refMap.push_back(ref);
	}

	cachedRequires[abspath.string()] = refMap;
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