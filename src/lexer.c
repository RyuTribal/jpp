#include "lexer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "log.h"

TokenData get_next_token(char **input, uint64_t line_number)
{
    TokenData token;
    token.type = TOKEN_UNKNOWN;
    token.lexeme[0] = '\0';
    token.line = line_number;

    while (**input == ' ' || **input == '\t' || **input == '\n')
    {
        (*input)++;
    }

    if (**input == '\0')
    {
        token.type = TOKEN_EOF;
        return token;
    }
    if (isalpha(**input))
    {
        int i = 0;
        while (isalnum(**input))
        {
            token.lexeme[i++] = **input;
            (*input)++;
        }
        token.lexeme[i] = '\0';
        if (strcmp(token.lexeme, "return") == 0)
        {
            token.type = TOKEN_RETURN;
            log_message(LOG_LEVEL_TRACE, "Recognized 'return' keyword");
        }
        else if (strcmp(token.lexeme, "uint8") == 0)
        {
            token.type = TOKEN_TYPE;
            log_message(LOG_LEVEL_TRACE, "Recognized 'uint8' type");
        }
        else
        {
            token.type = TOKEN_IDENTIFIER;
            log_message(LOG_LEVEL_TRACE, "Recognized identifier: %s", token.lexeme);
        }
        return token;
    }
    if (isdigit(**input))
    {
        int i = 0;
        while (isdigit(**input))
        {
            token.lexeme[i++] = **input;
            (*input)++;
        }
        token.lexeme[i] = '\0';
        token.type = TOKEN_NUMBER_LITERAL;
        log_message(LOG_LEVEL_TRACE, "Recognized number literal: %s", token.lexeme);
        return token;
    }
    switch (**input)
    {
    case '(':
        token.type = TOKEN_LPAREN;
        token.lexeme[0] = '(';
        token.lexeme[1] = '\0';
        (*input)++;
        break;
    case ')':
        token.type = TOKEN_RPAREN;
        token.lexeme[0] = ')';
        token.lexeme[1] = '\0';
        (*input)++;
        break;
    case '{':
        token.type = TOKEN_LBRACE;
        token.lexeme[0] = '{';
        token.lexeme[1] = '\0';
        (*input)++;
        break;
    case '}':
        token.type = TOKEN_RBRACE;
        token.lexeme[0] = '}';
        token.lexeme[1] = '\0';
        (*input)++;
        break;
    case ';':
        token.type = TOKEN_SEMICOLON;
        token.lexeme[0] = ';';
        token.lexeme[1] = '\0';
        (*input)++;
        break;
    case '-':
        if ((*input)[1] == '>')
        {
            token.type = TOKEN_ARROW;
            token.lexeme[0] = '-';
            token.lexeme[1] = '>';
            token.lexeme[2] = '\0';
            (*input) += 2;
        }
        break;
    default:
        token.type = TOKEN_UNKNOWN;
        int i = 0;
        token.lexeme[i++] = **input;
        token.lexeme[i] = '\0';
        (*input)++;
        break;
    }

    log_message(LOG_LEVEL_TRACE, "Returning token: %s (type: %d)", token.lexeme, token.type);
    return token;
}

void tokenize_line(char *line, uint64_t line_number)
{
    char *input = line;

    while (*input != '\0')
    {
        TokenData token_data = get_next_token(&input, line_number);

        if (token_data.type == TOKEN_UNKNOWN)
        {
            log_message(LOG_LEVEL_ERROR, "Unknown token on line %llu: %s", line_number, token_data.lexeme);
        }
        else if (token_data.type != TOKEN_EOF)
        {
            log_message(LOG_LEVEL_TRACE, "Token on line %llu: %s (type %d)", line_number, token_data.lexeme, token_data.type);
        }
    }
}

int Lexer_build_from_file(char *file)
{
    FILE *file_ptr;

    char ch;

    file_ptr = fopen(file, "r");
    char line_buffer[2048];
    if (NULL == file_ptr)
    {
        log_message(LOG_LEVEL_ERROR, "Could not open file %s", file);
        return EXIT_FAILURE;
    }

    uint64_t line = 0;

    while (fgets(line_buffer, sizeof(line_buffer), file_ptr) != NULL)
    {
        line += 1;
        log_message(LOG_LEVEL_TRACE, "Line %llu: %s", line, line_buffer);

        tokenize_line(line_buffer, line);
    }
    fclose(file_ptr);
    return EXIT_SUCCESS;
}