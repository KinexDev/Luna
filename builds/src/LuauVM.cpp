#include "../include/LuauVM.h"
#include "../include/Lib.h"
#include "../include/Userdata.h"
#include <unordered_map>

std::unordered_map<std::string, std::vector<int>> LuauVM::cachedRequires;
std::filesystem::path LuauVM::directory;
std::unordered_map<std::string, std::string> LuauVM::requiredScripts;

int LuauVM::DoString(const std::string& source, int results)
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

int LuauVM::DoFile(const std::string filePath, int results)
{
	std::ifstream file(filePath);

	std::stringstream buffer;
	buffer << file.rdbuf();
	std::string scriptContent = buffer.str();
	return DoString(scriptContent, results);
}

int LuauVM::DoBytecode(const char* bytecode, int results)
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

void LuauVM::PushGlobalFunction(const std::string& name, const lua_CFunction& function)
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
	directory = std::filesystem::current_path();
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
	std::string scriptContent;

	if (filePath.find(".luau") == std::string::npos) {
		filePath += ".luau";
	}

	auto originalDirectory = directory;

	std::filesystem::path fileNamePath(filePath);
	std::string fileName = fileNamePath.filename().string();
	std::string abspathStr = fileName;

	lua_settop(L, 0);

	if (requiredScripts.find(fileName) != requiredScripts.end())
	{
		scriptContent = requiredScripts[fileName];
	}
	else
	{
		lua_pushstring(L, "Script wasn't bundled into executable?");
		lua_error(L);
	}

	if (cachedRequires.find(abspathStr) != cachedRequires.end())
	{
		directory = originalDirectory;
		auto refVector = cachedRequires[abspathStr];

		for (int ref : refVector)
		{
			lua_getref(L, ref);
			lua_pushvalue(L, -1);
		}

		return lua_gettop(L);
	}

	const char* sourceCstr = scriptContent.c_str();

	if (!luau_load(L, filePath.c_str(), sourceCstr, strlen(sourceCstr), 0))
	{
		if (lua_pcall(L, 0, LUA_MULTRET, 0))
		{
			const char* error = lua_tostring(L, 1);
			std::cout << error << "\n";
			directory = originalDirectory;
			return 1;
		}
	}
	else
	{
		std::cerr << "Error loading the script!";
		directory = originalDirectory;
		return 1;
	}

	int numResults = lua_gettop(L);

	lua_newtable(L);

	// ran out of ideas ;( but it works.
	std::vector<int> refMap;
	for (int i = 1; i < lua_gettop(L); i++)
	{
		int ref = lua_ref(L, i);
		refMap.push_back(ref);
	}

	cachedRequires[abspathStr] = refMap;
	directory = originalDirectory;
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