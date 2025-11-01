# Mathematical Expression Plotter

A DSL (Domain Specific Language) for plotting and visualizing mathematical expressions using Flex, Bison, and Gnuplot.

## Features

- Parse mathematical expressions with variables
- Support for basic arithmetic: `+`, `-`, `*`, `/`, `^`
- Trigonometric functions: `sin`, `cos`, `tan`, `asin`, `acos`, `atan`
- Hyperbolic functions: `sinh`, `cosh`, `tanh`
- Other functions: `exp`, `log`, `ln`, `sqrt`, `abs`
- Mathematical constants: `pi`, `e`
- Configurable plot ranges
- Expression validation (division by zero, invalid log arguments)
- Interactive REPL interface

## Modes

### Single-Function Mode (Default)
- Each expression plots immediately
- Perfect for quick visualization
- No need to type 'plot'

### Multi-Function Mode
- Collect multiple expressions
- Type 'plot' to visualize all together
- Compare functions on same graph
- Use 'list' to see stored functions
- Use 'clear' to remove all

**Switch modes anytime with the `mode` command!**

## Building
```bash
make
```

## Usage

### Single Function Mode
```bash
./graph_compiler

# Or with custom range
./graph_compiler -5 5 0.05
```

### Examples
```
f(x) = x^2 + 2*x + 1
f(x) = sin(x) * cos(x)
f(x) = exp(-x^2)
f(x) = sqrt(abs(x))
f(x) = sin(x) + cos(2*x)
f(x) = log(x^2 + 1)
f(x) = pi * sin(x/2)
```

## Supported Operations

### Operators
- Addition: `+`
- Subtraction: `-`
- Multiplication: `*`
- Division: `/`
- Exponentiation: `^`
- Unary minus: `-x`

### Functions
**Trigonometric:** sin, cos, tan, asin, acos, atan  
**Hyperbolic:** sinh, cosh, tanh  
**Other:** exp, log, ln, sqrt, abs

### Constants
- `pi` or `PI` → 3.14159...
- `e` or `E` → 2.71828...

## Dependencies

- **GCC** - C compiler
- **Flex** - Lexical analyzer generator
- **Bison** - Parser generator
- **Gnuplot** - Plotting utility

Install on Ubuntu/Debian:
```bash
sudo apt-get install gcc flex bison gnuplot
```

Install on macOS:
```bash
brew install flex bison gnuplot
```

## Project Structure
```
.
├── Makefile           # Build configuration
├── ast.h              # AST node definitions
├── ast.c              # AST operations (create, evaluate, validate)
├── expr.l             # Lexer (tokenizer)
├── expr.y             # Parser (grammar rules)
├── main.c             # Main driver program
└── README.md          # This file
```

## Cleaning
```bash
make clean
```

## Advanced Usage

### Custom Plot Range
```bash
# Plot from x=-5 to x=5 with step 0.05
./graph_compiler -5 5 0.05
```

### Command Line Arguments
```
./graph_compiler [x_min] [x_max] [step]
```
- `x_min`: Starting x value (default: -10)
- `x_max`: Ending x value (default: 10)
- `step`: Step size (default: 0.1)

## Examples Gallery
```bash
# Parabola
f(x) = x^2

# Damped oscillation
f(x) = exp(-x/5) * sin(x)

# Gaussian curve
f(x) = exp(-x^2)

# Trigonometric sum
f(x) = sin(x) + cos(2*x) + sin(3*x)

# Absolute value function
f(x) = abs(x)

# Natural logarithm
f(x) = ln(x + 1)
```

## Error Handling

The plotter validates expressions and catches:
- Division by zero (constant)
- Logarithm of non-positive numbers
- Invalid syntax

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## Author

**R-Nithyanand**

A DSL for plotting mathematical expressions using Flex, Bison, and Gnuplot.

---

Feel free to use, modify, and learn from this project!