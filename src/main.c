#include <stdio.h>
#include <stdlib.h>
#include "cli.h"
#include "log.h"

int main(int argc, char *argv[])
{
    if (!init_logger(LOG_LEVEL_TRACE))
    {
        printf("Logger could not be initialized :(");
        return EXIT_FAILURE;
    }
    log_message(LOG_LEVEL_INFO, "Logger initialized");
    return jpp_cli_init(argc, argv);
}