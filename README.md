# CDSL — Custom Description Scripting Language

CDSL (Custom Description Scripting Language) is a lightweight domain-specific language (DSL) for describing robotic movement and control instructions in a simple, human-readable format.

## Build

### Requirements

Make sure the following tools are installed:

* C++ compiler with C++23 support
* CMake
* Ninja
* Git
* Make
* ClangFormat *(optional, for formatting)*

### Build Commands

| Command          | Description                                                  |
| ---------------- | ------------------------------------------------------------ |
| `make`           | Build the project. This is the default target.               |
| `make clean`     | Remove build files and `compile_commands.json`.              |
| `make rebuild`   | Clean the project, reconfigure it, and build from scratch.   |
| `make check`     | Check whether the required build dependencies are installed. |
| `make install`   | Add the final binary to `~/.local/bin`.                      |
| `make uninstall` | Remove the binary from `~/.local/bin`.                       |

### Build Type

The default build type is `Debug`.

To build using `Release` instead:

```sh
make BUILD_TYPE=Release
```

## Usage

The CDSL interpreter currently provides an **interactive mode** for entering and testing instructions directly from the terminal.

### Interactive Mode

Run `cdsl`:

```sh
cdsl
```

The interpreter displays a prompt:

```text
cdsl>
```

Instructions can then be entered directly:

```text
set speed 50
print $speed
move BASE $speed
wait 1000
```

The interactive mode can be exited using:

```text
exit
```

`Ctrl+D` also exits the interpreter. `Ctrl+C` can be used to interrupt the current input.

The `clear` command clears the terminal screen:

```text
clear
```

Command history is automatically stored in:

```text
.cdsl_history
```

## Documentation

For the detailed CDSL language reference, see:

[Language Documentation](src/README.md)
