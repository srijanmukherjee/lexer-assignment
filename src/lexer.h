#ifndef __LEXER_H__
#define __LEXER_H__

#include <limits.h>
#include <stdint.h>
#include <stdio.h>

#include "symbol_table.h"

#define LEXER_BUFFER_SIZE 4096

typedef struct {
    const char *filepath;
    FILE *source;
    ST *st;
    int row;
    int col;
    char last_char;
    // for both scientific and double/float values
    double val_double;
    int val_int;
} Lexer;

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
    TOK_STRING_LITERAL
} TokenType;

typedef enum { RELOP_LT, RELOP_GT, RELOP_EQ, RELOP_LE, RELOP_GE, RELOP_NE } RelOp;

typedef struct {
    TokenType type;
    int value;
} Token;

/**
 * creates a new lexer context
 * @param filepath input source file
 */
Lexer *create_lexer(const char *filepath);
Token get_token(Lexer *lexer);

#endif