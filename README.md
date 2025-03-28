# LuauRuntime
LuauRuntime is a standalone runtime for luau.

# Features
- Dynamic library linking
- Support for multiple scripts (using require function)
- System commands (using system function)
- File handelling (using file table)
- Globals for handling different OS's (`platform` returns `windows`, `linux` or `apple`)
- Cross platform (tested on windows and ubuntu)

more coming soon!

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

# File handling
the file class comes with 7 methods,
```luau
    read_all_text: (path: string) -> string|nil,
    write_all_text: (path: string, content: string) -> boolean,
    exists: (path: string) -> boolean,
    delete: (path: string) -> boolean,
    create_directory: (dir_path: string) -> (),
    delete_directory: (dir_path: string) -> (),
    exists_directory: (path: string) -> boolean,
    get_file_extension: (path: string) -> string,
    copy_file: (src_path: string, dest_path: string) -> boolean,
    move_file: (src_path: string, dest_path: string) -> boolean,
    list_files_in_directory: (dir_path: string) -> { [number]:string },
    copy_directory: (src_dir_path: string, dest_dir_path: string) -> boolean,
    move_directory: (src_dir_path: string, dest_dir_path: string) -> boolean
```

# Running Scripts
to run scripts, you execute the exe and pass in the script you want to run, heres a simple batch script

```
luau.exe --run main.luau
```
