#include "logger.h"

int LOG_RUN_LEVEL = LOG_LEVEL_INFO;

const char* LOGGER_LEVEL_NAMES[] = {
  "TRACE",
  "DEBUG",
  "INFO",
  "WARN",
  "ERROR",
  "CRITICAL",
  "FATAL",
};

void log_debug(const char *fmt) {
    logger(LOG_LEVEL_DEBUG, fmt);
}

void log_info();
void log_warn();
void log_error();

const char * get_log_time_string() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    time_t t = tv.tv_sec;
    return ctime(&t);
}

const char * get_log_level_name(int log_level) {
    if (log_level >= LOG_LEVEL_TRACE && log_level <= LOG_LEVEL_FATAL) {
        return LOGGER_LEVEL_NAMES[log_level];
    } else {
        //set to debug if invalid.DEBUG
        return LOGGER_LEVEL_NAMES[LOG_LEVEL_DEBUG];
    }
}

void set_log_level_debug () {
    LOG_RUN_LEVEL = LOG_LEVEL_DEBUG;
}
void set_log_level_info() {
    LOG_RUN_LEVEL = LOG_LEVEL_ERROR;
}
void set_log_level_warn() {
    LOG_RUN_LEVEL = LOG_LEVEL_WARN;
}
void set_log_level_error() {
    LOG_RUN_LEVEL = LOG_LEVEL_ERROR;
}

void print_logger_config () {
    printf("%s %s\n", "log run level", get_log_level_name(LOG_RUN_LEVEL));
    printf("%s %s\n", "log build level", get_log_level_name(LOG_BUILD_LEVEL));
}

void logger(int log_level, const char *msg) {
    if (log_level >= LOG_BUILD_LEVEL && log_level >= LOG_RUN_LEVEL) {
      printf("[ %s ] %s | %s \n", get_log_level_name(log_level), get_log_time_string(), msg);
    }
}
