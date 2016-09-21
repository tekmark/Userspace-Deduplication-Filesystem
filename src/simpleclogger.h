/*
  Logger.
 */
#ifndef SIMPLE_C_LOGGER_H_
#define SIMPLE_C_LOGGER_H_
#include <stdio.h>

//NOTE: log names
#define LOG_LEVELS 7
#define LOG_LEVEL_COUNT LOG_LEVELS
enum {
    LOG_LEVEL_TRACE = 0,     //0
    LOG_LEVEL_DEBUG,
    LOG_LEVEL_INFO,
    LOG_LEVEL_WARN,
    LOG_LEVEL_ERROR,
    LOG_LEVEL_CRITICAL,
    LOG_LEVEL_FATAL
};

struct logger_config {
    int log_lvl_build;
    int log_lvl_runtime;
    int log_buf_size;
    FILE *log_fp;
    const char * log_path;
    const char * log_fmt;
    const char * log_ts_fmt;
    int log_ts_buf_size;
};
typedef struct logger_config logger_config_t;

int logger_set_lvl(int log_lvl);
int logger_set_lvl_by_name(const char *log_lvl_name);
void logger_set_lvl_debug ();
void logger_set_lvl_info();
void logger_set_lvl_warn();
void logger_set_lvl_error();
void logger_set_lvl_critical();
void logger_set_lvl_fatal();

int logger_set_log_file(const char * log_file);

//logging interfaces.
void logger_trace(const char *fmt, ...);
void logger_debug(const char *fmt, ...);
void logger_info(const char *fmt, ...);
void logger_warn(const char *fmt, ...);
void logger_error(const char *fmt, ...);
void logger_critical(const char *fmt, ...);
void logger_fatal(const char *fmt, ...);

void logger(int log_level, const char *fmt, ...);

void logger_configure(logger_config_t *config);
void logger_print_config();
#endif
