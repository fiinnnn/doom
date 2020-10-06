#include "log.h"

#include <stdio.h>
#include <stdarg.h>
#include <time.h>

struct log_state {
    int level;
    int quiet;
};

static struct log_state state;

static const char* level_names[] = {
    "TRACE",
    "DEBUG",
    "INFO",
    "WARN",
    "ERROR",
    "FATAL"
};

static const char* level_colors[] = {
    "\033[37m", "\033[34m", "\033[32m", "\033[33m", "\033[31m", "\033[97;41m"
};

void log_init(int level)
{
    state.level = LOGLEVEL_TRACE;
    state.quiet = 0;
}

void log_write(int level, const char* file, int line, const char* fmt, ...)
{
    if (level < state.level || state.quiet)
        return;

    time_t t = time(NULL);
    struct tm* lt = localtime(&t);

    va_list args;
    char time[16];

    strftime(time, sizeof(time), "%H:%M:%S", lt);

    if (level <= LOGLEVEL_DEBUG)
        fprintf(stderr, "\033[90m[%s] %s%-5s \033[90m%s:%d\033[0m ",
            time, level_colors[level], level_names[level], file, line);
    else
        fprintf(stderr, "\033[90m[%s] %s%-5s\033[0m ",
            time, level_colors[level], level_names[level]);

    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);
    fprintf(stderr, "\n");
}

void log_set_level(int level)
{
    state.level = level;
}

void log_set_quiet(int quiet)
{
    state.quiet = quiet ? 1 : 0;
}
