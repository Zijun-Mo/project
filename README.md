# ****README****

This project is a simple relational database management system that implements basic SQL statement parsing and execution functions. Below is a brief description of each module.

## 1. Lexer

**Files:** `Lexer.hpp`, `Lexer.cpp`

- **Function:** Converts the input SQL string into a sequence of tokens, preparing for syntax analysis.
- **Implementation:**
  - Define the `TokenType` enum and `Token` struct to represent different types of tokens, such as keywords, identifiers, symbols, numbers, strings, etc.
  - Use the `Lexer` class to scan the input string character by character, recognizing and generating the corresponding tokens.
  - Maintain a set of keywords to recognize keywords in SQL statements.

## 2. Parser

**Files:** `parser.hpp`, `parser.cpp`

- **Function:** Performs syntax analysis on the token sequence and executes the corresponding database operations.
- **Implementation:**
  - The `Parser` class contains multiple parsing functions, such as `parseCreateDatabase`, `parseCreateTable`, `parseInsert`, `parseSelect`, `parseUpdate`, etc., corresponding to different SQL statements.
  - Use helper functions like `match` and `expect` to gradually parse the token sequence, build the syntax tree, or directly execute operations.
  - Use data structures such as `Condition` and `Join` to represent conditions and join operations in SQL.
  - Provide functions like `executeSelect` and `evaluateConditions` to execute specific queries and condition evaluations.

## 3. Calculation Module

**Files:** `calculation.hpp`, `calculation.cpp`

- **Function:** Evaluates expressions in SQL statements, supporting basic arithmetic operations.
- **Implementation:**
  - The `Calculation` class receives a token sequence (representing an infix expression) and provides the `execute` method to perform the calculation.
  - Use the `infixToPostfix` method to convert infix expressions to postfix expressions for easier computation.
  - Use the `evaluatePostfix` method to calculate the value of postfix expressions, supporting operations like addition, subtraction, multiplication, and division.
  - Use a stack structure to parse and calculate the value of expressions step by step according to the algorithm.

## 4. Main Program

**Files:** `main.cpp`

- **Function:** Entry point of the program, handling command-line input and file I/O.
- **Implementation:**
  - Get the input file and output file paths from command-line arguments and convert them to absolute paths.
  - Read SQL statements from the input file, use `Lexer` to parse them into token sequences, and then use `Parser` to parse and execute them.
  - Write the execution results to the output file and handle possible exceptions.

## 5. Test Program

**Files:** `test.cpp`

- **Function:** Provides an interactive testing environment for debugging and testing each functional module.
- **Implementation:**
  - Use a loop to read SQL statements from standard input until the user exits.
  - Parse and execute the input SQL statements using `Lexer` and `Parser`, and output the results.
  - Facilitate quick testing of syntax parsing and execution correctness during development.

## 6. Project Features

- **Modular Design:** Each functional module is independent, achieving a clear hierarchical structure that is easy to maintain and extend.
- **Basic SQL Support:** Implements common SQL operations such as creating databases and tables, inserting, querying, updating, and deleting data.
- **Error Handling:** Provides basic error detection and prompts to help users understand issues in their input.

## 7. Usage

1. **Compile the project:** Use a compiler that supports C++17 or higher standards to compile the project.
   ```bash
   cd build
   cmake ..
   make
   ```
2. **Run the program:** Use the following command to run the program, passing the input file path and output file path as arguments.
   ```bash
   cd ../bin
   ./minidb input.txt output.txt
   ```