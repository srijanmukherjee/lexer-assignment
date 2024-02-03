#include <errno.h>
#include <stdio.h>
#include <string.h>

#include "lexer.h"

const char *tok_to_str(TokenType tok) {
    switch (tok) {
        case TOK_IDENTIFIER:
            return "TOK_IDENTIFIER";
        case TOK_INT:
            return "TOK_INT";
        case TOK_DOUBLE:
            return "TOK_DOUBLE";
        case TOK_SCIENTIFIC:
            return "TOK_SCIENTIFIC";
        case TOK_RELOP:
            return "TOK_RELOP";
        case TOK_KEYWORD_AUTO:
            return "TOK_KEYWORD_AUTO";
        case TOK_KEYWORD_BREAK:
            return "TOK_KEYWORD_BREAK";
        case TOK_KEYWORD_CASE:
            return "TOK_KEYWORD_CASE";
        case TOK_KEYWORD_CHAR:
            return "TOK_KEYWORD_CHAR";
        case TOK_KEYWORD_CONST:
            return "TOK_KEYWORD_CONST";
        case TOK_KEYWORD_CONTINUE:
            return "TOK_KEYWORD_CONTINUE";
        case TOK_KEYWORD_DEFAULT:
            return "TOK_KEYWORD_DEFAULT";
        case TOK_KEYWORD_DO:
            return "TOK_KEYWORD_DO";
        case TOK_KEYWORD_DOUBLE:
            return "TOK_KEYWORD_DOUBLE";
        case TOK_KEYWORD_ELSE:
            return "TOK_KEYWORD_ELSE";
        case TOK_KEYWORD_ENUM:
            return "TOK_KEYWORD_ENUM";
        case TOK_KEYWORD_EXTERN:
            return "TOK_KEYWORD_EXTERN";
        case TOK_KEYWORD_FLOAT:
            return "TOK_KEYWORD_FLOAT";
        case TOK_KEYWORD_FOR:
            return "TOK_KEYWORD_FOR";
        case TOK_KEYWORD_GOTO:
            return "TOK_KEYWORD_GOTO";
        case TOK_KEYWORD_IF:
            return "TOK_KEYWORD_IF";
        case TOK_KEYWORD_INT:
            return "TOK_KEYWORD_INT";
        case TOK_KEYWORD_LONG:
            return "TOK_KEYWORD_LONG";
        case TOK_KEYWORD_REGISTER:
            return "TOK_KEYWORD_REGISTER";
        case TOK_KEYWORD_RETURN:
            return "TOK_KEYWORD_RETURN";
        case TOK_KEYWORD_SHORT:
            return "TOK_KEYWORD_SHORT";
        case TOK_KEYWORD_SIGNED:
            return "TOK_KEYWORD_SIGNED";
        case TOK_KEYWORD_SIZEOF:
            return "TOK_KEYWORD_SIZEOF";
        case TOK_KEYWORD_STATIC:
            return "TOK_KEYWORD_STATIC";
        case TOK_KEYWORD_STRUCT:
            return "TOK_KEYWORD_STRUCT";
        case TOK_KEYWORD_SWITCH:
            return "TOK_KEYWORD_SWITCH";
        case TOK_KEYWORD_TYPEDEF:
            return "TOK_KEYWORD_TYPEDEF";
        case TOK_KEYWORD_UNION:
            return "TOK_KEYWORD_UNION";
        case TOK_KEYWORD_UNSIGNED:
            return "TOK_KEYWORD_UNSIGNED";
        case TOK_KEYWORD_VOID:
            return "TOK_KEYWORD_VOID";
        case TOK_KEYWORD_VOLATILE:
            return "TOK_KEYWORD_VOLATILE";
        case TOK_KEYWORD_WHILE:
            return "TOK_KEYWORD_WHILE";
        case TOK_STRING_LITERAL:
            return "TOK_STRING_LITERAL";
        case TOK_ARITHMETIC_OPERATOR:
            return "TOK_ARITHMETIC_OPERATOR";
        case TOK_R_BRACE:
            return "TOK_R_BRACE";
        case TOK_L_BRACE:
            return "TOK_L_BRACE";
        case TOK_R_PARAN:
            return "TOK_R_PARAN";
        case TOK_L_PARAN:
            return "TOK_L_PARAN";
        case TOK_R_SQUARE_BRACKET:
            return "TOK_R_SQUARE_BRACKET";
        case TOK_L_SQUARE_BRACKET:
            return "TOK_L_SQUARE_BRACKET";
        case TOK_R_ANGLE_BRACKET:
            return "TOK_R_ANGLE_BRACKET";
        case TOK_L_ANGLE_BRACKET:
            return "TOK_L_ANGLE_BRACKET";
        case TOK_LOGICAL_OPERATOR:
            return "TOK_LOGICAL_OPERATOR";
        case TOK_EQUAL:
            return "TOK_EQUAL";
        case TOK_COLON:
            return "TOK_COLON";
        case TOK_COMMA:
            return "TOK_COMMA";
        default:
            return "<unknown>";
    }
}

