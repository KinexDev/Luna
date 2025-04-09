#include "include/LuauVM.h"
#include <filesystem>
#include "include/HexEncoder.h"
#include "luaucode.h"

int main(int argc, char* argv[]) 
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
        system("pause");
    }
}
