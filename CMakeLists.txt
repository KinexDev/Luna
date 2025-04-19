cmake_minimum_required (VERSION 3.8)


if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("luna")

file(GLOB_RECURSE SOURCES_FILES "src/*.cpp")
file(GLOB_RECURSE HEADER_FILES "include/*.h")

if(WIN32)
    set(RESOURCE_FILE "${CMAKE_SOURCE_DIR}/luau.rc")
    set(OBJECT_FILE "${CMAKE_SOURCE_DIR}/luau.o")

    if(EXISTS ${RESOURCE_FILE})
        add_custom_command(
            OUTPUT ${OBJECT_FILE}
            COMMAND windres ${RESOURCE_FILE} -o ${OBJECT_FILE}
            DEPENDS ${RESOURCE_FILE}
            COMMENT "Compiling resource file into ${OBJECT_FILE}"
        )
    else()
        message(FATAL_ERROR "Resource file ${RESOURCE_FILE} not found.")
    endif()
else()
    set(OBJECT_FILE "")
endif()

set(CMAKE_CXX_STANDARD 17)

file(COPY "${CMAKE_SOURCE_DIR}/builds" DESTINATION "${CMAKE_BINARY_DIR}/")

add_library("luna_static" STATIC
    ${SOURCES_FILES}
    ${HEADER_FILES}
)

target_link_libraries("luna_static" PRIVATE
    "${PROJECT_SOURCE_DIR}/libs/Luau.Compiler.lib"
    "${PROJECT_SOURCE_DIR}/libs/Luau.VM.lib"
    "${PROJECT_SOURCE_DIR}/libs/Luau.Ast.lib"
)

target_include_directories("luna_static" PRIVATE
    "${PROJECT_SOURCE_DIR}/include/luau"
)

add_executable("luna" 
    "main.cpp"
    ${OBJECT_FILE}
)

target_link_libraries("luna" PRIVATE
    luna_static
)

target_include_directories("luna" PRIVATE
    "${PROJECT_SOURCE_DIR}/include/luau"
)