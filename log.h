/*
 * logging system
 */

#ifndef LOG_H
#define LOG_H

#include <stdarg.h>

enum {
    LOGLEVEL_TRACE,
    LOGLEVEL_DEBUG,
    LOGLEVEL_INFO,
    LOGLEVEL_WARN,
    LOGLEVEL_ERROR,
    LOGLEVEL_FATAL
};

#define LOG_TRACE(...) log_write(LOGLEVEL_TRACE, __FILE__, __LINE__, __VA_ARGS__)
#define LOG_DEBUG(...) log_write(LOGLEVEL_DEBUG, __FILE__, __LINE__, __VA_ARGS__)
#define LOG_INFO(...)  log_write(LOGLEVEL_INFO,  __FILE__, __LINE__, __VA_ARGS__)
#define LOG_WARN(...)  log_write(LOGLEVEL_WARN,  __FILE__, __LINE__, __VA_ARGS__)
#define LOG_ERROR(...) log_write(LOGLEVEL_ERROR, __FILE__, __LINE__, __VA_ARGS__)
#define LOG_FATAL(...) log_write(LOGLEVEL_FATAL, __FILE__, __LINE__, __VA_ARGS__)

void log_init();

void log_write(int level, const char* file, int line, const char* fmt, ...);

void log_set_level(int level);
void log_set_quiet(int quiet);

#endif // LOG_H
