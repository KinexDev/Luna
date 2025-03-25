cmake_minimum_required (VERSION 3.8)

if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("LuauRuntime")
add_subdirectory(thirdparty/luau)

file(GLOB_RECURSE SOURCES_FILES "src/*.cpp" )
file(GLOB_RECURSE HEADER_FILES "include/*.h" )
add_executable (LuauRuntime "src/Lib.cpp" "src/LuauVM.cpp" "src/main.cpp" "src/Userdata.cpp" "include/Lib.h" "include/LuauVM.h" "include/main.h" "include/Userdata.h"  "include/File.h" "src/File.cpp")

target_link_libraries(LuauRuntime PRIVATE Luau.VM Luau.Compiler)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET LuauRuntime PROPERTY CXX_STANDARD 20)
endif()