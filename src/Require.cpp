#include "../include/Require.h"

#ifdef LUAU_IS_BUILD
std::unordered_map<std::string, std::string> Require::requiredScripts;
#endif
std::unordered_map<std::string, std::vector<int>> Require::cachedRequires;
std::filesystem::path Require::directory;

int Require::Require(lua_State* L)
{
	std::string filePath = std::string(lua_tostring(L, 1));
	std::string scriptContent;

	if (filePath.find(".luau") == std::string::npos) {
		filePath += ".luau";
	}

	auto originalDirectory = directory;

	lua_settop(L, 0);


#ifdef LUAU_IS_BUILD
	std::filesystem::path fileNamePath(filePath);
	std::string fileName = fileNamePath.filename().string();
	std::string abspathStr = fileName;

	if (requiredScripts.find(fileName) != requiredScripts.end())
	{
		scriptContent = requiredScripts[fileName];
	}
	else
	{
		lua_pushstring(L, "Script wasn't bundled into executable?");
		lua_error(L);
	}

	const char* bytecode = scriptContent.c_str();
	size_t bytecodeSize = strlen(sourceCstr);
#else
	auto relativePath = std::filesystem::path(filePath);
	auto abspath = directory / relativePath;

	directory = abspath.parent_path().string();

	std::ifstream file(abspath);
	if (!file.good())
	{
		directory = originalDirectory;
		std::cout << abspath.string() << std::endl;
		lua_pushnil(L);
		return 0;
	}

	std::stringstream buffer;
	buffer << file.rdbuf();
	scriptContent = buffer.str();

	std::string abspathStr = abspath.string();

	size_t bytecodeSize = 0;
	const char* sourceCstr = scriptContent.c_str();

	char* bytecode = luau_compile(sourceCstr, strlen(sourceCstr), nullptr, &bytecodeSize);
#endif

	if (cachedRequires.find(abspathStr) != cachedRequires.end())
	{
		directory = originalDirectory;
		auto refVector = cachedRequires[abspathStr];

		for (int ref : refVector)
		{
			lua_getref(L, ref);
		}

		return lua_gettop(L);
	}

	if (!luau_load(L, filePath.c_str(), bytecode, bytecodeSize, 0))
	{
		if (lua_pcall(L, 0, LUA_MULTRET, 0))
		{
			const char* error = lua_tostring(L, 1);
			std::cout << error << "\n";
			free(bytecode);
			directory = originalDirectory;
			return 1;
		}
	}
	else
	{
		size_t len;
		const char* msg = lua_tolstring(L, -1, &len);

		std::string error(msg, len);
		lua_pop(L, 1);
		free(bytecode);
		directory = originalDirectory;
		throw std::runtime_error(error);
	}

	free(bytecode);

	int numResults = lua_gettop(L);

	std::vector<int> refMap;

	for (int i = 1; i <= numResults; i++)
	{
		int ref = lua_ref(L, i);
		refMap.push_back(ref);
	}


	cachedRequires[abspathStr] = refMap;
	directory = originalDirectory;
	return numResults;
}