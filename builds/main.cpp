#include "include/LuauVM.h"
#include <filesystem>
#include "include/HexEncoder.h"
#include "luaucode.h"
#ifdef LUAU_BUILD_WIN
#include <windows.h>
#endif

int logic()
{
	for (auto& x : LuauCode::requiredScripts)
	{
		std::filesystem::path fileNamePath(x.first);
		std::string fileName = fileNamePath.filename().string();

		std::string code = HexEncoder::DecodeFromHex(x.second);
		LuauVM::requiredScripts[fileName] = code;
	}

	LuauVM vm;

	try
	{
		std::string main = HexEncoder::DecodeFromHex(LuauCode::main);
		vm.DoBytecode(main.c_str());
	}
	catch (std::exception& e)
	{
		std::cout << e.what() << "\n";

#ifndef LUAU_BUILD_WIN
		system("pause");
#endif
	}
	return 0;
}

#ifdef LUAU_BUILD_WIN
static int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	PSTR lpCmdLine, int nCmdShow)
{
	logic();
	return 0;
}
#else
int main()
{
	logic();
}
#endif