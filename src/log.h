#pragma once
#include <stdint.h>

#define RESET "\x1B[0m"
#define RED "\x1B[31m"
#define GREEN "\x1B[32m"
#define YELLOW "\x1B[33m"
#define BLUE "\x1B[37m"

typedef enum
{
    LOG_LEVEL_TRACE,
    LOG_LEVEL_INFO,
    LOG_LEVEL_WARN,
    LOG_LEVEL_ERROR
} LogLevel;

typedef struct
{
    LogLevel level;
} LoggerConfig;

uint8_t init_logger(LogLevel level);

void set_logger_level(LogLevel level);

void log_message(LogLevel level, const char *format, ...);