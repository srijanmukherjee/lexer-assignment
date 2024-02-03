#include "lexer.h"

#include <ctype.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "string.h"
#include "symbol_table.h"

typedef struct {
    const char *str;
    TokenType token_class;
} Keyword;

Keyword keywords[] = {{"auto", TOK_KEYWORD_AUTO},         {"break", TOK_KEYWORD_BREAK},
                      {"case", TOK_KEYWORD_CASE},         {"char", TOK_KEYWORD_CHAR},
                      {"const", TOK_KEYWORD_CONST},       {"continue", TOK_KEYWORD_CONTINUE},
                      {"default", TOK_KEYWORD_DEFAULT},   {"do", TOK_KEYWORD_DO},
                      {"double", TOK_KEYWORD_DOUBLE},     {"else", TOK_KEYWORD_ELSE},
                      {"enum", TOK_KEYWORD_ENUM},         {"extern", TOK_KEYWORD_EXTERN},
                      {"float", TOK_KEYWORD_FLOAT},       {"for", TOK_KEYWORD_FOR},
                      {"goto", TOK_KEYWORD_GOTO},         {"if", TOK_KEYWORD_IF},
                      {"int", TOK_KEYWORD_INT},           {"long", TOK_KEYWORD_LONG},
                      {"register", TOK_KEYWORD_REGISTER}, {"return", TOK_KEYWORD_RETURN},
                      {"short", TOK_KEYWORD_SHORT},       {"signed", TOK_KEYWORD_SIGNED},
                      {"sizeof", TOK_KEYWORD_SIZEOF},     {"static", TOK_KEYWORD_STATIC},
                      {"struct", TOK_KEYWORD_STRUCT},     {"switch", TOK_KEYWORD_SWITCH},
                      {"typedef", TOK_KEYWORD_TYPEDEF},   {"union", TOK_KEYWORD_UNION},
                      {"unsigned", TOK_KEYWORD_UNSIGNED}, {"void", TOK_KEYWORD_VOID},
                      {"volatile", TOK_KEYWORD_VOLATILE}, {"while", TOK_KEYWORD_WHILE}};

static TokenType get_keyword_class(const char *id) {
    if (sizeof(keywords) == 0) return TOK_ERROR;

    for (int i = 0; i < sizeof(keywords) / sizeof(keywords[0]); i++) {
        if (strcmp(id, keywords[i].str) == 0) {
            return keywords[i].token_class;
        }
    }

    return TOK_ERROR;
}

Lexer *create_lexer(const char *filepath) {
    if (filepath == NULL) {
        fprintf(stderr, "ERROR: filepath is NULL\n");
        return NULL;
    }

    FILE *file = fopen(filepath, "r");
    if (file == NULL) {
        return NULL;
    }

    Lexer *lexer = malloc(sizeof(Lexer));
    if (lexer == NULL) {
        fprintf(stderr, "ERROR: not enough memory\n");
        return NULL;
    }

    lexer->col = -1;
    lexer->row = 1;
    lexer->filepath = filepath;
    lexer->source = file;
    lexer->last_char = ' ';
    lexer->st = st_create();
    return lexer;
}

static bool isdelim(uint8_t c) { return isspace(c) || c == '\t' || c == '\r' || c == '\n'; }

static void next_char(Lexer *lexer) {
    lexer->col++;
    lexer->last_char = fgetc(lexer->source);
    if (lexer->last_char == '\n') {
        lexer->row++;
        lexer->col = 1;
    }
}

static void report_error(Lexer *lexer, const char *format, ...) {
    va_list args;
    va_start(args, format);
    fprintf(stderr, "%s:%d:%d: ", lexer->filepath, lexer->row, lexer->col);
    vfprintf(stderr, format, args);
    fprintf(stderr, "\n");
    va_end(args);
}

static Token process_number(Lexer *lexer) {
    String str = new_string();
    do {
        string_append_char(str, lexer->last_char);
        next_char(lexer);
    } while (isdigit(lexer->last_char));

    if (lexer->last_char != '.' && tolower(lexer->last_char) != 'e') {
        lexer->val_int = strtol(str->buf, NULL, 10);
        free_string(str);
        return (Token){TOK_INT};
    }

    if (tolower(lexer->last_char) == 'e') goto scientific;

    do {
        string_append_char(str, lexer->last_char);
        next_char(lexer);
    } while (isdigit(lexer->last_char));

    if (tolower(lexer->last_char) != 'e') {
        lexer->val_double = strtod(str->buf, NULL);
        free_string(str);
        return (Token){TOK_DOUBLE};
    }

scientific:
    string_append_char(str, lexer->last_char);
    next_char(lexer);

    // ERROR: nothing after e
    if (lexer->last_char != '+' && lexer->last_char != '-' && !isdigit(lexer->last_char)) {
        free_string(str);
        report_error(lexer, "Expected [+-](digit+) after e");
        return (Token){TOK_ERROR};
    }

    int digit_count = 0;
    do {
        if (isdigit(lexer->last_char)) digit_count++;
        string_append_char(str, lexer->last_char);
        next_char(lexer);
    } while (isdigit(lexer->last_char));

    // ERROR: nothing after e[+-]
    if (digit_count == 0 || lexer->last_char == '.') {
        free_string(str);
        report_error(lexer, "Expected [+-](digit+) after e");
        return (Token){TOK_ERROR};
    }

    lexer->val_double = strtod(str->buf, NULL);
    free_string(str);
    return (Token){TOK_SCIENTIFIC};
}

