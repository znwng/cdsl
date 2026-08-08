# CDSL (Custom Description Scripting Language)

CDSL is a lightweight domain-specific language (DSL) designed to describe robotic movement and control instructions in a simple, human-readable format.

Scripts consist of one instruction per line and are executed sequentially from top to bottom. CDSL scripts use the `.cdsl` file extension.

---

# Syntax

Each instruction follows the general format:

```text
INSTRUCTION ARGUMENT_1 ARGUMENT_2 ...
```

---

# Build

Make sure **Make**, **CMake**, and a **C++ compiler** are installed.

## Clone

```sh
git clone https://github.com/znwng/cdsl.git
cd cdsl
```

## Build
```sh
make
```

Binary built in `build` directory with name cdsl.

```sh
make clean
```
To delete the build directory.

---

# Usage

```sh
# Open the help menu
cdsl --help 

# Start executing the instructions provided in the file provided
cdsl filename.cdsl 

# Validates the instructions without executing them
cdsl filename.cdsl --check

# Starts an interactive shell to execute commands
cdsl
```
