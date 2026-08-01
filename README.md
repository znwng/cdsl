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

Make sure CMake is installed.

### Clone

```sh
git clone https://github.com/znwng/cdsl.git
cd cdsl
```

### Generate Binary

```sh
chmod +x bootstrap.sh
chmod +x clean.sh

./bootstrap.sh
```

### Uninstall

```sh
./clean.sh
```

