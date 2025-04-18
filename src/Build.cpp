#include "../include/Build.h"
#include "../include/Runtime.h"

std::string Build::Compile(std::string& source)
{
    size_t bytecodeSize = 0;
    const char* sourceCstr = source.c_str();
    char* bytecode = luau_compile(sourceCstr, strlen(sourceCstr), nullptr, &bytecodeSize);

    auto bytecodeStr = std::string(bytecode, bytecodeSize);
    free(bytecode);
    return bytecodeStr;
}

void Build::ReplaceAll(std::string& str, const std::string& from, const std::string& to) {
    size_t start_pos = 0;
    while ((start_pos = str.find(from, start_pos)) != std::string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length();
    }
}

std::string Build::EncodeToHex(const std::string& str)
{
    std::ostringstream oss;
    for (size_t i = 0; i < str.length(); ++i)
    {
        oss << "0x";

        oss << std::setw(2) << std::setfill('0') << std::hex
            << (int)(unsigned char)str[i];

        if (i != str.length() - 1)
        {
            oss << ", ";
        }
    }

    return oss.str();
}

void Build::CreateProject(std::string& name)
{
    std::filesystem::path workingDir = std::filesystem::current_path();
    std::filesystem::path nameDir = workingDir / name;
    std::filesystem::path srcDir = nameDir / "src";
    std::filesystem::path mainDir = srcDir / "main.luau";
    std::filesystem::path buildDir = nameDir / "build.lua";
    std::filesystem::path gitignoreDir = nameDir / ".gitignore";

    if (!std::filesystem::exists(nameDir))
    {
        std::filesystem::create_directory(nameDir);
    }

    if (!std::filesystem::exists(srcDir))
    {
        std::filesystem::create_directory(srcDir);
    }

    std::ofstream main(mainDir);
    main << "print(\"Hello world!\")";
    main.close();

    std::ofstream gitignore(gitignoreDir);
    gitignore << "out/";
    gitignore.close();


    std::ofstream build(buildDir);
    build << "return {\n";
    build << "   name = \"" + name + "\", -- the name of the executable/project \n";
    build << "   main = \"src/main.luau\", -- the starting point of the script \n";
    build << "   scripts = {}, -- additional scripts to compile \n";
    build << "   dependencies = {}, -- files to copy over after the build is finished \n";
    build << "   noConsole = false -- if you want to build without console (windows only) \n";
    build << "}";
    build.close();

    std::cout << "Generated project " << name << "!";
}

int Build::Build(std::string& exePathStr, std::string& buildPathStr)
{
    std::filesystem::path filePath(buildPathStr);

    std::ifstream file(filePath);

    if (!file.good())
    {
        printf("script doesn't exist!");
        return 0;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string source = buffer.str();

    Runtime vm;
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

    std::string mainByteCode = Compile(mainSource);

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
                std::string scriptByteCode = Compile(scriptSource);
                bytecode[script] = scriptByteCode;
            }

            lua_pop(vm.L, 1);
        }
    }

    bool noConsole = false;

    lua_pop(vm.L, -2);

    lua_pushstring(vm.L, "noConsole");

    lua_gettable(vm.L, -2);

    if (lua_isboolean(vm.L, -1))
    {
        noConsole = lua_toboolean(vm.L, -1);
    }

    std::filesystem::path exePath(exePathStr);
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

    if (std::filesystem::exists(outPath))
    {
        std::filesystem::remove_all(outPath);
    }

    std::filesystem::create_directory(outPath);
    std::filesystem::copy(buildDir.string(), tempPath, std::filesystem::copy_options::recursive | std::filesystem::copy_options::overwrite_existing);

    auto previousWorkingDirectory = std::filesystem::current_path();

    std::filesystem::current_path(tempPath);

    std::filesystem::path codePath = tempPath / "luaucode.h";

    std::ofstream codeFile(codePath);
    std::string code =
        "#pragma once\n"
        "#include \"iostream\" \n"
        "namespace LuauCode { \n"
        "    std::vector<unsigned char> main = {" + EncodeToHex(mainByteCode) + "}; \n"
        "    std::unordered_map<std::string, std::vector<unsigned char>> requiredScripts = { \n";

    if (!bytecode.empty()) {
        for (auto& x : bytecode)
        {
            code += "{ \"" + x.first + "\", {" + EncodeToHex(x.second) + "} }, ";
        }

        code[code.size() - 2] = '}';
        code[code.size() - 1] = ';';
    }
    else {
        code += "};\n";
    }
    code += "\n}";
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

    std::stringstream cmakeListsbuffer;
    cmakeListsbuffer << cmakeListsFile.rdbuf();
    cmakeListsFile.close();

    std::string cmakeListsTxt = cmakeListsbuffer.str();
    std::string previousCmakeLists = previousCmakeLists;
    ReplaceAll(cmakeListsTxt, "\"luau\"", name);

    std::ofstream cmakeListsFileW(cmakeListsPath);
    cmakeListsFileW << cmakeListsTxt;
    cmakeListsFileW.close();

    printf("made changes to cmakelists");

    printf("done! compiling project! \n");

    if (noConsole)
    {
        system("cmake -S . -B build -DLUAU_BUILD_WIN=ON");
    }
    else
    {
        system("cmake -S . -B build -DLUAU_BUILD_WIN=OFF");
    }

    system("cmake --build build --config Release");


    std::filesystem::path executablePath = tempPath / "build" / "Release" / exeName;
    std::ifstream executableFile(cmakeListsPath);

    // failed compiling or something.
    if (!executableFile.good())
    {
        printf("failed compiling!");
        std::filesystem::remove(tempPath);
        return 1;
    }

    //move exe to accessible area.
    std::filesystem::rename(executablePath, executableDestPath);

    executableFile.close();

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

    std::filesystem::remove_all(tempPath);

    printf("deleted temp! \n");
    printf("finished compilation! \n");

    lua_pop(vm.L, 1);
    return 0;
}
