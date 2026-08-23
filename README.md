# CDSL — Custom Description Scripting Language

CDSL (Custom Description Scripting Language) is a lightweight domain-specific language (DSL) for describing robotic movement and control instructions in a simple, human-readable format.

CDSL scripts use the `.cdsl` file extension. Instructions are executed sequentially, from top to bottom.

## Build

### Requirements

Make sure the following tools are installed:

* C++ compiler
* CMake
* Ninja
* Git
* Make
* ClangFormat *(optional, for formatting)*

### Build Commands

| Command           | Description                                                                      |
| ----------------- | -------------------------------------------------------------------------------- |
| `make`            | Build the project. This is the default target.                                   |
| `make setup`      | Check dependencies, initialize Git submodules, and configure the CMake build.    |
| `make build`      | Build the project using CMake and Ninja. Automatically runs `setup` if required. |
| `make clean`      | Remove build files and `compile_commands.json`.                                  |
| `make rebuild`    | Clean the project, reconfigure it, and build from scratch.                       |
| `make check-deps` | Check whether the required build dependencies are installed.                     |
| `make format`     | Format C/C++ source files using ClangFormat.                                     |

The default build type is `Debug`. You can specify a different CMake build type:

```sh
make BUILD_TYPE=Release
```

### Building From Scratch

For a fresh clone, run:

```sh
make setup
make build
```

Or simply:

```sh
make
```

## Usage

The CDSL interpreter supports two modes:

* **Script mode** — Execute instructions from a `.cdsl` file.
* **Interactive mode** — Enter instructions directly in the terminal.

### Script Mode

Create a `.cdsl` file containing the instructions in the order they should be executed.

For example:

```cdsl
move FORWARD 10
wait 2
move COMOPNENT_NAME 5
```

Then execute the script with:

```sh
cdsl filename.cdsl
```

The interpreter processes instructions sequentially and stops immediately when it encounters:

* An invalid command
* An invalid value
* An invalid instruction

### Validation

To validate a script without executing it, use the `--check` flag:

```sh
cdsl filename.cdsl --check
```

This is useful for detecting errors before running a script.

### Interactive Mode

Run `cdsl` without providing a file:

```sh
cdsl
```

The interpreter will enter interactive mode and accept instructions directly from the terminal.

The `--check` flag is not available in interactive mode.

## Instruction Syntax

CDSL instructions generally follow one of these forms:

```text
ACTION LABEL VALUE
```

or:

```text
ACTION VALUE
```

The exact syntax and available instructions are documented in the language reference.

For detailed information about CDSL syntax, supported instructions, values, and semantics, see the [language documentation](src/README.md).

## Project Structure

```text
.
├── CMakeLists.txt
├── Makefile
├── README.md
├── include/
├── src/
├── tests/
└── build/
```

The `build/` directory is generated during compilation and does not need to be committed to version control.

## Development

### Formatting

To format the C and C++ source files:

```sh
make format
```

This uses `clang-format` on files under:

```text
src/
include/
```

### Cleaning

To remove generated build files:

```sh
make clean
```

To perform a completely fresh build:

```sh
make rebuild
```

## Documentation

For the complete CDSL language reference, see:

[Language Documentation](src/README.md)
