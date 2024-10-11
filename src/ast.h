#pragma once
#include <stdint.h>

typedef enum
{
    AST_FUNCTION,
    AST_RETURN,
    AST_LITERAL
} ASTNodeType;

typedef struct ASTNode
{
    ASTNodeType type;
} ASTNode;

typedef struct
{
    ASTNode base;
    char *name;
    char *return_type;
    ASTNode *body;
} FunctionASTNode;

typedef struct
{
    ASTNode base;
    ASTNode *value;
} ReturnASTNode;

typedef struct
{
    ASTNode base;
    int value;
} LiteralASTNode;

ASTNode *parse_function(char **input, uint64_t line_number);

ASTNode *ast_build_from_file(char *file);