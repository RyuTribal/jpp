#include "cli.h"
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "log.h"
#include "lexer.h"
#include "ast.h"
#include "llvm.h"

int jpp_cli_init(int argc, char *args[])
{
    if (argc > 2)
    {
        log_message(LOG_LEVEL_INFO, "Compiling file: %s", args[1]);
        char *file_path;
        uint16_t last_dot_index = 0;
        uint16_t size = 0;
        for (file_path = args[1]; *file_path != '\0'; file_path++)
        {
            if (*file_path == '.')
            {
                last_dot_index = size;
            }
            size++;
        }
        if (last_dot_index + 4 == size && args[1][last_dot_index + 1] == 'j' && args[1][last_dot_index + 2] == 'p' && args[1][last_dot_index + 3] == 'p')
        {
            log_message(LOG_LEVEL_TRACE, "Valid .jpp file provided");
            ASTNode *root_node = ast_build_from_file(args[1]);
            if (root_node == NULL)
            {
                log_message(LOG_LEVEL_ERROR, "Failed to parse the file into an AST.");
                return EXIT_FAILURE;
            }
            log_message(LOG_LEVEL_INFO, "AST successfully built.");
            generate_code_from_ast(root_node, args[2]);
            return EXIT_SUCCESS;
        }
        log_message(LOG_LEVEL_ERROR, "Invalid file provided. Make sure it has the .jpp extension");
        return EXIT_FAILURE;
    }
    else
    {
        log_message(LOG_LEVEL_WARN, "Make sure you add the the jpp program you want to compile as well as a name");
        log_message(LOG_LEVEL_WARN, "Example: jpp <path_to_jpp_file> <name_of_executable>");
        return EXIT_FAILURE;
    }
}