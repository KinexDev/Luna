#include "../include/File.h"

void File::Register(lua_State* L)
{
	lua_newtable(L);
	lua_pushcfunction(L, &ReadAllText, "read_all_text");
	lua_setfield(L, -2, "read_all_text");
	lua_pushcfunction(L, &WriteAllText, "write_all_text");
	lua_setfield(L, -2, "write_all_text");
	lua_pushcfunction(L, &Exists, "exists");
	lua_setfield(L, -2, "exists");
	lua_pushcfunction(L, &Delete, "delete");
	lua_setfield(L, -2, "delete");
	lua_pushcfunction(L, &CreateDirectory, "create_directory");
	lua_setfield(L, -2, "create_directory");
	lua_pushcfunction(L, &DeleteDirectory, "delete_directory");
	lua_setfield(L, -2, "delete_directory");
	lua_pushcfunction(L, &ExistsDirectory, "exists_directory");
	lua_setfield(L, -2, "exists_directory");
	lua_setglobal(L, "file");
}

int File::ReadAllText(lua_State* L)
{
	const char* filePath = lua_tostring(L, 1);
	std::ifstream file(filePath);
	if (!file.good())
	{
		lua_pushnil(L);
		return 1;
	}
	std::stringstream buffer;
	buffer << file.rdbuf();
	std::string scriptContent = buffer.str();
	lua_pushstring(L, scriptContent.c_str());
	return 1;
}

int File::WriteAllText(lua_State* L)
{
	const char* filePath = lua_tostring(L, 1);
	const char* content = lua_tostring(L, 2);

	std::ofstream file(filePath);
	if (!file) {
		lua_pushboolean(L, false);
		return 1;
	}

	file << content;
	lua_pushboolean(L, true);
	return 0;
}

int File::Exists(lua_State* L)
{
	const char* filePath = lua_tostring(L, 1);

	std::ifstream file(filePath);
	lua_pushboolean(L, file.good());
	return 1;
}

int File::Delete(lua_State* L)
{
	const char* filePath = lua_tostring(L, 1);
	if (std::remove(filePath) != 0)
		lua_pushboolean(L, false);
	else
		lua_pushboolean(L, true);
	return 1;
}

int File::CreateDirectory(lua_State* L)
{
	const char* dirPath = lua_tostring(L, 1);
	
	try {
		if (std::filesystem::create_directory(dirPath))
			lua_pushboolean(L, true);
		else
			lua_pushboolean(L, false);
	}
	catch (const std::exception& e) {
		lua_pushboolean(L, false);
	}

	return 1;
}

int File::DeleteDirectory(lua_State* L)
{
	const char* dirPath = lua_tostring(L, 1);

	try {
		if (std::filesystem::remove(dirPath))
			lua_pushboolean(L, true);
		else
			lua_pushboolean(L, false);
	}
	catch (const std::exception& e) {
		lua_pushboolean(L, false);
	}

	return 1;
}

int File::ExistsDirectory(lua_State* L)
{
	const char* path = lua_tostring(L, 1);
	std::filesystem::path dirPath(path);
	lua_pushboolean(L, std::filesystem::is_directory(dirPath));
	return 1;
}
