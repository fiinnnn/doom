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

#define LOG_TRACE(...) log_write(LOGLEVEL_TRACE, __VA_ARGS__)
#define LOG_DEBUG(...) log_write(LOGLEVEL_DEBUG, __VA_ARGS__)
#define LOG_INFO(...) log_write(LOGLEVEL_INFO, __VA_ARGS__)
#define LOG_WARN(...) log_write(LOGLEVEL_WARN, __VA_ARGS__)
#define LOG_ERROR(...) log_write(LOGLEVEL_ERROR, __VA_ARGS__)
#define LOG_FATAL(...) log_write(LOGLEVEL_FATAL, __VA_ARGS__)

void log_init();

void log_write(int level, const char* fmt, ...);

void log_set_level(int level);
void log_set_quiet(int quiet);

#endif // LOG_H
