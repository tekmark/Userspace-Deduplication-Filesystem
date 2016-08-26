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

const char * LOGGER_DEFAULT_TIME_FORMAT = "%Y-%m-%d %H:%M:%S";
// const char * LOGGER_TIME_FORMAT = LOGGER_DEFAULT_TIME_FORMAT;
// const char * LOGGER_TIME_FORMAT = "%Y-%m-%d %H:%M:%S.";
const char * LOGGER_TIME_FORMAT = "%Y-%m-%d %H:%M:%S.%%06u %Z";

void logger_helper(int log_level, const char *fmt, va_list ap);
void logger_debug(const char *fmt, ...) {
    va_list ap;
    va_start (ap, fmt);
    logger_helper(LOG_LEVEL_DEBUG, fmt, ap);
    va_end(ap);
}

void logger_info(const char *fmt, ...) {
    va_list ap;
    va_start (ap, fmt);
    logger_helper(LOG_LEVEL_INFO, fmt, ap);
    va_end(ap);
}
void logger_warn (const char *fmt, ...) {
    va_list ap;
    va_start (ap, fmt);
    logger_helper(LOG_LEVEL_WARN, fmt, ap);
    va_end(ap);
};
void logger_error(const char *fmt, ...) {
    va_list ap;
    va_start (ap, fmt);
    logger_helper(LOG_LEVEL_ERROR, fmt, ap);
    va_end(ap);
}

size_t get_log_time_string(char * buf, size_t buf_size) {
    struct timeval tv;
    struct tm nowtm;

    //return current time in timeval struct
    gettimeofday(&tv, NULL);
    time_t nowtime = tv.tv_sec;
    //convert timt_t to broken-down format struct tm. this is thread-safe.
    //Note: difference between localtime() and localtime_r.
    localtime_r(&nowtime, &nowtm);

    char fmt[buf_size];
    size_t length = strftime(fmt, buf_size, LOGGER_TIME_FORMAT, &nowtm);
    snprintf (buf, buf_size, fmt, tv.tv_usec);
    return strlen (buf);
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

void logger(int log_level, const char *fmt, ...) {
    va_list ap;
    va_start (ap, fmt);
    logger_helper(log_level, fmt, ap);
    va_end(ap);
}

void logger_helper(int log_level, const char *fmt, va_list ap) {
  if (log_level >= LOG_BUILD_LEVEL && log_level >= LOG_RUN_LEVEL) {
      //get timestamp of logger.
      char log_time_buf [64];
      get_log_time_string(log_time_buf, 64);
      //get name of log level.
      const char * level = get_log_level_name(log_level);

      char log_buf[256];
      snprintf(log_buf, 256, "[ %s ] [ %s ] MSG: %s\n", log_time_buf, level, fmt);

      vfprintf(stdout, log_buf, ap);
  }
}
