#include "cli.h"
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "log.h"
#include "lexer.h"

int jpp_cli_init(int argc, char *args[])
{
    if (argc > 1)
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
            return Lexer_build_from_file(args[1]);
        }
        else
        {
            log_message(LOG_LEVEL_ERROR, "Invalid file provided. Make sure it has the .jpp extension");
            return EXIT_FAILURE;
        }
    }
    else
    {
        log_message(LOG_LEVEL_WARN, "Make sure you add the the jpp program you want to compile");
        return EXIT_FAILURE;
    }
}