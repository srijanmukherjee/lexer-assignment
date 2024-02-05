# lexer-assignment
A simple lexer implementation using C

## Running instruction

```shell
# Build the project
$ make

# Run the lexer
$ ./main my_source_code
```

## Tokens
```c
typedef enum {
    TOK_ERROR = -1,

    // identifier
    TOK_IDENTIFIER,

    // numbers
    TOK_INT,
    TOK_DOUBLE,
    TOK_SCIENTIFIC,

    // operators
    TOK_RELOP,

    // End of file
    TOK_EOF,

    // keywords
    TOK_KEYWORD_AUTO,
    TOK_KEYWORD_BREAK,
    TOK_KEYWORD_CASE,
    TOK_KEYWORD_CHAR,
    TOK_KEYWORD_CONST,
    TOK_KEYWORD_CONTINUE,
    TOK_KEYWORD_DEFAULT,
    TOK_KEYWORD_DO,
    TOK_KEYWORD_DOUBLE,
    TOK_KEYWORD_ELSE,
    TOK_KEYWORD_ENUM,
    TOK_KEYWORD_EXTERN,
    TOK_KEYWORD_FLOAT,
    TOK_KEYWORD_FOR,
    TOK_KEYWORD_GOTO,
    TOK_KEYWORD_IF,
    TOK_KEYWORD_INT,
    TOK_KEYWORD_LONG,
    TOK_KEYWORD_REGISTER,
    TOK_KEYWORD_RETURN,
    TOK_KEYWORD_SHORT,
    TOK_KEYWORD_SIGNED,
    TOK_KEYWORD_SIZEOF,
    TOK_KEYWORD_STATIC,
    TOK_KEYWORD_STRUCT,
    TOK_KEYWORD_SWITCH,
    TOK_KEYWORD_TYPEDEF,
    TOK_KEYWORD_UNION,
    TOK_KEYWORD_UNSIGNED,
    TOK_KEYWORD_VOID,
    TOK_KEYWORD_VOLATILE,
    TOK_KEYWORD_WHILE,

    // Literals
    TOK_STRING_LITERAL,

    // Symbols
    TOK_R_BRACE,
    TOK_L_BRACE,
    TOK_R_PARAN,
    TOK_L_PARAN,
    TOK_R_SQUARE_BRACKET,
    TOK_L_SQUARE_BRACKET,
    TOK_R_ANGLE_BRACKET,
    TOK_L_ANGLE_BRACKET,

    // Operators
    TOK_ARITHMETIC_OPERATOR,
    TOK_LOGICAL_OPERATOR,

    // Misc
    TOK_EQUAL,
    TOK_SEMI_COLON,
    TOK_COLON,
    TOK_COMMA,
    TOK_DOT
} TokenType;

typedef enum { RELOP_LT, RELOP_GT, RELOP_EQ, RELOP_LE, RELOP_GE, RELOP_NE } RelOp;

typedef enum {
    A_OP_PLUS,
    A_OP_MINUS,
    A_OP_DIV,
    A_OP_MUL,
    A_OP_EXP,
    A_OP_MOD,
    A_OP_DOUBLE_PLUS,
    A_OP_DOUBLE_MINUS
} ArithmeticOperator;

typedef enum { L_OP_AND, L_OP_OR, L_OP_NOT } LogicalOperator;
```