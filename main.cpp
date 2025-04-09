#include "include/LuauVM.h"
#include <filesystem>
#include "include/HexEncoder.h"

void help() 
{
    printf("Usage: luau.exe [options]\n");
    printf("Options:\n");
    printf("    --run <script>      Runs the specified Luau script.\n");
    printf("    --compile <script>  Compiles the specified script to bytecode.\n");
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
            std::ifstream file(argv[2]);
            if (!file.good())
            {
                printf("script doesn't exist!");
                return 0;
            }

            auto lastPath = std::filesystem::current_path();
            auto relativePath = std::filesystem::path(argv[2]);
            auto abspath = lastPath / relativePath;

            LuauVM vm;
            LuauVM::directory = abspath.parent_path();
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
    else if (strcmp(argv[1], "--compile") == 0)
    {
        std::filesystem::path filePath(argv[2]);

        std::ifstream file(filePath);

        if (!file.good())
        {
            printf("script doesn't exist!");
            return 0;
        }

        size_t bytecodeSize = 0;

        std::stringstream buffer;
        buffer << file.rdbuf();
        std::string source = buffer.str();
        const char* sourceCstr = source.c_str();
        char* bytecode = luau_compile(sourceCstr, strlen(sourceCstr), nullptr, &bytecodeSize);

        std::string compiledBytecode = std::string(bytecode, bytecodeSize);

        std::string compiledPath = filePath.stem().string() + ".luauc";
        std::ofstream compiledFile(compiledPath);

        if (file) {
            compiledFile << compiledBytecode;
            std::cout << "successfully compiled to bytecode!";
        }

        free(bytecode);
        return 0;
    }

    help();
}
