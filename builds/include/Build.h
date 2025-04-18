#pragma once

extern "C" {
#include "lualib.h"
#include "luacode.h"
}
#include <iostream> 
#include <fstream>
#include <sstream>
#include <filesystem>

namespace Build {
	int Build(std::string& exePathStr, std::string& buildPathStr);
	std::string Compile(std::string& source);
	void ReplaceAll(std::string& str, const std::string& from, const std::string& to);
	std::string EncodeToHex(const std::string& str);
	void CreateProject(std::string& name);
}