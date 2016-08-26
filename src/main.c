#include <getopt.h>
#include "logger.h"
#include "util.h"

void print_usage() {
    printf("%s\n", "Usage: ");
    printf("%s  %s   %s\n", "--log-level", "-L",
        "Require Argument. log level(trace|debug|info|warn|error|critical|fatal)");
}

void print_version() {
    printf("%s\n", "0.0.1");
}

int main ( int  argc, char *argv[] ) {
    // test_func();
    int c;
    int digit_optind = 0;
    //
    int log_level = LOG_LEVEL_DEBUG;
    //add options here.
    static struct option long_options [] = {
        {"log-level",  required_argument, 0, 'L'},
        {"log-file",   required_argument, 0, 'l'},
        {"mount-path", required_argument, 0, 'm'},
        {"help",       no_argument,       0, 'h'},
        {"version",    no_argument,       0, 'V'},
        {0,            0,                 0, 0}
    };

    while (1) {
        int this_option_optind = optind ? optind : 1;
        int option_index = 0;
        c = getopt_long(argc, argv, "L:l:m:hv", long_options, &option_index);

        //reach the end of options.
        if (c == -1)
            break;
        switch (c) {
        case 0:
            printf("option %s", long_options[option_index].name);
            if (optarg)
                printf(" with arg %s", optarg);
            printf("\n");
            break;
        case 'l':
            //get log file here.
            break;
        case 'L':
            //get log level here.
            //printf ("option -L with value `%s'\n", optarg);
            string_toupper(optarg);
            int ret = set_log_level_by_name(optarg);
            if (ret < 0) {
                printf("%s\n", "No such log level." );
                exit(EXIT_FAILURE);
            } else {
                //printf("%s%d\n", "Set LOG_RUN_LEVEL to ", ret);
            }
            break;
        case 'h':
            print_usage();
            break;
        case 'v':
            print_version();
            break;
        case '?':
            break;
        default:
            print_usage();
            exit(EXIT_FAILURE);
        }
    }

    if (optind < argc) {
        printf("non-option ARGV-elements: ");
        while (optind < argc)
            printf("%s ", argv[optind++]);
        printf("\n");
    }
    print_logger_config();
    exit(EXIT_SUCCESS);
}
