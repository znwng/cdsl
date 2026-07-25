# CDSL (Custom Description Scripting Language)

CDSL is a lightweight domain-specific language (DSL) designed to describe robotic movement and control instructions in a simple, human-readable format.

Scripts consist of one instruction per line and are executed sequentially from top to bottom.

---

# File Extension

CDSL scripts use the `.cdsl` file extension.

Example:

```text
robot.cdsl
```

---

# Syntax

Each instruction follows the general format:

```text
INSTRUCTION ARGUMENT_1 ARGUMENT_2 ...
```

Arguments are separated by whitespace.

---

# Instructions

## SET

Defines a named constant.

### Syntax

```text
SET <NAME> <VALUE>
```

### Example

```text
SET SPEED 120
SET HEIGHT 45.5
```

Constants can later be referenced using the `$` prefix.

---

## MOVE

Moves a robot component by the specified value.

### Syntax

```text
MOVE <COMPONENT> <VALUE>
```

The value may be either:

- A numeric literal
- A previously defined constant

### Examples

```text
MOVE JOINT_1 45
MOVE ARM -30
MOVE BASE 12.5
```

Using constants:

```text
SET SPEED 90

MOVE JOINT_1 $SPEED
```

---

## WAIT

Pauses execution.

### Syntax

```text
WAIT <MILLISECONDS>
```

### Example

```text
WAIT 500
```

The delay must be a non-negative integer.

---

Values must be valid numeric literals.
Invalid examples:

```text
12abc
10deg
3.1.4
abc
```

---

# Comments

Everything after `//` on a line is treated as a comment.

Full-line comment:

```text
// Move the robot
```

Inline comment:

```text
MOVE JOINT_1 45 // Rotate first joint
```

---

# Execution

Instructions are executed sequentially in the order they appear.

Example:

```text
SET SPEED 100

MOVE BASE 20
WAIT 500
MOVE ARM $SPEED
```

---

# Error Handling

Execution stops immediately when an error is encountered.

Sample script is provided with intentional errors.

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

