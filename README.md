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

| Command           | Description                                                                      |
| ----------------- | -------------------------------------------------------------------------------- |
| `make`            | Build the project. This is the default target.                                   |
| `make setup`      | Initialize Git submodules and configure the CMake build.                         |
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

## Instructions

CDSL currently supports the following instructions:

| Instruction | Syntax                 | Description                                       |
| ----------- | ---------------------- | ------------------------------------------------- |
| `set`       | `set VARIABLE VALUE`   | Create or update a variable.                      |
| `print`     | `print VALUE`          | Print a variable or expression value.             |
| `move`      | `move COMPONENT VALUE` | Move a configured component to a specified value. |
| `wait`      | `wait DURATION_MS`     | Wait for the specified number of milliseconds.    |

Instruction names are case-sensitive and should be written in lowercase when entering them in the interpreter.

### set

Set a variable to a numeric value:

```text
set speed 50
```

Variables can be referenced using `$`:

```text
set speed 50
print $speed
```

Variables may also be assigned from another variable:

```text
set speed 50
set target $speed
```

### print

Print a variable:

```text
set speed 50
print $speed
```

Expressions can also be evaluated:

```text
print #[10+20*2]
```

> Note: No space to be given in the expression. Else, it'll be read as two separate arguments and will invlidated

### move

Move a configured component to a specified value:

```text
move BASE 90
```

A variable can be used as the value:

```text
set angle 90
move BASE $angle
```

Expressions can also be used:

```text
set offset 10
move BASE #[30+$offset]
```

Component names and their limits are defined in the CDSL configuration file in `~/.config/cdls`.

The current hardware implementation is a placeholder and does not yet communicate with a physical robot.

### wait

Wait for a specified duration in milliseconds:

```text
wait 1000
```

The duration must be a non-negative integer.

Variables and expressions can also be used:

```text
set delay 500
wait $delay
```

```text
wait #[250+250]
```

## Variables

Variable names must:

* Start with a letter or `_`
* Contain only letters, digits, and `_`

Examples:

```text
set speed 50
set _offset 10
set motor_angle 90
```

Invalid examples:

```text
set 123value 10
set motor-angle 90
set motor.angle 90
```

Variables store floating-point values.

## Expressions

Expressions are written using `#[...]`:

```text
#[10+20]
```

CDSL supports:

* Addition: `+`
* Subtraction: `-`
* Multiplication: `*`
* Division: `/`
* Unary `+` and `-`
* Parentheses
* Variables

Examples:

```text
#[10+20]
#[10*5]
#[(10+20)*2]
#[100-$offset]
#[-$angle]
```

Variables are referenced using `$`:

```text
#[10+$offset]
```

**Expressions must not contain whitespace.**

For example:

```text
#[10+20*2]
```

is valid, while:

```text
#[10 + 20 * 2]
```

is not.

Division by zero and references to undefined variables result in an interpreter error.

## Comments

Comments begin with `//`.

```text
// This is a comment
set speed 50
move BASE $speed // Move the base
```

## Components and Configuration

CDSL components are configured through:

```text
~/.config/cdsl/config.toml
```

A component has its allowed value range.

For example:

```toml
[component.BASE]
min = 0
max = 180
```

A `move` instruction validates the requested value against the configured component limits before executing it.

The configuration directory and file are created automatically if they do not already exist.

## Documentation

For the detailed CDSL language reference, see:

[Language Documentation](src/README.md)

