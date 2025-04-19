#include "include/Runtime.h"
#include "include/Require.h"
#include "include/Build.h"
#include <string>

#if defined(_WIN32)
#include <windows.h>
#elif defined(__APPLE__)
#include <mach-o/dyld.h>
#include <limits.h>
#elif defined(__linux__)
#include <unistd.h>
#include <limits.h>
#endif

// lowkey used chatgpt to generate this quickly
std::string GetExecutablePath() {
    char buffer[260];

#if defined(_WIN32)
    DWORD length = GetModuleFileNameA(NULL, buffer, 260);
    if (length == 0 || length == 260)
        return "";
#elif defined(__APPLE__)
    uint32_t size = sizeof(buffer);
    if (_NSGetExecutablePath(buffer, &size) != 0)
        return "";
#elif defined(__linux__)
    ssize_t length = readlink("/proc/self/exe", buffer, 260);
    if (length == -1 || length == 260)
        return "";
    buffer[length] = '\0';
#endif

    return std::string(buffer);
}

void help() 
{
    printf("Usage: luna [args]\n");
    printf("Options:\n");
    printf("    run <script>      Runs the specified Luau script.\n");
    printf("    build <script>    Builds a self-contained executable based on the config.\n");
    printf("    project <name>    Creates a new luna project.\n");
    printf("    version           Show version information.\n");
    printf("    help              Shows this message.\n");
}

void version()
{
    printf("luau: 0.6.9\n");
    printf("Luna: 0.0.2\n");
}


int main(int argc, char* argv[]) 
{
    if (argc < 2) 
    {
        help();
        return 0;
    }
    
    if (strcmp(argv[1], "run") == 0)
    {
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

            Runtime vm;
            Require::directory = abspath.parent_path();
            vm.DoFile(argv[2]);
        }
        catch (std::exception& e)
        {
            std::cout << e.what() << "\n";
        }

        return 0;
    }
    if (strcmp(argv[1], "version") == 0)
    {
        version();
        return 0;
    }
    else if (strcmp(argv[1], "project") == 0)
    {
        std::string name = std::string(argv[2]);
        Build::CreateProject(name);
        return 0;
    }
    else if (strcmp(argv[1], "help") == 0)
    {
        help();
        return 0;
    }
    else if (strcmp(argv[1], "build") == 0)
    {
        std::string exePath = getExecutablePath();
        std::string buildPath = std::string(argv[2]);
        return Build::Build(exePath, buildPath);
    }

    help();
}
