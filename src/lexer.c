#include "lexer.h"

#include <ctype.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/cdefs.h>

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

    lexer->col = 0;
    lexer->row = 1;
    lexer->prev_col = lexer->row;
    lexer->prev_col = lexer->col;
    lexer->is_error = false;
    lexer->filepath = filepath;
    lexer->source = file;
    lexer->last_char = ' ';
    lexer->st = st_create();
    return lexer;
}

static bool isdelim(uint8_t c) { return isspace(c) || c == '\t' || c == '\r' || c == '\n'; }

static void next_char(Lexer *lexer) {
    lexer->prev_row = lexer->row;
    lexer->prev_col = lexer->col;

    lexer->col++;
    lexer->last_char = fgetc(lexer->source);
    if (lexer->last_char == '\n') {
        lexer->row++;
        lexer->col = 0;
    }
}

// Can go back only once
static void prev_char(Lexer *lexer) {
    ungetc(lexer->last_char, lexer->source);
    lexer->col = lexer->prev_col;
    lexer->row = lexer->prev_row;
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
        prev_char(lexer);
        lexer->val_int = strtol(str->buf, NULL, 10);
        free_string(str);
        return (Token){TOK_INT};
    }

    // e after int part
    if (tolower(lexer->last_char) == 'e') goto scientific;

    // read digits after dot(.)
    do {
        string_append_char(str, lexer->last_char);
        next_char(lexer);
    } while (isdigit(lexer->last_char));

    // only dot(.)
    if (str->n - 1 == 0) {
        free_string(str);
        return (Token){TOK_ERROR};
    }

    prev_char(lexer);
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
    prev_char(lexer);

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
    if (lexer == NULL || lexer->is_error) {
        return (Token){TOK_ERROR};
    }

    next_char(lexer);

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
        prev_char(lexer);

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
        bool parse_number = true;
        if (lexer->last_char == '.') {
            // TODO: fgetc shouldn't be called directly
            char c = fgetc(lexer->source);
            // this means this is just a dot
            if (!isdigit(c)) {
                parse_number = false;
            }
            ungetc(c, lexer->source);
        }
        if (parse_number) {
            Token result = process_number(lexer);
            if (result.type == TOK_ERROR) {
                lexer->is_error = true;
            }
            return result;
        }
    }

    // String literal
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
            lexer->is_error = true;
            return (Token){TOK_ERROR};
        }

        const char *s = string_c_str(str);
        free_string(str);

        return (Token){TOK_STRING_LITERAL, st_insert(lexer->st, s)};
    }

    switch (lexer->last_char) {
        case '>': {
            next_char(lexer);
            if (lexer->last_char == '=') {
                return (Token){TOK_RELOP, RELOP_GE};
            }
            prev_char(lexer);
            return (Token){TOK_RELOP, RELOP_GT};
        }
        case '<': {
            next_char(lexer);
            if (lexer->last_char == '=') {
                return (Token){TOK_RELOP, RELOP_LE};
            } else if (lexer->last_char == '>') {
                return (Token){TOK_RELOP, RELOP_NE};
            }
            prev_char(lexer);
            return (Token){TOK_RELOP, RELOP_LT};
        }
        case '=': {
            next_char(lexer);
            if (lexer->last_char == '=') {
                return (Token){TOK_RELOP, RELOP_EQ};
            }
            prev_char(lexer);
            return (Token){TOK_EQUAL};
        }
        case '+': {
            next_char(lexer);
            if (lexer->last_char == '+') {
                return (Token){TOK_ARITHMETIC_OPERATOR, A_OP_DOUBLE_PLUS};
            }
            prev_char(lexer);
            return (Token){TOK_ARITHMETIC_OPERATOR, A_OP_PLUS};
        }
        case '-': {
            next_char(lexer);
            if (lexer->last_char == '-') {
                return (Token){TOK_ARITHMETIC_OPERATOR, A_OP_DOUBLE_MINUS};
            }
            prev_char(lexer);
            return (Token){TOK_ARITHMETIC_OPERATOR, A_OP_MINUS};
        }
        case '/': {
            next_char(lexer);
            // comment
            if (lexer->last_char == '/') {
                do {
                    next_char(lexer);
                } while (lexer->last_char != '\n' && lexer->last_char != EOF);
                return get_token(lexer);
            }
            prev_char(lexer);
            return (Token){TOK_ARITHMETIC_OPERATOR, A_OP_DIV};
        }
        case '*': {
            next_char(lexer);
            if (lexer->last_char == '*') {
                return (Token){TOK_ARITHMETIC_OPERATOR, A_OP_EXP};
            }
            prev_char(lexer);
            return (Token){TOK_ARITHMETIC_OPERATOR, A_OP_MUL};
        }
        case '%':
            return (Token){TOK_ARITHMETIC_OPERATOR, A_OP_MOD};
        case '(':
            return (Token){TOK_L_PARAN};
        case ')':
            return (Token){TOK_R_PARAN};
        case '[':
            return (Token){TOK_L_SQUARE_BRACKET};
        case ']':
            return (Token){TOK_R_SQUARE_BRACKET};
        case '{':
            return (Token){TOK_L_BRACE};
        case '}':
            return (Token){TOK_R_BRACE};
        case '&': {
            next_char(lexer);
            if (lexer->last_char == '&') {
                return (Token){TOK_LOGICAL_OPERATOR, L_OP_AND};
            }
            prev_char(lexer);
            break;
        }
        case '|': {
            next_char(lexer);
            if (lexer->last_char == '|') {
                return (Token){TOK_LOGICAL_OPERATOR, L_OP_OR};
            }
            prev_char(lexer);
            break;
        }
        case '!':
            return (Token){TOK_LOGICAL_OPERATOR, L_OP_NOT};
        case ';':
            return (Token){TOK_SEMI_COLON};
        case ':':
            return (Token){TOK_COLON};
        case ',':
            return (Token){TOK_COMMA};
        case '.':
            return (Token){TOK_DOT};
        case EOF:
            return (Token){TOK_EOF};
    }

    lexer->is_error = true;
    report_error(lexer, "Unrecognised token '%c'", lexer->last_char);
    return (Token){TOK_ERROR};
}
