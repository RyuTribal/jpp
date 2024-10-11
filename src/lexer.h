#pragma once
#include <stdint.h>
#define LEXEME_MAX_SIZE 256

typedef enum Token
{
    TOKEN_KEYWORD,
    TOKEN_IDENTIFIER,
    TOKEN_NUMBER_LITERAL,
    TOKEN_ARROW,
    TOKEN_LPAREN,
    TOKEN_RPAREN,
    TOKEN_LBRACE,
    TOKEN_RBRACE,
    TOKEN_TYPE,
    TOKEN_RETURN,
    TOKEN_COMMA,
    TOKEN_SEMICOLON,
    TOKEN_EOF,
    TOKEN_UNKNOWN
} Token;

typedef struct
{
    Token type;
    char lexeme[LEXEME_MAX_SIZE];
    int line;
} TokenData;

int Lexer_build_from_file(char *file);
TokenData get_next_token(char **input, uint64_t line_number);