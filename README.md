# Luna
Luna is a standalone runtime for luau.
download is [here](https://github.com/KinexDev/luna/releases/tag/Release2).

# Features
- Dynamic library linking
- Support for loading modules (using require function)
- Extras for standard libraries, System commands (using system function) + File handelling
- Globals for handling different OS's (`platform` returns `windows`, `linux` or `apple`)
- Bytecode Compilation
- Building into self-contained executables (compiles to bytecode and then gets bundled with the interpreter into an exe)

# Prerequisites
- you need cmake for self-contained executables (it compiles using cmake)

# Dynamic Library Linking
This is an extension of the standard library, To make a dynamic library for luna, refer to this [example](https://github.com/KinexDev/luna-LibExample), it has an example function called CustomPrint, this is compiled to `example.dll`.

in luau we load the library, load the extern function and cast it to it's function type

`example.luau`
```luau
local example_lib: lib = lib.load("example.dll")

return {
    custom_print = example_lib:extern("CustomPrint") :: (message: string) -> ()
}
```

we can then call this in other scripts.
`main.luau`
```luau
local example = require("example")
example.custom_print("hello world!")
```

a great example to see this actually being used for something is the [raylib bindings for luna](https://github.com/KinexDev/Luna-Raylib/tree/main)

# Standard Library Extras
the file module comes with 14 methods,
```luau
    readAllText: (path: string) -> string?,
    writeAllText: (path: string, content: string) -> boolean,
    exists: (path: string) -> boolean,
    delete: (path: string) -> boolean,
    createDirectory: (dir_path: string) -> (),
    deleteDirectory: (dir_path: string) -> (),
    existsDirectory: (path: string) -> boolean,
    getFileExtension: (path: string) -> string?,
    copyFile: (src_path: string, dest_path: string) -> boolean,
    moveFile: (src_path: string, dest_path: string) -> boolean,
    listFilesInDirectory: (dir_path: string) -> { [number]:string }?,
    listDirectoriesInDirectory: (dir_path: string) -> { [number]:string }?,
    copyDirectory: (src_dir_path: string, dest_dir_path: string) -> boolean,
    moveDirectory: (src_dir_path: string, dest_dir_path: string) -> boolean
```

the system function calls a operating system command

```luau
system("tree") -- creates an ascii tree of files
```

Luna is still young, so it doesn't have a developed standard library yet.

# Example Project
this is a [snake game](https://github.com/KinexDev/Luau-Snake) completely written in luau using luna using raylib bindings, that is also a great example to show you how to create a project with luna.

# Running Scripts
to run scripts, you run luna and pass in the script you want to run

```
luna main.luau
```

# Building

to build a project, you run luna, add the build arg and pass in the script you want to run

```
luna --build build.lua
```

this would build the project based on that build script.
build scripts are in this structure

```lua
return {
   name = "luau_empty", -- the name of the executable/project 
   main = "src/main.luau", -- the starting point of the script 
   scripts = {}, -- additional scripts to compile 
   dependencies = {}, -- files to copy over after the build is finished 
   buildWin = false -- if you want to build without console (also makes it windows only)
}
```

# Project

to create a project, you run luna, add the project arg and pass the name of the project

```
luna --project example
```

this creates a folder called example with the project set up with build and main.

Im open to any contributions to this project.
