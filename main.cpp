#include "include/LuauVM.h"
#include <filesystem>
#include "include/HexEncoder.h"

void help() 
{
    printf("Usage: luau.exe [options]\n");
    printf("Options:\n");
    printf("    <script>            Runs the specified Luau script.\n");
    printf("    --compile <script>  Compiles the specified script to bytecode.\n");
    printf("    --version           Show version information.\n");
    printf("    --build <script>    Builds a self-contained executable based on the config.\n");
    printf("    --help              Shows this message.\n");
}

void version()
{
    printf("luau: 0.6.6\n");
    printf("luauRuntime: 0.0.1\n");
}

std::string compile(std::string& source)
{
    size_t bytecodeSize = 0;
    const char* sourceCstr = source.c_str();
    char* bytecode = luau_compile(sourceCstr, strlen(sourceCstr), nullptr, &bytecodeSize);

    auto bytecodeStr = std::string(bytecode, bytecodeSize);
    free(bytecode);
    return bytecodeStr;
}

void replaceAll(std::string& str, const std::string& from, const std::string& to) {
    size_t start_pos = 0;
    while ((start_pos = str.find(from, start_pos)) != std::string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length();
    }
}


int main(int argc, char* argv[]) 
{
    if (argc < 2) 
    {
        help();
        return 0;
    }
    
    if (strcmp(argv[1], "--version") == 0)
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

        std::stringstream buffer;
        buffer << file.rdbuf();
        std::string source = buffer.str();

        std::string compiledBytecode = compile(source);

        std::string compiledPath = filePath.stem().string() + ".luauc";
        std::ofstream compiledFile(compiledPath);

        if (file) {
            compiledFile << compiledBytecode;
            std::cout << "successfully compiled to bytecode!";
        }

        return 0;
    }
    else if (strcmp(argv[1], "--build") == 0)
    {
        std::filesystem::path filePath(argv[2]);

        std::ifstream file(filePath);

        if (!file.good())
        {
            printf("script doesn't exist!");
            return 0;
        }

        std::stringstream buffer;
        buffer << file.rdbuf();
        std::string source = buffer.str();

        LuauVM vm;
        vm.DoString(source, 1);
        
        if (lua_istable(vm.L, 1) == 0)
        {
            printf("Not a table!");
            return 0;
        }

        lua_pushstring(vm.L, "name");
        lua_gettable(vm.L, -2);
        const char* name = lua_tostring(vm.L, -1);
        
        lua_pop(vm.L, -2);

        lua_pushstring(vm.L, "main");
        lua_gettable(vm.L, -2);
        const char* main = lua_tostring(vm.L, -1);

        std::filesystem::path mainPath(main);
        std::ifstream mainFile(main);

        if (!mainFile.good())
        {
            printf("main script doesn't exist!");
            return 0;
        }

        std::stringstream mainBuffer;
        mainBuffer << mainFile.rdbuf();

        std::string mainSource = mainBuffer.str();

        std::string mainByteCode = compile(mainSource);

        lua_pop(vm.L, -2);

        lua_pushstring(vm.L, "scripts");

        lua_gettable(vm.L, -2);

        std::unordered_map<std::string, std::string> bytecode;

        if (lua_istable(vm.L, -1) == 1) 
        {
            lua_pushnil(vm.L);

            while (lua_next(vm.L, -2) != 0) 
            {
                if (lua_isstring(vm.L, -1)) 
                {
                    const char* script = lua_tostring(vm.L, -1);

                    std::ifstream scriptFile(script);

                    if (!scriptFile.good())
                    {
                        printf("script doesn't exist!");
                        return 0;
                    }

                    std::stringstream scriptBuffer;
                    scriptBuffer << scriptFile.rdbuf();

                    std::string scriptSource = scriptBuffer.str();
                    std::string scriptByteCode = compile(scriptSource);
                    bytecode[script] = scriptByteCode;
                }

                lua_pop(vm.L, 1);
            }
        }

        std::filesystem::path exePath(argv[0]);
        std::filesystem::path exeDir = exePath.parent_path();
        std::filesystem::path buildDir = exeDir / "builds";
        
        std::filesystem::path outPath = std::filesystem::current_path();
        outPath /= "out";

        auto exeName = std::string(name);
        exeName += ".exe";
        std::filesystem::path executableDestPath = outPath / exeName;

        if (std::filesystem::exists(executableDestPath))
        {
            std::filesystem::remove(executableDestPath);
        }

        std::filesystem::path tempPath = outPath / "temp";

        bool exists = std::filesystem::exists(outPath);

        if (!exists)
        {
            std::filesystem::create_directory(outPath);
            std::filesystem::copy(buildDir.string(), tempPath, std::filesystem::copy_options::recursive | std::filesystem::copy_options::overwrite_existing);
        }

        auto previousWorkingDirectory = std::filesystem::current_path();

        std::filesystem::current_path(tempPath);

        std::filesystem::path codePath = tempPath / "luaucode.h";

        std::ofstream codeFile(codePath);
        std::string code =
            "#pragma once\n"
            "#include \"iostream\" \n"
            "namespace LuauCode { \n"
            "    std::vector<unsigned char> main = {" + HexEncoder::EncodeToHex(mainByteCode) + "}; \n"
            "    std::unordered_map<std::string, std::vector<unsigned char>> requiredScripts = { \n";

        for (auto& x : bytecode)
        {
            code += "{ \"" + x.first + "\", {" + HexEncoder::EncodeToHex(x.second) + "} }, ";
        }

        code[code.size() - 2] = '}';
        code[code.size() - 1] = ';';
        code += "\n";

        code += "}";
        codeFile << code;
        codeFile.close();


        printf("made changes to luaucode\n");

        std::filesystem::path cmakeListsPath = tempPath / "CMakeLists.txt";
        std::ifstream cmakeListsFile(cmakeListsPath);

        if (!cmakeListsFile.good())
        {
            printf("no cmake lists file!");
            return 1;
        }

        if (!exists)
        {
            std::stringstream cmakeListsbuffer;
            cmakeListsbuffer << cmakeListsFile.rdbuf();
            std::string cmakeListsTxt = cmakeListsbuffer.str();
            std::string previousCmakeLists = previousCmakeLists;
            replaceAll(cmakeListsTxt, "\"luau\"", name);

            std::ofstream cmakeListsFileW(cmakeListsPath);
            cmakeListsFileW << cmakeListsTxt;
            cmakeListsFileW.close();
            printf("made changes to cmakelists");
        }

        printf("done! compiling project! \n");

        if (!exists)
        {
            system("cmake -S . -B build");
        }

        system("cmake --build build --config Release");

        std::filesystem::path executablePath = tempPath / "build" / "Release" / exeName;
        std::ifstream executableFile(cmakeListsPath);

        // failed compiling or something.
        if (!executableFile.good())
        {
            printf("failed compiling!");
            return 1;
        }

        //move exe to accessible area.
        std::filesystem::rename(executablePath, executableDestPath);

        printf("moving dependencies! \n");

        lua_pop(vm.L, -2);

        lua_pushstring(vm.L, "dependencies");

        lua_gettable(vm.L, -2);

        std::filesystem::current_path(previousWorkingDirectory);

        if (lua_istable(vm.L, -1) == 1)
        {
            lua_pushnil(vm.L);

            while (lua_next(vm.L, -2) != 0)
            {
                if (lua_isstring(vm.L, -1))
                {
                    const char* path = lua_tostring(vm.L, -1);

                    std::filesystem::path dependecyPath(path);
                    if (!std::filesystem::exists(dependecyPath))
                    {
                        printf("script doesn't exist!");
                        return 0;
                    }

                    auto newPath = outPath / std::filesystem::relative(dependecyPath, previousWorkingDirectory);

                    std::filesystem::copy(dependecyPath, newPath, std::filesystem::copy_options::overwrite_existing);
                }

                lua_pop(vm.L, 1);
            }
        }

        printf("finished! \n");

        lua_pop(vm.L, 1);
        return 0;
    }
    else 
    {
        try
        {
            std::ifstream file(argv[1]);
            if (!file.good())
            {
                printf("script doesn't exist!");
                return 0;
            }

            auto lastPath = std::filesystem::current_path();
            auto relativePath = std::filesystem::path(argv[1]);
            auto abspath = lastPath / relativePath;

            LuauVM vm;
            LuauVM::directory = abspath.parent_path();
            vm.DoFile(argv[1]);
        }
        catch (std::exception& e)
        {
            std::cout << e.what() << "\n";
            system("pause");
        }

        return 0;
    }

    help();
}