const char *arithmetic_op_to_str(ArithmeticOperator op) {
    switch (op) {
        case A_OP_PLUS:
            return "A_OP_PLUS";
        case A_OP_MINUS:
            return "A_OP_MINUS";
        case A_OP_DIV:
            return "A_OP_DIV";
        case A_OP_MUL:
            return "A_OP_MUL";
        case A_OP_EXP:
            return "A_OP_EXP";
        case A_OP_MOD:
            return "A_OP_MOD";
        case A_OP_DOUBLE_PLUS:
            return "A_OP_DOUBLE_PLUS";
        case A_OP_DOUBLE_MINUS:
            return "A_OP_DOUBLE_MINUS";
        default:
            return "<unknown>";
    }
}

const char *logical_op_to_str(LogicalOperator op) {
    switch (op) {
        case L_OP_AND:
            return "L_OP_AND";
        case L_OP_OR:
            return "L_OP_OR";
        case L_OP_NOT:
            return "L_OP_NOT";
        default:
            return "<unknown>";
    }
}

const char *relop_to_str(RelOp relop) {
    switch (relop) {
        case RELOP_LT:
            return "RELOP_LT";
        case RELOP_LE:
            return "RELOP_LE";
        case RELOP_EQ:
            return "RELOP_EQ";
        case RELOP_GE:
            return "RELOP_GE";
        case RELOP_GT:
            return "RELOP_GT";
        case RELOP_NE:
            return "RELOP_NE";
        default:
            return "<unknown>";
    }
}

int main(int argc, const char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "usage: %s sourcefile\n", argv[0]);
        return 1;
    }

    Lexer *lexer = create_lexer(argv[1]);
    if (lexer == NULL) {
        fprintf(stderr, "%s: %s\n", argv[1], strerror(errno));
        return 1;
    }

    while (1) {
        Token token = get_token(lexer);
        if (token.type == TOK_EOF) {
            break;
        }

        if (token.type == TOK_ERROR) {
            fprintf(stderr, "ERROR: lexer failed\n");
            return 1;
        }

        printf("%s", tok_to_str(token.type));

        if (token.type == TOK_IDENTIFIER || token.type == TOK_STRING_LITERAL) {
            printf(": (%d) %s", token.value, st_get(lexer->st, token.value));
        }

        if (token.type == TOK_INT) {
            printf(": %d", lexer->val_int);
        }

        if (token.type == TOK_DOUBLE || token.type == TOK_SCIENTIFIC) {
            printf(": %lf", lexer->val_double);
        }

        if (token.type == TOK_RELOP) {
            printf(": %s", relop_to_str(token.value));
        }

        if (token.type == TOK_ARITHMETIC_OPERATOR) {
            printf(": %s", arithmetic_op_to_str(token.value));
        }

        printf("\n");
    }

    printf("Lexer finished\n");

    return 0;
}