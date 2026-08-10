# CDSL Instruction Set

CDSL programs are composed of instructions, with each instruction written on a separate line.

The current instruction set consists of:

* `SET` — Create or update a variable
* `PRINT` — Print a variable or value
* `MOVE` — Move a component by a specified value
* `WAIT` — Pause execution for a specified duration

Comments can be written using `//`. Empty lines and comment lines are ignored.

---

## `SET`

Creates a variable or changes the value of an existing variable.

### Syntax

```text
SET <variable> <value>
```

### Examples

```text
SET A 121
SET speed 100
SET angle 45.5
```

Variable names must be valid identifiers. Values can be numeric literals, variables, or expressions.

A variable is referenced using `$`:

```text
SET A 121
SET B $A
```

---

## `PRINT`

Prints a value or the value of a variable.

### Syntax

```text
PRINT <value>
```

### Examples

Print a variable:

```text
SET A 121
PRINT $A
```

Print a literal:

```text
PRINT 100
PRINT 45.5
```

A variable that has not been defined cannot be printed:

```text
PRINT $M
```

---

## `MOVE`

Moves a specified component by a given value.

### Syntax

```text
MOVE <component> <value>
```

The component name is user-defined and identifies the hardware component being controlled.

### Examples

Move a component by a literal value:

```text
MOVE JOINT_2 120
```

Use a variable:

```text
SET A 121
MOVE JOINT_1 $A
```

The value must be a valid numeric value, variable, or expression.

Invalid examples:

```text
MOVE JOINT abc
MOVE JOINT 12abc
MOVE JOINT 12.0.1
```

An undefined variable also results in an error:

```text
MOVE BASE $B
```

Once the variable exists, it can be used:

```text
SET B 1330
MOVE BASE $B
```

---

## `WAIT`

Pauses execution for a specified number of milliseconds.

### Syntax

```text
WAIT <duration_ms>
```

### Examples

```text
WAIT 500
WAIT 1000
```

The duration can also be stored in a variable:

```text
SET delay 500
WAIT $delay
```

The delay cannot be negative:

```text
WAIT -500
```

Invalid values are rejected:

```text
WAIT abc
```

---

## Variables

Variables are created using `SET` and referenced using `$`.

```text
SET speed 100
SET angle 45.5

PRINT $speed
MOVE JOINT_1 $angle
WAIT $speed
```

Using an undefined variable results in an error:

```text
MOVE BASE $UNKNOWN
```

---

## Expressions

Expressions can be used where a numeric value is accepted.

Expressions are enclosed in `#[...]`. Spaces shouldn't be included

```text
#[10 + 20] // Invalid
#[10+20] // Valid
```

For example:

```text
SET A 100
SET B #[50+25]

PRINT #[$A+$B]
MOVE JOINT_1 #[45*2]
WAIT #[250+250]
```

Expressions support arithmetic operators such as:

```text
+   Addition
-   Subtraction
*   Multiplication
/   Division
```

Parentheses can be used to control evaluation order:

```text
PRINT #[(10+20)*2]
```

Variables can be referenced inside expressions using `$`:

```text
SET A 100
SET B 50

PRINT #[$A+$B]
```

---

## Comments

Use `//` to add comments.

```text
SET A 121 // Set A to 121
PRINT $A // Print A
```

A line beginning with `//` is ignored:

```text
// SET A 500
```

Comments can also be used to temporarily disable an instruction.

---

## Complete Example

```text
// Configure initial values
SET A 121
SET speed 100
SET delay 500

// Display values
PRINT $A
PRINT $speed

// Move components
MOVE JOINT_1 $speed
MOVE JOINT_2 120

// Wait between operations
WAIT $delay

// Use an expression
SET speed #[50*2]
MOVE JOINT_1 $speed

WAIT #[250+250]

PRINT $speed
```

This demonstrates the basic CDSL workflow:

```text
SET    → define values
PRINT  → inspect values
MOVE   → control components
WAIT   → introduce delays
#[...] → calculate values
$...   → reference variables
//     → add comments
```

---

## Validation

CDSL reports errors when instructions are malformed or contain invalid values.

For example:

```text
PRINT
SET X
SET @C 12
MOVE JOINT abc
WAIT -500
MOVE BASE $UNKNOWN
ALIGN JX 21
```

These correspond to invalid arguments, invalid variable names, invalid values, negative delays, undefined variables, and unsupported instructions.

You can validate a CDSL file without executing it using:

```bash
cdsl <file>.cdsl --check
```

This is useful for checking a script for instruction errors before running it.
