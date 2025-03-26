﻿#include "../include/main.h"

void help() 
{
    printf("Usage: luau.exe [options]\n");
    printf("Options:\n");
    printf("    --run <script>      Runs the specified Luau script.\n");
    printf("    --version           Show version information.\n");
    printf("    --help              Shows this message.\n");
}

void version()
{
    printf("luau: 0.6.6\n");
    printf("luauRuntime: 0.0.1\n");
}

int main(int argc, char* argv[]) 
{
    if (argc < 2) 
    {
        help();
        return 0;
    }

    if (strcmp(argv[1], "--run") == 0)
    {
        if (argc < 3)
        {
            printf("incorrect usage of --run\n");
            help();
            return 0;
        }
        
        try 
        {
            LuauVM vm;
            vm.DoFile(argv[2]);
        }
        catch (std::exception& e) 
        {
            std::cout << e.what() << "\n";
            system("pause");
        }

        return 0;
    }
    else if (strcmp(argv[1], "--version") == 0)
    {
        version();
        return 0;
    }
    else if (strcmp(argv[1], "--help") == 0)
    {
        help();
        return 0;
    }

    help();
}
