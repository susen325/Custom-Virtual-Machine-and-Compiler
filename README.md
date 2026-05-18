# CVM++ (Custom Virtual Machine & Compiler)

CVM++ is a lightweight, stack-based virtual machine and a custom compiler for a simple scripting language. This project was developed to demystify the process of translating source code into executable bytecode.

## Features

- **Recursive Descent Parser:** Generates an Abstract Syntax Tree (AST).
- **Bytecode Compiler:** Converts AST into a proprietary instruction set.
- **Stack-Based VM:** Executes bytecode with support for local variables and control flow.
- **Language Syntax:**
  - **Variables:** `let x = 10;`
  - **Assignment:** `x = 20;`
  - **Arithmetic:** `+`, `-`, `*`, `/`
  - **Comparison:** `==`, `<`
  - **Control Flow:** `if (condition) { ... } else { ... }`, `while (condition) { ... }`
  - **I/O:** `print expression;`, `let x = input;`
- **Interactive REPL:** Persistent state across multiple lines.
- **Debugging Tools:** Flags to visualize AST and Bytecode.

## Building the Project

Ensure you have `g++` (supporting C++17) installed.

```bash
g++ -std=c++17 src/*.cpp -o cvm.exe
```

## Usage

### Running a Script

To run a `.cvm` file:

```bash
./cvm.exe path/to/script.cvm
```

### Interactive REPL

Start the REPL by running the executable without arguments:

```bash
./cvm.exe
```

### Debugging Flags

- `--ast`: Show the Abstract Syntax Tree before execution.
- `--code`: Show the compiled bytecode (disassembly).

Example:
```bash
./cvm.exe --ast --code script.cvm
```

## Example Script

```javascript
let i = 0;
while (i < 5) {
    print i;
    i = i + 1;
}

if (i == 5) {
    print 100;
}
```

## Instruction Set Architecture (ISA)

| OpCode | Description |
| :--- | :--- |
| `CONSTANT` | Pushes a constant value from the pool onto the stack. |
| `ADD`, `SUB`, `MUL`, `DIV` | Binary arithmetic operations. |
| `EQUAL`, `LESS` | Comparison operations. |
| `PRINT` | Pops and prints the top value of the stack. |
| `GET_LOCAL`, `SET_LOCAL` | Variable access and assignment. |
| `JUMP`, `JUMP_IF_FALSE` | Forward branching. |
| `LOOP` | Backward jumping for loops. |
| `INPUT` | Takes integer input from the user. |
| `HALT` | Stops execution. |
