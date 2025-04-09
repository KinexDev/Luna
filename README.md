# LuauRuntime
LuauRuntime is a prototype standalone runtime for luau.

# Features
- Dynamic library linking
- Support for multiple scripts (using require function)
- Extras for standard libraries, System commands (using system function) + File handelling
- Globals for handling different OS's (`platform` returns `windows`, `linux` or `apple`)
- Bytecode Compilation

# Coming soon
- Building into self-contained executables (in the works, currently unfinished)
- Cross platform (untested on linux and apple devices since i don't have the time)

# Dynamic Library Linking
To make a dynamic library for luauRuntime, refer to this [example](https://github.com/KinexDev/LuauRuntime-LibExample), it has an example function called CustomPrint, this is compiled to `example.dll`.

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

# Standard Library Extras
the file class comes with 7 methods,
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

# Example Project
this is a [snake game](https://github.com/KinexDev/Luau-Snake) completely written in luau using luauRuntime using raylib bindings.

# Running Scripts
to run scripts, you execute luau and pass in the script you want to run

```
luau main.luau
```
