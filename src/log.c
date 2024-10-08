#include "log.h"

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <time.h>

#ifdef _WIN32
#include <windows.h>
#endif

static LoggerConfig log_config = {LOG_LEVEL_TRACE};

void get_timestamp(char *buffer, size_t size){
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    if (t == NULL)
    {
        // Fallback in case localtime fails
        snprintf(buffer, size, "Unknown Time");
        return;
    }
    strftime(buffer, size, "%Y-%m-%d %H:%M:%S", t);
}

uint8_t init_logger(LogLevel level){
#ifdef _WIN32
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut == INVALID_HANDLE_VALUE)
        return 0;

    DWORD dwMode = 0;
    if (!GetConsoleMode(hOut, &dwMode))
        return 0;

    dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    SetConsoleMode(hOut, dwMode);
#endif
    log_config.level = level;
    return 1;
}

void set_logger_level(LogLevel level){
    log_config.level = level;
}

void log_message(LogLevel level, const char *format, ...){
    if (level < log_config.level)
        return;

    char timestamp[20];
    get_timestamp(timestamp, sizeof(timestamp));

    const char *color;
    const char *level_str;

    switch (level)
    {
    case LOG_LEVEL_TRACE:
        color = BLUE;
        level_str = "DEBUG";
        break;
    case LOG_LEVEL_INFO:
        color = GREEN;
        level_str = "INFO";
        break;
    case LOG_LEVEL_WARN:
        color = YELLOW;
        level_str = "WARNING";
        break;
    case LOG_LEVEL_ERROR:
        color = RED;
        level_str = "ERROR";
        break;
    default:
        color = RESET;
        level_str = "UNKNOWN";
    }
    printf("[%s] ", timestamp);
    printf("%s[%s] ", color, level_str);
    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);

    // Reset color
    printf(RESET "\n");
}