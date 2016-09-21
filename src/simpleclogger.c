#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include <fcntl.h>
#include <string.h>
#include <time.h>
#include <stdarg.h>

#include "simpleclogger.h"

#define DEFAULT_LOG_LEVEL LOG_LEVEL_DEBUG
#define DEFAULT_LOG_LEVEL_BUILD LOG_LEVEL_INFO
#define DEFUALT_LOG_LEVEL_RUNTIME DEFAULT_LOG_LEVEL

#define DEFAULT_LOG_PATH "/tmp/lfslog"

#define DEFAULT_LOG_TIMESTAMP_BUF_SIZE 64
#define DEFAULT_LOG_TIMESTAMP_FORMAT "%Y-%m-%d %H:%M:%S.%%06u %Z"

#define DEFAULT_LOG_BUF_SIZE 256
#define DEFAULT_LOG_FORMAT "[%s] [%s] %s\n"    //[timestamp] [log level] [msg]

//set logger to defaults
static logger_config_t default_config = {
    .log_lvl_build = DEFAULT_LOG_LEVEL_BUILD,
    .log_lvl_runtime = DEFUALT_LOG_LEVEL_RUNTIME,
    .log_buf_size = DEFAULT_LOG_BUF_SIZE,
    .log_fmt = DEFAULT_LOG_FORMAT,
    .log_ts_fmt = DEFAULT_LOG_TIMESTAMP_FORMAT,
    .log_ts_buf_size = DEFAULT_LOG_TIMESTAMP_BUF_SIZE,
    .log_path = DEFAULT_LOG_PATH,
    .log_fp = NULL,
};

static logger_config_t *logger_config = &default_config;

static const char* LOGGER_LEVEL_NAMES[] = {
    "TRACE",
    "DEBUG",
    "INFO",
    "WARN",
    "ERROR",
    "CRITICAL",
    "FATAL",
};

int logger_set_lvl (int lvl) {
    if (lvl >= LOG_LEVEL_DEBUG && lvl <= LOG_LEVEL_FATAL) {
        logger_config->log_lvl_runtime = lvl;
        return lvl;
    } else {
        logger_config->log_lvl_runtime = LOG_LEVEL_DEBUG;
        return -1;
    }
}
//set LOG_RUN_LEVEL. log_level must be one of LOGGER_LEVEL_NAMES
//returns log level int. see enum in logger.h
//returns -1, if no such log level name, set to default
int logger_set_lvl_by_name(const char *log_lvl_name) {
    int i = 0;
    for (; i < LOG_LEVELS; ++i) {
        if (!strcmp(LOGGER_LEVEL_NAMES[i], log_lvl_name)) {
            logger_config->log_lvl_runtime = i;
            return i;
        }
    }
    logger_config->log_lvl_runtime = LOG_LEVEL_DEBUG;
    return -1;
}
void logger_set_lvl_trace() {
    logger_config->log_lvl_runtime = LOG_LEVEL_TRACE;
}
void logger_set_lvl_debug () {
    logger_config->log_lvl_runtime = LOG_LEVEL_DEBUG;
}
void logger_set_lvl_info() {
    logger_config->log_lvl_runtime = LOG_LEVEL_INFO;
}
void logger_set_lvl_warn() {
    logger_config->log_lvl_runtime = LOG_LEVEL_WARN;
}
void logger_set_lvl_error() {
    logger_config->log_lvl_runtime = LOG_LEVEL_ERROR;
}
void logger_set_lvl_critical() {
    logger_config->log_lvl_runtime = LOG_LEVEL_CRITICAL;
}
void logger_set_lvl_fatal() {
    logger_config->log_lvl_runtime = LOG_LEVEL_FATAL;
}

int logger_set_log_file(const char * log_file) {
    logger_config->log_path = log_file;
    logger_config->log_fp = fopen(logger_config->log_path, "w+");
    return 0;
}

