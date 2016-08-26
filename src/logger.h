/*
  Logger.
 */
#ifndef LOGGER_H
#define LOGGER_H

#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include <string.h>
#include <time.h>
#include <stdarg.h>

#define DEFAULT_LOG_LEVEL LOG_LEVEL_DEBUG
#define DEFAULT_LOG_BUILD_LEVEL LOG_LEVEL_INFO

//TODO: LOG_BUILD_LEVEL should not be defined here.
#define LOG_BUILD_LEVEL LOG_LEVEL_DEBUG

#ifndef LOG_BUILD_LEVEL
#define LOG_BUILD_LEVEL DEFAULT_LOG_BUILD_LEVEL
#endif

// #ifndef LOG_LEVEL
// #define LOG_LEVEL DEFAULT_LOG_LEVEL
// #endif

#define NUM_OF_LOG_LEVELS 7
enum {
    LOG_LEVEL_TRACE,     //0
    LOG_LEVEL_DEBUG,
    LOG_LEVEL_INFO,
    LOG_LEVEL_WARN,
    LOG_LEVEL_ERROR,
    LOG_LEVEL_CRITICAL,
    LOG_LEVEL_FATAL
};

//this is defined in logger.c file.
//NOTE: LOGGER_LEVEL_NAMES should be defined according to enum about and
//NUM_OF_LOG_LEVELS;
extern const char* LOGGER_LEVEL_NAMES[];

int LOG_RUN_LEVEL;

#define LOGGER_DEFAULT_FORMAT "%s"
extern const char * LOGGER_DEFAULT_TIME_FORMAT;

// #ifndef LOGGER_TIME_FORMAT
// #define LOGGER_TIME_FORMAT LOGGER_DEFAULT_TIME_FORMAT
// #endif

void set_log_level_debug ();
void set_log_level_info();
void set_log_level_warn();
void set_log_level_error();

int set_log_level_by_name (const char * log_level);

int is_log_level_valid(const char * log_level);

void logger_debug(const char *fmt, ...);
void logger_info(const char *fmt, ...);
void logger_warn(const char *fmt, ...);
void logger_error(const char *fmt, ...);

void logger(int log_level, const char *fmt, ...);

void print_logger_config();

#endif
