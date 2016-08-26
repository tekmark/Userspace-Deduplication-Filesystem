#include "logger.h"

int main ( int  argc, char *argv[] ) {
  set_log_level_debug ();
  print_logger_config();
  logger(LOG_LEVEL_DEBUG, "%s, %s" , "hello", "wolrd");
  // logger(LOG_LEVEL_INFO, "INFO");
  logger(LOG_LEVEL_WARN, "WARN");
  // logger(LOG_LEVEL_ERROR, "ERROR");
  // logger(LOG_LEVEL_CRITICAL, "CRITICAL");
  logger_debug("%s, %s, %s", "test", "log", "debug");
  // lfs_init();
  // return fuse_main( argc, argv, &lfs_oper, NULL);
}
