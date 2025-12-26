# LFI3A Programming Language Documentation

LFI3A (pronounced "lfi-3a") is a beginner-friendly programming language that blends programming concepts with Moroccan Arabic (Darija) vocabulary. Created by Oussama Jabrane as a personal project to break free from the "computer fixer" stereotype, LFI3A offers a unique and culturally-inspired approach to learning programming.

## 📋 Table of Contents

- [Quick Start](#-quick-start)
- [Language Basics](#-language-basics)
- [Data Types & Variables](#️-data-types--variables)
- [Control Flow](#-control-flow)
- [Loops](#-loops)
- [Functions](#-functions)
- [Project Structure](#️-project-structure)
- [Examples](#-examples)
- [Limitations](#-limitations)
- [Why LFI3A?](#-why-lfi3a)
- [Contributing](#-contributing)
- [License](#-license)

## 🚀 Quick Start

### Installation

1. Clone the repository (if applicable)
```bash
git clone https://github.com/Oussama-jabrane/lfi3a.git
cd lfi3a
```

2. Compile the interpreter:

```bash
g++ -std=c++17 src/*.cpp -o lfi3a
```

### Your First Program

Create a file called `hello.lfi3a`:

```lfi3a
// My first LFI3A program
dir ism = "Oussama"
dir age = 25
kteb("Salam", ism, "!")
kteb("3ndek", age, "years")
```

Run it:

```bash
./lfi3a hello.lfi3a
```

Output:

```
Salam Oussama !
3ndek 25 years
```

## 📖 Language Basics

### Syntax Overview

LFI3A is dynamically typed (no need to specify variable types) and uses Darija-inspired keywords. Semicolons are optional.

### Comments

```lfi3a
// This is a single-line comment
dir x = 5  // Comments can follow code
```

## 🏷️ Data Types & Variables

### Variable Declaration

Use `dir` ("give/put" in Darija) to create variables:

```lfi3a
dir number = 42           // Integer
dir decimal = 3.14        // Float
dir name = "LFI3A"        // String
dir is_true = s7i7        // Boolean true
dir is_false = ghalat     // Boolean false
```

### Strings

Strings can be concatenated with `+`:

```lfi3a
dir greeting = "Salam" + " " + "Oussama"
kteb(greeting)  // Output: Salam Oussama
```

### Boolean Values

- `s7i7` = true (literally "correct")
- `ghalat` = false (literally "wrong")

## 🔄 Control Flow

### Conditional Statements

```lfi3a
dir score = 85

ila (score >= 90) {
    kteb("Excellent!")              // If score >= 90
} wila (score >= 70) {
    kteb("Good job!")               // Else if score >= 70
} wla {
    kteb("Try harder next time!")   // Else
}
```

### Comparison Operators

```lfi3a
==   // Equal to
!=   // Not equal to
<    // Less than
>    // Greater than
<=   // Less than or equal to
>=   // Greater than or equal to
```

### Logical Operators

```lfi3a
w    // AND (means "and" in Darija)
wla  // OR (means "or" in Darija)

// Example:
ila (age > 18 w age < 30) {
    kteb("Young adult")
}
```

## 🔁 Loops

### While Loop (ma7ad)

`ma7ad` means "while" or "as long as" in Darija:

```lfi3a
dir counter = 1
ma7ad (counter <= 5) {
    kteb("Iteration:", counter)
    counter = counter + 1
}
```

### For Loop (kol)

`kol` means "each" or "every" in Darija:

```lfi3a
kol (i = 0; i < 5; i++) {
    kteb("Number:", i)
}
```

## 📦 Functions

### Function Declaration

Use `dalla` (means "defined" or "set" in Darija) to create functions:

```lfi3a
dalla greet(name) {
    kteb("Salam", name)
    rje3 "Welcomed " + name
}

dir result = greet("Oussama")
kteb(result)  // Output: Welcomed Oussama
```

### Return Statement

Use `rje3` (means "return" in Darija) to send back a value:

```lfi3a
dalla add(a, b) {
    rje3 a + b
}

dir sum = add(10, 20)  // sum = 30
```

### Recursion

LFI3A supports recursive functions:

```lfi3a
dalla factorial(n) {
    ila (n <= 1) {
        rje3 1
    }
    rje3 n * factorial(n - 1)
}

kteb(factorial(5))  // Output: 120
```

## 🏗️ Project Structure

```
lfi3a/
├── src/                    # Source code
│   ├── main.cpp           # Entry point
│   ├── Token.hpp          # Token definitions
│   ├── lexer.hpp/cpp      # Lexical analyzer
│   ├── parser.hpp/cpp     # Syntax parser
│   ├── AST.hpp            # Abstract Syntax Tree
│   └── interpreter.hpp/cpp # Program executor
├── examples/              # Sample programs
│   ├── hello.lfi3a       # Basic example
│   ├── test_features.lfi3a # All features
│   ├── functions.lfi3a   # Function examples
│   └── comments_and_recursion.lfi3a
└── README.md             # This documentation
```

### Architecture Components

- **Lexer**: Breaks code into tokens
- **Parser**: Builds syntax tree from tokens
- **Interpreter**: Executes the syntax tree

## 📚 Examples

### Example 1: Basic Calculator

```lfi3a
dalla calculate(x, y, operation) {
    ila (operation == "+") {
        rje3 x + y
    } wila (operation == "-") {
        rje3 x - y
    } wila (operation == "*") {
        rje3 x * y
    } wla {
        rje3 x / y
    }
}

kteb("5 + 3 =", calculate(5, 3, "+"))
kteb("10 / 2 =", calculate(10, 2, "/"))
```

### Example 2: Number Guessing Game

```lfi3a
dir target = 42
dir attempts = 0

ma7ad (s7i7) {
    kteb("Guess a number:")
    dir guess = 50  // In real code, this would be user input
    
    ila (guess == target) {
        kteb("Correct! Attempts:", attempts)
        break
    } wila (guess < target) {
        kteb("Too low!")
    } wla {
        kteb("Too high!")
    }
    
    attempts = attempts + 1
}
```

## 📝 Language Keywords Reference

| Keyword | Meaning | Usage |
|---------|---------|-------|
| `dir` | Declare variable | `dir x = 5` |
| `kteb` | Print/output | `kteb("Hello")` |
| `ila` | If statement | `ila (condition) {...}` |
| `wila` | Else if | `wila (condition) {...}` |
| `wla` | Else/OR | `wla {...}` / `a wla b` |
| `ma7ad` | While loop | `ma7ad (condition) {...}` |
| `kol` | For loop | `kol (i=0; i<10; i++) {...}` |
| `dalla` | Function | `dalla func() {...}` |
| `rje3` | Return | `rje3 value` |
| `s7i7` | True | `dir flag = s7i7` |
| `ghalat` | False | `dir flag = ghalat` |
| `w` | AND | `a w b` |
| `wla` | OR | `a wla b` |

## ⚠️ Limitations
<a id="limitations"></a>

### Current Version

- **Global scope only**: All variables are global
- **Basic data structures**: No arrays or objects yet
- **Simple type system**: Strings and numbers are somewhat interchangeable
- **No modules**: Cannot import external code
- **No error recovery**: First error stops execution

### Future Improvements Planned

- Local variable scope in functions
- Arrays and dictionaries
- Type checking
- Standard library functions
- File I/O operations

## 🎯 Why LFI3A?

### For Beginners

- **Familiar vocabulary**: Uses words from everyday Moroccan Arabic
- **Simple syntax**: No complex type declarations or boilerplate
- **Clear error messages**: Designed to be beginner-friendly

### For Moroccan Developers

- **Cultural connection**: Bridges programming with local language
- **Educational tool**: Great for teaching programming in Darija
- **Community project**: Open for contributions and extensions

## 🤝 Contributing

Found a bug? Have a feature request? Want to add more Darija words?

1. Check the GitHub repository (if available)
2. Submit issues or pull requests
3. Join the discussion in Oussama's YouTube channel

## 📄 License

LFI3A is released as an educational project. See the project repository for specific licensing information.

---

Created with ❤️ by **Oussama Jabrane**. Inspired by Moroccan culture and a desire to make programming more accessible.

