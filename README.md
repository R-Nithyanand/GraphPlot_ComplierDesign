# Mathematical Expression Plotter

A powerful DSL (Domain Specific Language) for plotting and visualizing mathematical expressions with advanced features including user-defined variables, functions, AST visualization, and interactive zoom controls.

![Version](https://img.shields.io/badge/version-1.0-blue)
![License](https://img.shields.io/badge/license-MIT-green)
![Language](https://img.shields.io/badge/language-C-orange)

## ✨ Features

### Core Features
- Parse and evaluate mathematical expressions
- Real-time plotting with Gnuplot
- Interactive zoom/pan controls
- User-defined variables (`let a = 5`)
- User-defined functions (`def f = x^2 + a`)
- AST visualization with colors
- Constant folding optimization
- Three-Address Code (TAC) generation
- Two modes: Single (advanced) and Multi (overlay plots)

### Mathematical Operations
**Operators:** `+`, `-`, `*`, `/`, `^` (power), unary `-`

**Functions:**
- **Trigonometric:** `sin`, `cos`, `tan`, `asin`, `acos`, `atan`
- **Hyperbolic:** `sinh`, `cosh`, `tanh`
- **Logarithmic:** `log` (base 10), `ln` (natural log)
- **Other:** `exp`, `sqrt`, `abs`, `ceil`, `floor`
- **Two-argument:** `max(a,b)`, `min(a,b)`
- **Derivatives:** `d(expr)` (numerical derivative)

**Constants:** `pi` (π), `e` (Euler's number)

## 🚀 Quick Start

### Installation

**Prerequisites:**
```bash
# Ubuntu/Debian
sudo apt-get install gcc flex bison gnuplot gnuplot-qt

# macOS
brew install flex bison gnuplot
```

**Build:**
```bash
make
```

### Basic Usage
```bash
# Launch with default range [-10, 10]
./graph_compiler

# Custom range [min, max, step]
./graph_compiler -5 5 0.01
```

## 📖 Usage Guide

### Single-Function Mode (Default)

Immediate plotting with advanced features:
```
> x^2 + 2*x + 1
[Plots immediately with interactive controls]

> let a = 5
Variable 'a' = 5.0000

> def f = x^2 + a
Function 'f' defined

> f * 2
[Plots f(x) * 2]

> vars
Variables:
  a = 5.0000

> funcs
Functions:
  f(x)

> show f
[Displays AST tree for function f]

> ast x^2 + sin(x)
[Shows AST visualization]
```

### Multi-Function Mode

Overlay multiple functions:
```
> mode
★ Switched to MULTI-FUNCTION mode

f0(x) = x^2
✓ Function f0(x) added.

f1(x) = sin(x)
✓ Function f1(x) added.

f2(x) = cos(x)
✓ Function f2(x) added.

> list
Stored functions:
  f0(x) = x^2
  f1(x) = sin(x)
  f2(x) = cos(x)

> plot
[Shows all 3 functions on same graph with different colors]

> clear
All stored functions cleared.

> mode
★ Switched to SINGLE-FUNCTION mode
```

## 📝 Command Reference

### Common Commands
```
mode        - Toggle between single/multi mode
quit / exit - Exit the program
```

### Single-Mode Commands
```
let <var> = <expr>    - Define variable
def <func> = <expr>   - Define function
vars                  - List all variables
funcs                 - List all functions
show <name>           - Display function AST
ast <expr>            - Visualize expression AST
tac                   - Display Three-Address Code (TAC)
```

### Multi-Mode Commands
```
list        - Show stored functions
plot        - Plot all stored functions
clear       - Clear all stored functions
```

## 🔬 Advanced Examples

### Variables and Functions
```
> let r = 2
> let amplitude = 3
> def wave = amplitude * sin(r * x)
> wave + cos(x)
```

### More Expressions Example
```
> exp(-x^2) * sin(10*x)          # Damped oscillation
> sqrt(abs(x)) + x^2              # Composite function
> max(sin(x), cos(x))             # Maximum of two functions
> d(x^2)                          # Derivative (2x)
```

## Three-Address Code (TAC) Generation

### What is TAC?

Three-Address Code (TAC) is an intermediate representation used in compilers. Each instruction has at most three addresses (operands), making it easier to:
- Optimize code
- Generate assembly/machine code
- Understand expression evaluation order
- Debug complex expressions

### Viewing TAC

After evaluating any expression, view the generated TAC:
```bash
> x^2 + 2*x + 1
[Plot displays]

> tac
t1 = x * x
t2 = 2 * x
t3 = t1 + t2
t4 = t3 + 1
result = t4
```

### TAC Examples

#### Simple Arithmetic
```bash
> 2 + 3 * 4
> tac

Output:
t1 = 3 * 4      # Evaluate multiplication first
t2 = 2 + t1     # Then addition
result = t2
```

#### With Variables
```bash
> let a = 5
> let b = 3
> a * x + b
> tac

Output:
t1 = a * x      # Multiply variable 'a' by x
t2 = t1 + b     # Add variable 'b'
result = t2
```

#### Function Calls
```bash
> sin(x^2)
> tac

Output:
t1 = x * x      # Compute x^2
t2 = sin(t1)    # Apply sin function
result = t2
```

#### Complex Expression
```bash
> exp(-x^2) * sin(x)
> tac

Output:
t1 = -x         # Unary minus
t2 = x * x      # Square
t3 = t1 * t2    # Multiply
t4 = exp(t3)    # Exponential
t5 = sin(x)     # Sine
t6 = t4 * t5    # Final multiplication
result = t6
```

#### Nested Functions
```bash
> sqrt(sin(x) + cos(x))
> tac

Output:
t1 = sin(x)
t2 = cos(x)
t3 = t1 + t2
t4 = sqrt(t3)
result = t4
```

### TAC Format
```
<temp_var> = <operand1> <operator> <operand2>
<temp_var> = <function>(<operand>)
<temp_var> = <unary_op> <operand>
result = <final_temp_var>
```

**Components:**
- **Temporary variables:** `t1`, `t2`, `t3`, ... (auto-generated)
- **Operators:** `+`, `-`, `*`, `/`, `^`
- **Functions:** `sin()`, `cos()`, `exp()`, `sqrt()`, etc.
- **Result:** Final value stored in `result`

### TAC File Output

TAC is automatically saved to **`tac.txt`** in the current directory. You can:
```bash
# View TAC
> tac

# Or read the file directly
$ cat tac.txt

# Use TAC for further processing
$ python optimize_tac.py tac.txt
```

### Use Cases

| Use Case | Description |
|----------|-------------|
| **Compiler Education** | Learn intermediate code generation |
| **Debugging** | Understand expression evaluation order |
| **Optimization** | Identify redundant operations |
| **Code Generation** | Convert TAC to assembly/bytecode |
| **Analysis** | Study compiler transformations |

### Comparison: Expression → AST → TAC
```bash
> (a + b) * c

# View AST
> ast (a + b) * c
OP: *
  OP: +
    ID: a
    ID: b
  ID: c

# View TAC
> tac
t1 = a + b
t2 = t1 * c
result = t2
```

### Benefits of TAC

1. **Simpler than AST:** Linear sequence of instructions
2. **Optimization-friendly:** Easy to apply transformations
3. **Machine-independent:** Abstract from target architecture
4. **Debugging:** Step-by-step execution trace
5. **Teaching:** Great for compiler courses

### Advanced: TAC Optimization

Future versions may include TAC optimizations like:
- **Constant folding:** `t1 = 2 + 3` → `t1 = 5`
- **Dead code elimination:** Remove unused temporaries
- **Common subexpression elimination:** Reuse calculated values
- **Strength reduction:** `x * 2` → `x + x`

---

**Tip:** Compare AST and TAC for the same expression to understand the transformation!
```bash
> let expr = x^2 + sin(x)
> ast expr      # Tree view
> expr          # Evaluate and plot
> tac           # Linear TAC view
```

### Optimization Demo
```
> ast 2 + 3 * 4
[Shows optimized: 14 instead of expression tree]

> ast sin(0) + cos(0)
[Shows: 1.0000 after constant folding]
```

## Project Structure
```
.
├── Makefile           # Build configuration
├── LICENSE            # MIT License
├── README.md          # This file
├── ast.h              # AST node definitions
├── ast.c              # AST operations (create, evaluate, optimize)
├── symtab.h           # Symbol table definitions
├── symtab.c           # Variable/function storage
├── commands.h         # Command flags
├── tac.h              # Three-Address Code definitions
├── tac.c              # TAC generation
├── expr.l             # Lexer (Flex)
├── expr.y             # Parser (Bison)
└── main.c             # Main driver program
```

## 🛠️ Building from Source
```bash
# Clean previous builds
make clean

# Build
make

# Run
./graph_compiler

# With custom range
./graph_compiler 0 20 0.05
```

### Parse Errors

- Check parentheses are balanced
- Ensure function names are spelled correctly
- Variables must be defined before use (`let a = 5` before using `a`)

## 📊 Performance

- **Constant folding:** Expressions like `2 + 3 * 4` → `14` at parse time
- **Efficient evaluation:** Optimized AST reduces redundant calculations
- **Configurable resolution:** Adjust step size for speed vs. accuracy


## 📄 License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

**TL;DR:** Free to use, modify, and distribute with attribution.

## Author

**R-Nithyanand**

Created as an educational DSL project demonstrating:
- Lexical analysis with Flex
- Syntax parsing with Bison
- AST construction and optimization
- Symbol table management
- Interactive data visualization

---

## 🎓 Learning Resources

### Understanding the Code

**Lexical Analysis (expr.l):**
- Tokenizes input into meaningful symbols
- Recognizes keywords, operators, numbers, identifiers
- Uses Flex pattern matching

**Syntax Analysis (expr.y):**
- Defines grammar rules for expressions
- Builds Abstract Syntax Tree (AST)
- Uses Bison for LALR parsing

**Semantic Analysis (ast.c):**
- Evaluates expressions recursively
- Performs constant folding optimization
- Validates mathematical operations

**Intermediate Code Generation (tac.c):**
- Generates Three-Address Code (TAC)
- Linearizes expression tree
- Prepares for code generation/optimization
- 
**Symbol Management (symtab.c):**
- Stores user-defined variables
- Manages function definitions
- Provides lookup and storage operations

## 📚 Examples Gallery

### Basic Plotting
```bash
> x^2
> sin(x)
> exp(-x)
> sqrt(abs(x))
```

### Trigonometric Combinations
```bash
> sin(x) * cos(x)
> sin(x)^2 + cos(x)^2        # Should equal 1
> tan(x)
> sin(2*x) + cos(3*x)
```

### Advanced Functions
```bash
> exp(-x^2) * sin(10*x)       # Damped oscillation
> x * sin(1/x)                # Interesting behavior near 0
> abs(sin(x))                 # Rectified sine wave
> floor(x) + 0.5*x            # Step function with slope
```

### Using Variables
```bash
> let freq = 5
> let amp = 2
> amp * sin(freq * x)
> let phase = 1.57
> amp * sin(freq * x + phase)
```

### Using Functions
```bash
> def wave = sin(x)
> def envelope = exp(-x/5)
> wave * envelope

> def gaussian = exp(-x^2)
> gaussian * cos(10*x)
```

### Multi-Function Comparison
```bash
> mode
f0(x) = x
f1(x) = x^2
f2(x) = x^3
f3(x) = sqrt(abs(x))
> plot
```

### Derivatives
```bash
> x^2                         # Plot x²
> d(x^2)                      # Plot derivative (≈2x)
> sin(x)
> d(sin(x))                      # Plot derivative (≈cos(x))
> d(d(sin(x)))                   # Plot derivative (≈-sin(x))
```

### Edge Cases
```bash
> 1/x                         # Asymptote at x=0
> log(x)                      # Only defined for x>0
> sqrt(x)                     # Only defined for x≥0
> tan(x)                      # Discontinuities at π/2, 3π/2, etc.
```

## 🔧 Customization

### Modify Plot Range
```bash
# Default: [-10, 10] with step 0.1
./graph_compiler -20 20 0.05

# High resolution
./graph_compiler 0 10 0.001

# Custom domain
./graph_compiler -3.14 3.14 0.01
```

### Add Custom Functions

**Edit ast.c** to add new functions:
```c
// In evaluate() function, add:
if (strcmp(node->func, "myfunction") == 0) {
    return /* your calculation */;
}
```

**Edit expr.l** to add token:
```c
"myfunc"              { return MYFUNC; }
```

**Edit expr.y** to add grammar:
```c
%token MYFUNC
// ...
| MYFUNC '(' expr ')' { $$ = createFuncNode("myfunction", $3); }
```

### Change Colors

**In main.c**, modify the colors array:
```c
const char *colors[] = {
    "#FF0000",  // Red
    "#00FF00",  // Green
    "#0000FF",  // Blue
    "#FFFF00",  // Yellow
    "#FF00FF",  // Magenta
    "#00FFFF",  // Cyan
    "#FFA500"   // Orange
};
```

## Testing

### Test Suite Examples
```bash
# Test basic operations
> 2 + 3
> 2 * 3 + 4
> (2 + 3) * 4
> 2^3
> -5

# Test functions
> sin(0)                      # Should be 0
> cos(0)                      # Should be 1
> sqrt(4)                     # Should be 2
> abs(-5)                     # Should be 5

# Test constants
> pi
> e
> pi * e

# Test variables
> let x_test = 10
> x_test * 2
> vars

# Test error handling
> 1/0                         # Division by zero warning
> log(-1)                     # Invalid log argument
> sqrt(-1)                    # Invalid sqrt argument

# Test optimization
> ast 2 + 3                   # Should show: NUMBER: 5.0000
> ast sin(0) + 1              # Should show: NUMBER: 1.0000
```

## 🎯 Use Cases

### Education
- **Teaching calculus:** Visualize functions and derivatives
- **Numerical methods:** Compare analytical vs numerical solutions
- **Signal processing:** Plot waveforms and analyze frequencies

### Research
- **Quick prototyping:** Test mathematical models rapidly
- **Data visualization:** Plot experimental functions
- **Algorithm testing:** Verify mathematical algorithms visually

### Personal Projects
- **Math exploration:** Experiment with complex functions
- **Art generation:** Create mathematical art patterns
- **Learning DSLs:** Study compiler construction

## 🚀 Future Enhancements

### Planned Features
- [ ] Parametric plots: `plot(sin(t), cos(t))`
- [ ] Implicit plots: `x^2 + y^2 = 1`
- [ ] 3D surface plots: `plot3d(x^2 + y^2)`
- [ ] Export to PNG/SVG
- [ ] Multiple variables: `f(x,y)`
- [ ] Symbolic differentiation
- [ ] Integration support
- [ ] Polar coordinates
- [ ] Complex numbers
- [ ] Matrix operations

### Optimization Ideas
- [ ] Common subexpression elimination
- [ ] Algebraic simplification
- [ ] Dead code elimination
- [ ] Loop unrolling for evaluation

## 💡 Tips & Tricks

### Performance Tips
```bash
# Use larger step for faster plotting
./graph_compiler -10 10 0.5

# Use smaller step for smoother curves
./graph_compiler -10 10 0.01

# Limit range for expensive functions
./graph_compiler 0 5 0.1
> exp(exp(x))
```

### Workflow Tips
```bash
# Save common functions
> def wave1 = sin(x)
> def wave2 = cos(x)
> wave1 + wave2

# Use mode switching efficiently
> mode              # Switch to multi-mode
f0(x) = x^2
f1(x) = x^3
> plot
> mode              # Back to single-mode for analysis
> ast x^2
```

### Debugging Tips
```bash
# View AST before plotting
> ast sin(x^2 + 1)

# Check variable values
> let a = 5
> vars

# Verify function definitions
> def f = x^2 + a
> show f
```

## 📖 Mathematical Background

### Supported Function Domains

| Function | Domain | Range |
|----------|--------|-------|
| `sin(x)` | ℝ | [-1, 1] |
| `cos(x)` | ℝ | [-1, 1] |
| `tan(x)` | ℝ \ {π/2 + nπ} | ℝ |
| `exp(x)` | ℝ | (0, ∞) |
| `log(x)` | (0, ∞) | ℝ |
| `ln(x)` | (0, ∞) | ℝ |
| `sqrt(x)` | [0, ∞) | [0, ∞) |
| `abs(x)` | ℝ | [0, ∞) |

### Numerical Derivatives

The `d()` function uses central difference:
```
f'(x) ≈ [f(x+h) - f(x-h)] / (2h)
```
where h = 1e-5



## Support

### Getting Help

**Common Issues:**
1. **"gnuplot not found"** → Install gnuplot: `sudo apt-get install gnuplot`
2. **"Parse error"** → Check syntax, use `ast` to debug
3. **"Undefined identifier"** → Define variables/functions first with `let`/`def`

**Report Bugs:**
- Check existing issues on GitHub
- Provide minimal reproducible example
- Include system info (OS, gnuplot version)

### Community

- **Discussions:** Share your mathematical discoveries
- **Contributions:** Pull requests welcome!
- **Examples:** Submit interesting function examples

## Acknowledgments

### Technologies Used
- **Flex:** Fast lexical analyzer generator
- **Bison:** GNU parser generator (YACC replacement)
- **Gnuplot:** Portable command-line graphing utility
- **GCC:** GNU Compiler Collection

### Inspiration
- Traditional compiler design principles
- Interactive mathematical tools
- Educational programming language design

### References
- Dragon Book (Compilers: Principles, Techniques, and Tools)
- Flex & Bison Manual (O'Reilly)
- Gnuplot Documentation
- Mathematical function libraries

## 📈 Changelog

### Version 1.0
- 🎉 Initial release
- Basic expression parsing
- Gnuplot integration
- Standard math functions
- Implemented user-defined variables and functions
- AST visualization with colors
- Constant folding optimization
- Dual-mode operation (single/multi)
- Enhanced error handling
- Numerical derivatives
-  Added Three-Address Code (TAC) generation
- New `tac` command to view intermediate code
- TAC output saved to `tac.txt` file
- Enhanced compiler education features

## ASCII Art Examples

Try these for fun patterns:
```bash
> sin(x) + sin(2*x)/2 + sin(3*x)/3
> abs(sin(x)) * abs(cos(x))
> sin(x) * exp(-abs(x)/10)
> sqrt(abs(x)) * sin(1/x)
```

## 🌟 Star History

If you find this project useful:
- ⭐ Star the repository
- 🔀 Fork for your own experiments
- 📢 Share with fellow math enthusiasts
- 🐛 Report bugs or suggest features

---

## 📝 Quick Reference Card
```
┌─────────────────────────────────────────────────┐
│          MATH PLOTTER QUICK REFERENCE           │
├─────────────────────────────────────────────────┤
│ OPERATORS:  + - * / ^                           │
│ FUNCTIONS:  sin cos tan exp log ln sqrt abs     │
│             asin acos atan sinh cosh tanh       │
│             max(a,b) min(a,b) ceil floor        │
│ CONSTANTS:  pi e                                │
│ SPECIAL:    d(expr) - derivative                │
├─────────────────────────────────────────────────┤
│ COMMANDS:                                       │
│   mode             - Toggle single/multi mode   │
│   let a = 5        - Define variable            │
│   def f = x^2      - Define function            │
│   vars             - List variables             │
│   funcs            - List functions             │
│   tac              - Show Three-Address Code    │
│   show <name>      - Display function AST       │
│   ast <expr>       - Show expression AST        │
│   list             - Show stored (multi-mode)   │
│   plot             - Plot all (multi-mode)      │
│   clear            - Clear all (multi-mode)     │
│   quit             - Exit program               │
└─────────────────────────────────────────────────┘
```
 
---

**Made with ❤️ for mathematics and compiler enthusiasts**

*Happy Plotting! 📊✨*

---

## 📜 License Summary
```
MIT License - Copyright (c) 2025 R-Nithyanand

✅ Commercial use     ✅ Modification
✅ Distribution       ✅ Private use
❌ Liability          ❌ Warranty
```

For full license text, see [LICENSE](LICENSE) file.

---

**⭐ If this project helped you, consider giving it a star!**