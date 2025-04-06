cmake_minimum_required(VERSION 3.10)
project(LUAU_BUILD)

add_executable(LUAU_BUILD "main.cpp" "luaucode.h" "include/luacode.h" "include/Luau/BytecodeBuilder.h" "include/Luau/Compiler.h" "include/lua.h" "include/luaconf.h" "include/lualib.h")

target_link_libraries(LUAU_BUILD PRIVATE "${CMAKE_SOURCE_DIR}/luau_static.lib")

target_include_directories(LUAU_BUILD PRIVATE "${CMAKE_SOURCE_DIR}/include")

set(CMAKE_CXX_STANDARD 17)