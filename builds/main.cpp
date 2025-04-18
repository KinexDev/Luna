#include "include/Require.h"
#include "include/Runtime.h"
#include "luaucode.h"
#ifdef LUAU_BUILD_WIN
#include <windows.h>
#endif

std::string DecodeFromHex(const std::vector<unsigned char>& hex)
{
	std::string decodedStr;
	decodedStr.reserve(hex.size());

	for (size_t i = 0; i < hex.size(); ++i)
	{
		decodedStr += static_cast<char>(hex[i]);
	}

	return decodedStr;
}

int RunVM()
{
	for (auto& x : LuauCode::requiredScripts)
	{
		std::filesystem::path fileNamePath(x.first);
		std::string fileName = fileNamePath.filename().string();

		std::string code = DecodeFromHex(x.second);
		Require::requiredScripts[fileName] = code;
	}

	Runtime vm;

	try
	{
		std::string main = DecodeFromHex(LuauCode::main);
		vm.DoBytecode(main.c_str());
	}
	catch (std::exception& e)
	{
		std::cout << e.what() << "\n";
	}
	return 0;
}

#ifdef LUAU_BUILD_WIN
static int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	PSTR lpCmdLine, int nCmdShow)
{
	return RunVM();
}
#else
int main()
{
	return RunVM();
}
#endif