void logger_real(int log_level, const char *fmt, va_list ap);
void logger_trace(const char *fmt, ...) {
    va_list ap;
    va_start (ap, fmt);
    logger_real(LOG_LEVEL_TRACE, fmt, ap);
    va_end(ap);
}
void logger_debug(const char *fmt, ...) {
    va_list ap;
    va_start (ap, fmt);
    logger_real(LOG_LEVEL_DEBUG, fmt, ap);
    va_end(ap);
}

void logger_info(const char *fmt, ...) {
    va_list ap;
    va_start (ap, fmt);
    logger_real(LOG_LEVEL_INFO, fmt, ap);
    va_end(ap);
}
void logger_warn (const char *fmt, ...) {
    va_list ap;
    va_start (ap, fmt);
    logger_real(LOG_LEVEL_WARN, fmt, ap);
    va_end(ap);
};
void logger_error(const char *fmt, ...) {
    va_list ap;
    va_start (ap, fmt);
    logger_real(LOG_LEVEL_ERROR, fmt, ap);
    va_end(ap);
}
void logger_critical(const char *fmt, ...) {
    va_list ap;
    va_start (ap, fmt);
    logger_real(LOG_LEVEL_CRITICAL, fmt, ap);
    va_end(ap);
}
void logger_fatal(const char *fmt, ...) {
    va_list ap;
    va_start (ap, fmt);
    logger_real(LOG_LEVEL_FATAL, fmt, ap);
    va_end(ap);
}

size_t get_log_ts_str(char * buf, size_t buf_size) {
    struct timeval tv;
    struct tm nowtm;

    //return current time in timeval struct
    gettimeofday(&tv, NULL);
    time_t nowtime = tv.tv_sec;
    //convert timt_t to broken-down format struct tm. this is thread-safe.
    //Note: difference between localtime() and localtime_r.
    localtime_r(&nowtime, &nowtm);

    char fmt[buf_size];
    size_t length = strftime(fmt, buf_size, logger_config->log_ts_fmt, &nowtm);
    snprintf (buf, buf_size, fmt, tv.tv_usec);
    return strlen (buf);
}


const char * logger_get_lvl_name(int log_level) {
    if (log_level >= LOG_LEVEL_TRACE && log_level <= LOG_LEVEL_FATAL) {
        return LOGGER_LEVEL_NAMES[log_level];
    } else {
        //set to debug if invalid.DEBUG
        return LOGGER_LEVEL_NAMES[LOG_LEVEL_DEBUG];
    }
}

void logger_set_default_config(logger_config_t *config) {
    memcpy((void*)config, (void*)&default_config, sizeof(default_config));
}

void logger_configure(logger_config_t *config) {
    logger_config = config;
}

void logger_print_config () {
    printf("Logfile path      : %s\n", logger_config->log_path);
    printf("Runtime log level : %s\n",
                logger_get_lvl_name(logger_config->log_lvl_runtime));
    printf("Build log level   : %s\n",
                logger_get_lvl_name(logger_config->log_lvl_build));
}

void logger(int log_level, const char *fmt, ...) {
    va_list ap;
    va_start (ap, fmt);
    logger_real(log_level, fmt, ap);
    va_end(ap);
}

void logger_real(int log_level, const char *fmt, va_list ap) {
    if (log_level >= logger_config->log_lvl_build &&
                log_level >= logger_config->log_lvl_runtime) {
        //get timestamp of logger.
        int ts_buf_size = logger_config->log_ts_buf_size;
        char log_time_buf [ts_buf_size];
        get_log_ts_str(log_time_buf, ts_buf_size);
        //get name of log level.
        const char * level = logger_get_lvl_name(log_level);

        int buf_size = logger_config->log_buf_size;
        char log_buf[buf_size];
        snprintf(log_buf, buf_size, logger_config->log_fmt, log_time_buf, level, fmt);
        if (!logger_config->log_fp) {
            vfprintf(stdout, log_buf, ap);
        } else {
            vfprintf(logger_config->log_fp, log_buf, ap);
        }
    }
}
