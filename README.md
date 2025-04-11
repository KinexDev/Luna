# Luna
**Note**: LuauRuntime is being rebranded to luna

Luna is a standalone runtime for luau.
download is [here](https://github.com/KinexDev/luna/releases/tag/Release).

# Features
- Dynamic library linking
- Support for multiple scripts (using require function)
- Extras for standard libraries, System commands (using system function) + File handelling
- Globals for handling different OS's (`platform` returns `windows`, `linux` or `apple`)
- Bytecode Compilation
- Building into self-contained executables (compiles to bytecode and then gets bundled with the interpreter into an exe)

# Coming soon
- Cross platform dynamic library support (untested on linux and apple devices since i don't have the time but it should be implemented.)
- Making it use luau static libs for VM and Compiler (so self-contained executables do not recompile luau fully.) which should result in faster compilation.

# Issues
- in self contained executables, scripts must not have the same name (it currently only looks for the name of the script currently) so make sure none of your scripts have the same name.

# Dynamic Library Linking
To make a dynamic library for luna, refer to this [example](https://github.com/KinexDev/luna-LibExample), it has an example function called CustomPrint, this is compiled to `example.dll`.

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

# Example Project
this is a [snake game](https://github.com/KinexDev/Luau-Snake) completely written in luau using luna using raylib bindings.

# Running Scripts
to run scripts, you execute luau and pass in the script you want to run

```
luna main.luau
```
