# CDSL (Custom Description Scripting Language)

CDSL is a lightweight domain-specific language (DSL) designed to describe robotic movement and control instructions in a simple, human-readable format.

Scripts consist of one instruction per line and are executed sequentially from top to bottom. CDSL scripts use the `.cdsl` file extension.

# Build
Ensure installed:
- Make
- CMake
- Ninja build system
- GCC/G++ Compiler
- GTests (optional)

| Command           | Description                                                                  |
| ----------------- | ---------------------------------------------------------------------------- |
| `make`            | Build the project (default).                                                 |
| `make setup`      | Check dependencies, initialize Git submodules, and configure CMake.          |
| `make build`      | Build the project using CMake and Ninja. Automatically runs setup if needed. |
| `make clean`      | Remove build files and `compile_commands.json`.                              |
| `make rebuild`    | Clean the project, reconfigure it, and build from scratch.                   |
| `make check-deps` | Check whether CMake, Ninja, Git, and a C++ compiler are installed.           |

# Docs
The interpreter expects instructions either in a file or directly (interactive mode).
To pass the instructions in a file, put all the commands (in expected order of execution) and save it
as a `.cdsl` file.
And then run:
```sh
cdsl filename.cdsl
```

The interpreter executes the commands line-by-line and immediately stops execution as soon as it
detects an invalid command, invalid value or an overall invalid instruction. To just validate the 
instructions file without actually executing the instructions, use the `--check` flag.
```sh
cdsl filename.cdsl --check
```
This is particularly helpful to pin-point errors before execution.

To start the interactive mode just run `cdsl`. There is no `--check` flag for interactive mode.

Instructions are of the form: `ACTION LABEL VALUE`

or sometimes just: `ACTION VALUE`

For more information, check out [this](runtime/README.md)
