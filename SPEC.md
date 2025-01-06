# The Stone Programming Language Specification

## Introduction 

Stone is a modern programming language designed for high-performance applications, focusing on simplicity, clarity, and efficiency. It provides a unified and scalable approach to programming with advanced features like modules, structured memory management, and clean syntax.

## Lexical Structure

### Whitespace and Comments
- Whitespace is ignored except as a delimiter.
- Single-line comments begin with `//`.
- Multi-line comments are enclosed within `/* */`.

## Constants

Stone supports constant values that are immutable. These include:
- Numeric literals (e.g., `42`, `3.14`)
- Boolean literals (`true`, `false`)
- String literals (e.g., `"Hello, World!"`)

## Variables

Variables are declared using keywords like `auto` or explicit types. Stone enforces strong typing and supports both mutable and immutable variables.

#### Example:
```stone
auto x = 42;      // Type inferred
const int y = 10; // Immutable
```

## Types

Stone provides a rich type system, including:
- Primitive types: `int`, `float`, `bool`, etc.
- Composite types: `struct`, `interface`
- User-defined types

### Supported Types:
- `int`, `int8`, `int16`, `int32`, `int64`
- `uint`, `uint8`, `uint16`, `uint32`, `uint64`
- `float`, `float32`, `float64`
- `complex32`, `complex64`
- `imaginary32`, `imaginary64`
- `char`, `char8`, `char16`, `char32`
- `string`
- `bool`

## Expressions

Expressions are constructs that produce a value and include:
- Arithmetic operations: `+`, `-`, `*`, `/`
- Logical operations: `&&`, `||`, `!`
- Comparison: `==`, `!=`, `<`, `>`
- Assignment: `=`

### Example:
```stone
int result = (a + b) * c;
bool flag = (x > y) && !z;
```

## Statements

Statements perform actions and include:
- Conditional statements: `if`, `else`, `switch`
- Looping constructs: `for`, `while`, `do`
- Jump statements: `return`, `break`, `continue`

### Example:
```stone
if (x > 0) {
    print("Positive");
} else {
    print("Non-positive");
}
```

## Declarations

### Functions
Functions are defined with `fun` and must use braces for their bodies. Function names are in upper camel case.

#### Example:
```stone
public fun Add(int a, int b) -> int {
    return a + b;
}
```

### Structs
Structs define composite data types.

#### Example:
```stone
struct Particle {
    int mass;
    int charge;
}
```

### Interfaces
Interfaces define behavior without implementation.

#### Example:
```stone
interface Fireable {
    fun Fire() -> bool;
}
```

### Virtual Functions
Virtual functions provide polymorphism by allowing derived types to override base type behavior.

#### Example:
```stone
struct Particle {
    int mass;
    int charge;
}

virtual public fun Particle::Fire() -> bool {
    return false;  // Default behavior
}

struct Electron : Particle {
}

override public fun Electron::Fire() -> bool {
    return true;
}
```

### Anonymous Functions
Anonymous functions allow inline function definitions for quick computations.

#### Example:
```stone
public fun Main() -> void {
    auto square = fun(int x) -> int {
        return x * x;
    };

    print(square(5)); // Outputs: 25
}
```

## Pointers and Cleanup

Stone provides `ptr`, `ref`, and `val` for pointer management, ensuring safe and readable memory operations. Cleanup code can be written using `~fun()`.

#### Example:
```stone
public fun Main() -> void {
    ptr int particleMass = ref 10;
    ptr int particleCharge = ref 1;

    ~fun() {
        delete particleMass;
        delete particleCharge;
        print("Resources cleaned up.");
    }

    print("Particle mass: " + auto particleMass);
}
```

#### Example:
```stone
public fun Main() -> void {
    int x = 10, y = 20;

    ptr ... int p1 = ref x;
    ptr .. int p2 = ref y;

    auto result = (auto ... p1) * 2 + (auto .. p2);
    print(result);  // Outputs: 40

    ~fun() {
        delete p1;
        delete p2;
        print("Pointers cleaned up.");
    }
}
```


## Built-in and Reserved Keywords

### Active Declaration Keywords
- `interface`
- `enum`
- `struct`
- `this`
- `private`
- `internal`
- `public`
- `static`
- `space`
- `inline`
- `new`
- `const`
- `stone`
- `volatile`
- `restrict`
- `mutable`
- `delete`
- `operator`
- `fun`
- `bool`
- `auto`
- `void`
- `int`, `int8`, `int16`, `int32`, `int64`
- `uint`, `uint8`, `uint16`, `uint32`, `uint64`
- `byte`, `ubyte`
- `float`, `float32`, `float64`
- `complex32`, `complex64`
- `imaginary32`, `imaginary64`
- `real`
- `char`, `char8`, `char16`, `char32`
- `string`
- `alias`
- `import`

### Reserved Declaration Keywords
- `type`
- `ptr`
- `any`
- `object`
- `class`
- `immutable`
- `module`
- `global`
- `register`

### Statement Keywords
- `if`
- `else`
- `for`
- `in`
- `while`
- `return`
- `break`
- `continue`
- `next`
- `switch`
- `case`
- `default`

### Expression Keywords
- `true`
- `false`
- `null`

### Pound Keywords
- `if`
- `else`
- `file`

## Modules
Modules in Stone are explicit and scalable. Files must declare their `space`, and the `join` keyword defines their module.

### Example:
```stone
// Particle.stone
join Physics;

struct Particle {
    int mass;
    int charge;
}
```

```stone
// Electron.stone
join Physics;

struct Electron : Particle {
}

public fun Electron::Electron(ptr int mass, ptr int charge) -> void {
    this:mass = auto mass;
    this:charge = auto charge;
}
```

```stone
// Main.stone
import Physics;

space Simulation;

public fun Main() -> void {
    ptr int electronMass = ref 9;
    ptr int electronCharge = ref -1;

    auto electron = Electron(ref electronMass, ref electronCharge);

    ~fun() {
        delete electronMass;
        delete electronCharge;
        print("Cleanup complete.");
    }

    print("Electron charge: " + auto electronCharge);
}
```

Modules are built independently and linked using the `stone` compiler:

### Build Process:
1. Compile modules:
    ```bash
    stone --module Physics Particle.stone Electron.stone
    ```
2. Compile and link `Main.stone` with `Physics`:
    ```bash
    stone --import-module Physics Main.stone -o Simulation
    ```

