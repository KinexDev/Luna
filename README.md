# LuauRuntime
LuauRuntime is a standalone runtime for luau.

# Features
- Dynamic library linking
- Support for multiple scripts (using require function)
- System commands (using system function)
- File handelling (using file table)

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
exists_directory: (path: string) -> boolean
```

# Running Scripts
to run scripts, you execute the exe and pass in the script you want to run, heres a simple batch script

```
LuauRuntime.exe main.luau
```
# Platforms
It currently only supports windows, i haven't written cross-platform code yet for the dynamic library handling, but i will work on that, i've got this up quick since im prototyping but overall i feel happy with this project, i want to work with it more definitely.
