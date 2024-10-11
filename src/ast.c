#include "ast.h"
#include "lexer.h"
#include "log.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

ASTNode *parse_function();
ASTNode *parse_return_statement();
ASTNode *parse_literal();

ASTNode *ast_build_from_file(char *file)
{
    FILE *file_ptr;
    file_ptr = fopen(file, "r");
    if (file_ptr == NULL)
    {
        log_message(LOG_LEVEL_ERROR, "Could not open file %s", file);
        return NULL;
    }

    char line_buffer[2048];
    char function_buffer[8192];
    uint64_t line = 0;
    ASTNode *ast = NULL;

    function_buffer[0] = '\0';

    while (fgets(line_buffer, sizeof(line_buffer), file_ptr) != NULL)
    {
        line += 1;
        log_message(LOG_LEVEL_TRACE, "Line %llu: %s", line, line_buffer);

        if (line_buffer[0] == '\0' || line_buffer[0] == '\n')
        {
            log_message(LOG_LEVEL_TRACE, "Skipping empty line %llu", line);
            continue;
        }

        strcat(function_buffer, line_buffer);

        if (strchr(line_buffer, '}') != NULL)
        {
            log_message(LOG_LEVEL_TRACE, "End of function found at line %llu", line);

            char *input = function_buffer;
            ast = parse_function(&input, line);
            if (ast == NULL)
            {
                log_message(LOG_LEVEL_ERROR, "Failed to parse function at line %llu", line);
                fclose(file_ptr);
                return NULL;
            }

            function_buffer[0] = '\0';
        }
    }

    fclose(file_ptr);
    return ast;
}

ASTNode *parse_function(char **input, uint64_t line_number)
{
    log_message(LOG_LEVEL_TRACE, "Parsing function at line %llu: %s", line_number, *input);

    TokenData token = get_next_token(input, line_number);
    log_message(LOG_LEVEL_TRACE, "First token (function name): %s (type: %d)", token.lexeme, token.type);

    if (token.type != TOKEN_IDENTIFIER)
    {
        log_message(LOG_LEVEL_ERROR, "Error: Expected function name at line %llu", line_number);
        return NULL;
    }

    FunctionASTNode *func = (FunctionASTNode *)malloc(sizeof(FunctionASTNode));
    func->base.type = AST_FUNCTION;
    func->name = strdup(token.lexeme);

    token = get_next_token(input, line_number);
    log_message(LOG_LEVEL_TRACE, "Next token (expect '('): %s (type: %d)", token.lexeme, token.type);
    if (token.type != TOKEN_LPAREN)
    {
        log_message(LOG_LEVEL_ERROR, "Error: Expected '(' at line %llu", line_number);
        return NULL;
    }

    token = get_next_token(input, line_number);
    log_message(LOG_LEVEL_TRACE, "Next token (expect ')'): %s (type: %d)", token.lexeme, token.type);
    if (token.type != TOKEN_RPAREN)
    {
        log_message(LOG_LEVEL_ERROR, "Error: Expected ')' at line %llu", line_number);
        return NULL;
    }

    token = get_next_token(input, line_number);
    log_message(LOG_LEVEL_TRACE, "Next token (expect '->'): %s (type: %d)", token.lexeme, token.type);
    if (token.type != TOKEN_ARROW)
    {
        log_message(LOG_LEVEL_ERROR, "Error: Expected '->' at line %llu", line_number);
        return NULL;
    }

    token = get_next_token(input, line_number);
    log_message(LOG_LEVEL_TRACE, "Next token (expect return type): %s (type: %d)", token.lexeme, token.type);
    if (token.type != TOKEN_TYPE)
    {
        log_message(LOG_LEVEL_ERROR, "Error: Expected return type at line %llu", line_number);
        return NULL;
    }
    func->return_type = strdup(token.lexeme);

    token = get_next_token(input, line_number);
    log_message(LOG_LEVEL_TRACE, "Next token (expect '{'): %s (type: %d)", token.lexeme, token.type);
    if (token.type != TOKEN_LBRACE)
    {
        log_message(LOG_LEVEL_ERROR, "Error: Expected '{' at line %llu", line_number);
        return NULL;
    }

    log_message(LOG_LEVEL_TRACE, "Parsing function body at line %llu", line_number);
    func->body = NULL;
    while (1)
    {
        token = get_next_token(input, line_number);
        log_message(LOG_LEVEL_TRACE, "Got token: %s (type: %d)", token.lexeme, token.type);

        if (token.type == TOKEN_RBRACE)
        {
            log_message(LOG_LEVEL_TRACE, "Function body parsed, closing brace '}' found at line %llu", line_number);
            break;
        }
        else if (token.type == TOKEN_RETURN)
        {
            log_message(LOG_LEVEL_TRACE, "Return statement found at line %llu", line_number);

            func->body = parse_return_statement(input, line_number);
            if (func->body == NULL)
            {
                log_message(LOG_LEVEL_ERROR, "Failed to parse return statement");
                return NULL;
            }
        }
        else
        {
            log_message(LOG_LEVEL_ERROR, "Error: Unexpected token '%s' in function body at line %llu", token.lexeme, line_number);
            return NULL;
        }
    }

    log_message(LOG_LEVEL_INFO, "Function successfully parsed at line %llu: %s", line_number, func->name);
    return (ASTNode *)func;
}

ASTNode *parse_return_statement(char **input, uint64_t line_number)
{
    log_message(LOG_LEVEL_TRACE, "Parsing return statement at line %llu, input: '%s'", line_number, *input);

    ReturnASTNode *ret = (ReturnASTNode *)malloc(sizeof(ReturnASTNode));
    ret->base.type = AST_RETURN;

    ret->value = parse_literal(input, line_number);
    if (ret->value == NULL)
    {
        log_message(LOG_LEVEL_ERROR, "Error: Expected a literal value after 'return'");
        return NULL;
    }

    TokenData token = get_next_token(input, line_number);
    log_message(LOG_LEVEL_TRACE, "Next token after return value: %d with lexeme '%s'", token.type, token.lexeme);

    if (token.type != TOKEN_SEMICOLON)
    {
        log_message(LOG_LEVEL_ERROR, "Error: Expected ';'");
        return NULL;
    }

    return (ASTNode *)ret;
}

ASTNode *parse_literal(char **input, uint64_t line_number)
{
    TokenData token = get_next_token(input, line_number);
    if (token.type != TOKEN_NUMBER_LITERAL)
    {
        log_message(LOG_LEVEL_ERROR, "Error: Expected a literal value");
        return NULL;
    }
    LiteralASTNode *literal = (LiteralASTNode *)malloc(sizeof(LiteralASTNode));
    literal->base.type = AST_LITERAL;
    literal->value = atoi(token.lexeme);

    return (ASTNode *)literal;
}