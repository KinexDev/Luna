#pragma once
extern "C" {
#include "lualib.h"
}
#include <fstream>
#include <sstream>
#include <filesystem>

namespace File {
	void Register(lua_State* L);
	int ReadAllText(lua_State* L);
	int WriteAllText(lua_State* L);
	int Exists(lua_State* L);
	int Delete(lua_State* L);
	int CreateDirectory(lua_State* L);
	int DeleteDirectory(lua_State* L);
	int ExistsDirectory(lua_State* L);
	int GetFileExtension(lua_State* L);
	int CopyFile(lua_State* L);
	int MoveFile(lua_State* L);
	int ListFilesInDirectory(lua_State* L);
	int ListDirectoriesInDirectory(lua_State* L);
	int CopyDirectory(lua_State* L);
	int MoveDirectory(lua_State* L);
}