#include "../../include/std/Filesystem.h"

// no safety rn (mockup)

void Filesystem::Register(lua_State* L)
{
	lua_newtable(L);
	lua_pushcfunction(L, &Read, "read");
	lua_setfield(L, -2, "read");
	lua_pushcfunction(L, &Write, "write");
	lua_setfield(L, -2, "write");
	lua_pushcfunction(L, &IsFile, "isFile");
	lua_setfield(L, -2, "isFile");
	lua_pushcfunction(L, &Move, "move");
	lua_setfield(L, -2, "move");
	lua_pushcfunction(L, &Remove, "remove");
	lua_setfield(L, -2, "remove");
	lua_pushcfunction(L, &RemoveDir, "removeDir");
	lua_setfield(L, -2, "removeDir");
	lua_setglobal(L, "fs");
}

int Filesystem::Read(lua_State* L)
{
	const char* filePath = lua_tostring(L, 1);
	std::ifstream file(filePath);

	std::stringstream buffer;
	buffer << file.rdbuf();

	std::string scriptContent = buffer.str();
	lua_pushstring(L, scriptContent.c_str());
	return 1;
}

int Filesystem::ReadDir(lua_State* L)
{
	const char* dirPath = lua_tostring(L, 1);
	lua_newtable(L);
	
	int index = 1;
	for (const auto& entry : std::filesystem::directory_iterator(dirPath)) {
		std::string pathStr = entry.path().string();
		lua_pushstring(L, pathStr.c_str());
		lua_rawseti(L, -2, index);
		index++;
	}
	return 1;
}

int Filesystem::Write(lua_State* L)
{
	const char* filePath = lua_tostring(L, 1);
	const char* content = lua_tostring(L, 2);

	std::ofstream file(filePath);
	file << content;
	lua_pushboolean(L, true);
	return 0;
}

int Filesystem::WriteDir(lua_State* L)
{
	const char* dirPath = lua_tostring(L, 1);
	std::filesystem::create_directory(dirPath);
	return 0;
}

int Filesystem::IsFile(lua_State* L)
{
	const char* filePath = lua_tostring(L, 1);
	lua_pushboolean(L, (std::filesystem::exists(filePath) && std::filesystem::is_regular_file(filePath)));
	return 1;
}

int Filesystem::IsDir(lua_State* L)
{
	const char* dirPath = lua_tostring(L, 1);
	lua_pushboolean(L, (std::filesystem::exists(dirPath) && std::filesystem::is_directory(dirPath)));
	return 1;
}

int Filesystem::Move(lua_State* L)
{
	const char* curPath = lua_tostring(L, 1);
	const char* destPath = lua_tostring(L, 2);
	std::filesystem::rename(curPath, destPath);
	return 0;
}

int Filesystem::Remove(lua_State* L)
{
	const char* filePath = lua_tostring(L, 1);
	std::filesystem::remove(filePath);
	return 0;
}

int Filesystem::RemoveDir(lua_State* L)
{
	const char* dirPath = lua_tostring(L, 1);
	std::filesystem::remove_all(dirPath);
	return 0;
}

int Filesystem::Copy(lua_State* L)
{
	const char* copyTargetPath = lua_tostring(L, 1);
	const char* destPath = lua_tostring(L, 2);
	std::filesystem::copy(copyTargetPath, destPath);
	return 0;
}