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
	lua_pushcfunction(L, &GetFileExtension, "get_file_extension");
	lua_setfield(L, -2, "get_file_extension");
	lua_pushcfunction(L, &CopyFile, "copy_file");
	lua_setfield(L, -2, "copy_file");
	lua_pushcfunction(L, &MoveFile, "move_file");
	lua_setfield(L, -2, "move_file");
	lua_pushcfunction(L, &ListFilesInDirectory, "list_files_in_directory");
	lua_setfield(L, -2, "list_files_in_directory");
	lua_pushcfunction(L, &CopyDirectory, "copy_directory");
	lua_setfield(L, -2, "copy_directory");
	lua_pushcfunction(L, &MoveDirectory, "move_directory");
	lua_setfield(L, -2, "move_directory");
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
