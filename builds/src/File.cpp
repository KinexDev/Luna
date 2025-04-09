#include "../include/File.h"

void File::Register(lua_State* L)
{
	lua_newtable(L);
	lua_pushcfunction(L, &ReadAllText, "readAllText");
	lua_setfield(L, -2, "readAllText");
	lua_pushcfunction(L, &WriteAllText, "writeAllText");
	lua_setfield(L, -2, "writeAllText");
	lua_pushcfunction(L, &Exists, "exists");
	lua_setfield(L, -2, "exists");
	lua_pushcfunction(L, &Delete, "delete");
	lua_setfield(L, -2, "delete");
	lua_pushcfunction(L, &CreateDirectory, "createDirectory");
	lua_setfield(L, -2, "createDirectory");
	lua_pushcfunction(L, &DeleteDirectory, "deleteDirectory");
	lua_setfield(L, -2, "deleteDirectory");
	lua_pushcfunction(L, &ExistsDirectory, "existsDirectory");
	lua_setfield(L, -2, "existsDirectory");
	lua_pushcfunction(L, &GetFileExtension, "getFileExtension");
	lua_setfield(L, -2, "getFileExtension");
	lua_pushcfunction(L, &CopyFile, "copyFile");
	lua_setfield(L, -2, "copyFile");
	lua_pushcfunction(L, &MoveFile, "moveFile");
	lua_setfield(L, -2, "moveFile");
	lua_pushcfunction(L, &ListFilesInDirectory, "listFilesInDirectory");
	lua_setfield(L, -2, "listFilesInDirectory");
	lua_pushcfunction(L, &ListDirectoriesInDirectory, "listDirectoriesInDirectory");
	lua_setfield(L, -2, "listDirectoriesInDirectory");
	lua_pushcfunction(L, &CopyDirectory, "copyDirectory");
	lua_setfield(L, -2, "copyDirectory");
	lua_pushcfunction(L, &MoveDirectory, "moveDirectory");
	lua_setfield(L, -2, "moveDirectory");
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
		if (std::filesystem::remove_all(dirPath))
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

int File::GetFileExtension(lua_State* L)
{
	const char* filePath = lua_tostring(L, 1);
	std::filesystem::path path(filePath);

	std::string extension = path.extension().string();

	if (extension.empty())
		lua_pushnil(L);
	else
		lua_pushstring(L, extension.c_str());

	return 1;
}


int File::CopyFile(lua_State* L)
{
	const char* sourcePath = lua_tostring(L, 1);
	const char* destPath = lua_tostring(L, 2);

	try
	{
		std::filesystem::copy(sourcePath, destPath, std::filesystem::copy_options::overwrite_existing);
		lua_pushboolean(L, true);
	}
	catch (const std::exception& e)
	{
		lua_pushboolean(L, false);
	}

	return 1;
}

int File::MoveFile(lua_State* L)
{
	const char* sourcePath = lua_tostring(L, 1);
	const char* destPath = lua_tostring(L, 2);

	try
	{
		std::filesystem::rename(sourcePath, destPath);
		lua_pushboolean(L, true);
	}
	catch (const std::exception& e)
	{
		lua_pushboolean(L, false);
	}

	return 1;
}

int File::ListFilesInDirectory(lua_State* L)
{
	const char* dirPath = lua_tostring(L, 1);
	std::filesystem::path path(dirPath);

	if (!std::filesystem::exists(path) || !std::filesystem::is_directory(path))
	{
		lua_pushnil(L);
		return 1;
	}

	lua_newtable(L);
	int index = 1;

	for (const auto& entry : std::filesystem::directory_iterator(path))
	{
		if (entry.is_regular_file())
		{
			lua_pushstring(L, entry.path().filename().string().c_str());
			lua_rawseti(L, -2, index++);
		}
	}

	return 1;
}

int File::ListDirectoriesInDirectory(lua_State* L)
{
	const char* dirPath = lua_tostring(L, 1);
	std::filesystem::path path(dirPath);

	if (!std::filesystem::exists(path) || !std::filesystem::is_directory(path))
	{
		lua_pushnil(L);
		return 1;
	}

	lua_newtable(L);
	int index = 1;

	for (const auto& entry : std::filesystem::directory_iterator(path))
	{
		if (entry.is_directory())
		{
			lua_pushstring(L, entry.path().filename().string().c_str());
			lua_rawseti(L, -2, index++);
		}
	}

	return 1;
}


int File::CopyDirectory(lua_State* L)
{
	const char* sourceDirPath = lua_tostring(L, 1);
	const char* destDirPath = lua_tostring(L, 2);

	try
	{
		std::filesystem::copy(sourceDirPath, destDirPath, std::filesystem::copy_options::recursive | std::filesystem::copy_options::overwrite_existing);
		lua_pushboolean(L, true);
	}
	catch (const std::exception& e)
	{
		lua_pushboolean(L, false);
	}

	return 1;
}

int File::MoveDirectory(lua_State* L)
{
	const char* sourceDirPath = lua_tostring(L, 1);
	const char* destDirPath = lua_tostring(L, 2);

	try
	{
		std::filesystem::rename(sourceDirPath, destDirPath);
		lua_pushboolean(L, true);
	}
	catch (const std::exception& e)
	{
		lua_pushboolean(L, false);
	}

	return 1;
}
