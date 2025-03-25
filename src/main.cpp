#include "../include/main.h"

int main(int argc, char* argv[]) {
    if (argc < 2) 
    {
        std::cerr << "Usage: LuauRuntime.exe <script.luau>\n";
        system("pause");
        return 1;
    }

    try 
    {
        LuauVM vm;
        vm.DoFile(argv[1]);
    }
    catch (std::exception& e) 
    {
        std::cout << e.what() << "\n";
        system("pause");
    }
}
