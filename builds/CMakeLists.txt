cmake_minimum_required (VERSION 3.8)

option(LUAU_BUILD_WIN "Build windows build" OFF)

if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("luau")
add_subdirectory(thirdparty/luau)

file(GLOB_RECURSE SOURCES_FILES "src/*.cpp")
file(GLOB_RECURSE HEADER_FILES "include/*.h")

set(CMAKE_CXX_STANDARD 17)

add_executable("luau" 
    "main.cpp"
    ${SOURCES_FILES}
    ${HEADER_FILES}
)

target_link_libraries("luau" PRIVATE Luau.VM Luau.Compiler)

if(LUAU_BUILD_WIN)
    target_compile_definitions("luau" PRIVATE LUAU_BUILD_WIN)
    set_target_properties("luau" PROPERTIES WIN32_EXECUTABLE TRUE)
endif()