Token get_token(Lexer *lexer) {
    // skip delimeters
    while (isdelim(lexer->last_char)) next_char(lexer);

    // identifier + keyword
    if (isalpha(lexer->last_char) || lexer->last_char == '_') {
        // read the whole identifier
        String str = new_string();
        do {
            string_append_char(str, lexer->last_char);
            next_char(lexer);
        } while (isalnum(lexer->last_char) || lexer->last_char == '_');

        // get c string
        // basically trims the extra preallocated portion from the String
        const char *identifier = string_c_str(str);
        free_string(str);

        // check if identifier is a keyword
        TokenType keyword_class = get_keyword_class(identifier);

        // this identifier is not a token
        if (keyword_class == TOK_ERROR) {
            return (Token){TOK_IDENTIFIER, st_insert(lexer->st, identifier)};
        }

        // this identifier is a keyword
        return (Token){keyword_class};
    }

    // number -> int + float + scientific
    if (isdigit(lexer->last_char) || lexer->last_char == '.') {
        return process_number(lexer);
    }

    if (lexer->last_char == '>') {
        next_char(lexer);
        if (lexer->last_char == '=') {
            next_char(lexer);
            return (Token){TOK_RELOP, RELOP_GE};
        }
        return (Token){TOK_RELOP, RELOP_GT};
    }

    if (lexer->last_char == '<') {
        next_char(lexer);
        if (lexer->last_char == '=') {
            next_char(lexer);
            return (Token){TOK_RELOP, RELOP_LE};
        } else if (lexer->last_char == '>') {
            next_char(lexer);
            return (Token){TOK_RELOP, RELOP_NE};
        }
        return (Token){TOK_RELOP, RELOP_LT};
    }

    if (lexer->last_char == '=') {
        next_char(lexer);
        if (lexer->last_char == '=') {
            next_char(lexer);
            return (Token){TOK_RELOP, RELOP_EQ};
        }
        return (Token){TOK_EQUAL};
    }

    if (lexer->last_char == '"' || lexer->last_char == '\'') {
        char string_literal_start = lexer->last_char;
        String str = new_string();
        next_char(lexer);
        while (lexer->last_char != EOF && lexer->last_char != string_literal_start) {
            string_append_char(str, lexer->last_char);
            next_char(lexer);
        }

        if (lexer->last_char != string_literal_start) {
            report_error(lexer, "Unterminated string");
            return (Token){TOK_ERROR};
        }

        const char *s = string_c_str(str);
        free_string(str);
        next_char(lexer);

        return (Token){TOK_STRING_LITERAL, st_insert(lexer->st, s)};
    }

    if (lexer->last_char == '+') {
        next_char(lexer);
        if (lexer->last_char == '+') {
            next_char(lexer);
            return (Token){TOK_ARITHMETIC_OPERATOR, A_OP_DOUBLE_PLUS};
        }
        return (Token){TOK_ARITHMETIC_OPERATOR, A_OP_PLUS};
    }

    if (lexer->last_char == '-') {
        next_char(lexer);
        if (lexer->last_char == '-') {
            next_char(lexer);
            return (Token){TOK_ARITHMETIC_OPERATOR, A_OP_DOUBLE_MINUS};
        }
        return (Token){TOK_ARITHMETIC_OPERATOR, A_OP_MINUS};
    }

    if (lexer->last_char == '%') {
        next_char(lexer);
        return (Token){TOK_ARITHMETIC_OPERATOR, A_OP_MOD};
    }

    if (lexer->last_char == '/') {
        next_char(lexer);
        // comment
        if (lexer->last_char == '/') {
            do {
                next_char(lexer);
            } while (lexer->last_char != '\n' && lexer->last_char != EOF);
            return get_token(lexer);
        }
        return (Token){TOK_ARITHMETIC_OPERATOR, A_OP_DIV};
    }

    if (lexer->last_char == '*') {
        next_char(lexer);
        if (lexer->last_char == '*') {
            next_char(lexer);
            return (Token){TOK_ARITHMETIC_OPERATOR, A_OP_EXP};
        }
        return (Token){TOK_ARITHMETIC_OPERATOR, A_OP_MUL};
    }

    if (lexer->last_char == '(') {
        next_char(lexer);
        return (Token){TOK_L_PARAN};
    }

    if (lexer->last_char == ')') {
        next_char(lexer);
        return (Token){TOK_R_PARAN};
    }

    if (lexer->last_char == '[') {
        next_char(lexer);
        return (Token){TOK_L_SQUARE_BRACKET};
    }

    if (lexer->last_char == ']') {
        next_char(lexer);
        return (Token){TOK_R_SQUARE_BRACKET};
    }

    if (lexer->last_char == '{') {
        next_char(lexer);
        return (Token){TOK_L_BRACE};
    }

    if (lexer->last_char == '}') {
        next_char(lexer);
        return (Token){TOK_R_BRACE};
    }

    // Logical operators
    if (lexer->last_char == '&') {
        next_char(lexer);
        if (lexer->last_char == '&') {
            next_char(lexer);
            return (Token){TOK_LOGICAL_OPERATOR, L_OP_AND};
        }
    }

    if (lexer->last_char == '|') {
        next_char(lexer);
        if (lexer->last_char == '|') {
            next_char(lexer);
            return (Token){TOK_LOGICAL_OPERATOR, L_OP_OR};
        }
    }

    if (lexer->last_char == '!') {
        next_char(lexer);
        return (Token){TOK_LOGICAL_OPERATOR, L_OP_NOT};
    }

    if (lexer->last_char == ';') {
        next_char(lexer);
        return (Token){TOK_COLON};
    }

    if (lexer->last_char == ',') {
        next_char(lexer);
        return (Token){TOK_COMMA};
    }

    if (lexer->last_char == EOF) {
        return (Token){TOK_EOF};
    }

    report_error(lexer, "Unrecognised token '%c'", lexer->last_char);
    return (Token){TOK_ERROR};
}
