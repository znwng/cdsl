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

---

# Versioning

ex: v0.0.1

```
1.4.2
│ │ │
│ │ └── PATCH
│ └──── MINOR
└────── MAJOR
```

| Version Part                | Increment When                                                                                  |
| --------------------------- | ----------------------------------------------------------------------------------------------- |
| **PATCH** (`1.0.0 → 1.0.1`) | Bug fixes, refactoring, or performance improvements that do not change the language's behavior. |
| **MINOR** (`1.0.0 → 1.1.0`) | New features are added in a backward-compatible way. Existing scripts continue to work.         |
| **MAJOR** (`1.0.0 → 2.0.0`) | Breaking changes are introduced that make older scripts incompatible.                           |

