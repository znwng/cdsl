# CDSL Instruction Set

CDSL programs are composed of instructions, with each instruction written on a separate line.

The current instruction set consists of:

* `set` — Create or update a variable
* `print` — Print a value or the value of a variable
* `move` — Move a component to a specified value
* `wait` — Pause execution for a specified duration

Comments can be written using `//`. Empty lines and comment lines are ignored.

---

## `set`

Creates a variable or changes the value of an existing variable.

### Syntax

```text
set <variable> <value>
```

### Examples

```text
set A 121
set speed 100
set angle 45.5
```

Variable names must be valid identifiers. Values can be numeric literals, variables, or expressions.

A variable is referenced using `$`:

```text
set A 121
set B $A
```

---

## `print`

Prints a value or the value of a variable.

### Syntax

```text
print <value>
```

### Examples

Print a variable:

```text
set A 121
print $A
```

Print a literal:

```text
print 100
print 45.5
```

An undefined variable cannot be printed:

```text
print $M
```

---

## `move`

Moves a specified component to a given value.

### Syntax

```text
move <component> <value>
```

The component name is user-defined and identifies the hardware component being controlled.

### Examples

Move a component to a literal value:

```text
move JOINT_2 120
```

Use a variable:

```text
set A 121
move JOINT_1 $A
```

The value must be a valid numeric value, variable, or expression.

Invalid values:

```text
move JOINT abc
move JOINT 12abc
move JOINT 12.0.1
```

An undefined variable also results in an error:

```text
move BASE $B
```

Once the variable exists, it can be used:

```text
set B 133
move BASE $B
```

The value must also fall within the configured limits of the specified component.

---

## `wait`

Pauses execution for a specified number of milliseconds.

### Syntax

```text
wait <duration_ms>
```

### Examples

```text
wait 500
wait 1000
```

The duration can also be stored in a variable:

```text
set delay 500
wait $delay
```

Expressions can also be used:

```text
wait #[250+250]
```

The delay cannot be negative:

```text
wait -500
```

Invalid values are rejected:

```text
wait abc
```

`wait` accepts integer durations in milliseconds.

---

## Variables

Variables are created using `set` and referenced using `$`.

```text
set speed 100
set angle 45.5

print $speed
move JOINT_1 $angle
wait $speed
```

Variables store floating-point values.

Using an undefined variable results in an error:

```text
move BASE $UNKNOWN
```

### Variable Names

Variable names must:

* Start with a letter or `_`
* Contain only letters, digits, and `_`

Valid examples:

```text
set speed 100
set _offset 10
set motor_angle 90
```

Invalid examples:

```text
set 123value 10
set motor-angle 90
set motor.angle 90
```

---

## Expressions

Expressions can be used wherever a numeric value is accepted.

Expressions are enclosed in `#[...]`.

**Expressions must not contain whitespace.**

```text
#[10+20]     // Valid
#[10 + 20]   // Invalid
```

### Examples

```text
set A 100
set B #[50+25]

print #[$A+$B]
move JOINT_1 #[45*2]
wait #[250+250]
```

Expressions support the following arithmetic operators:

```text
+   Addition
-   Subtraction
*   Multiplication
/   Division
```

Parentheses can be used to control evaluation order:

```text
print #[(10+20)*2]
```

Variables can be referenced inside expressions using `$`:

```text
set A 100
set B 50

print #[$A+$B]
```

Unary `+` and `-` are also supported:

```text
print #[-$A]
print #[+$B]
```

Division by zero and references to undefined variables result in an interpreter error.

---

## Comments

Use `//` to add comments.

```text
set A 121 // Set A to 121
print $A // Print A
```

Everything after `//` on the same line is treated as a comment.

Comments can also be used to temporarily disable an instruction:

```text
// move JOINT_1 90
```

---

## Complete Example

```text
// Configure initial values
set A 121
set speed 100
set delay 500

// Display values
print $A
print $speed

// Move components
move JOINT_1 $speed
move JOINT_2 120

// Wait between operations
wait $delay

// Use an expression
set speed #[50*2]
move JOINT_1 $speed

wait #[250+250]

print $speed
```

This demonstrates the basic CDSL workflow:

```text
set    → define values
print  → inspect values
move   → control components
wait   → introduce delays
#[...] → calculate values
$...   → reference variables
//     → add comments
```

---

## Validation

CDSL reports errors when instructions are malformed or contain invalid values.

For example:

```text
print
set X
set @C 12
move JOINT abc
wait -500
move BASE $UNKNOWN
```

These demonstrate:

* Invalid or missing arguments
* Invalid variable names
* Invalid numeric values
* Negative wait durations
* Undefined variables

Values supplied to `move` are also checked against the configured limits of the specified component.
