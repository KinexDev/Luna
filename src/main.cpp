#include "../include/main.h"

int main(int argc, char* argv[]) {
    if (argc < 2) 
    {
        std::cerr << "Usage: ./LuauBridge.exe <script.luau>\n";
        system("pause");
        return 1;
    }

	LuauVM vm;
    vm.DoFile(argv[1]);
    system("pause");
}
