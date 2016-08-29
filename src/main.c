#include <unistd.h>
#include <getopt.h>
#include <fcntl.h>
#include <string.h>
#include <error.h>
#include <errno.h>

#include "constants.h"
#include "lfs_stat.h"
#include "lfs_reserved.h"
#include "logger.h"
#include "util.h"

//for testing use.
#include "container_header.h"
#include "container.h"

const char *c_default_lfs_filename = "./lfsfile";         //only used in main.c
const char *c_default_lfs_mountpath = "/tmp/fuse";

//TODO: not used yet.
typedef struct {
    const char *filename;
    mode_t mode;
    uint32_t size;
} lfs_info_t;

//TODO: formatting.
void print_usage() {
    //Examples;
    printf("%s\n", "Examples: ");
    printf("%s\n", "Usage: ");
    printf("%s  %s   %s\n", "--log-level", "-L",
        "Argument Required (trace|debug|info|warn|error|critical|fatal)");
    // printf("%s %s %s\n", "--filename" )
}

void print_version() {
    printf("%s\n", "0.0.1");
}

void test() {
    c_header_test();
}

//declaration of helper functions.
uint32_t calculate_lfs_file_size(int blk_size, int container_blk_num, int container_num);

int main ( int  argc, char *argv[] ) {
    lfs_stat_t *stat = get_lfs_stat();

    lfs_summary_t summary;

    int c;
    int digit_optind = 0;
    //
    int log_level = LOG_LEVEL_DEBUG;
    //add options here.
    static struct option long_options [] = {
        {"log-level",  required_argument, 0, 'L'},
        {"log-file",   required_argument, 0, 'l'},
        {"filename",   required_argument, 0, 'F'},
        {"mount-path", required_argument, 0, 'm'},
        {"help",       no_argument,       0, 'h'},
        {"version",    no_argument,       0, 'V'},
        {0,            0,                 0, 0}
    };

    //arguments.
    const char *mount_path = c_default_lfs_mountpath;
    const char *lfs_filename = c_default_lfs_filename;

    while (1) {
        int this_option_optind = optind ? optind : 1;
        int option_index = 0;
        c = getopt_long(argc, argv, "L:l:m:F:hv", long_options, &option_index);

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
                //printf("%s\n", "No such log level." );
                logger_warn("No such log level");
                exit(EXIT_FAILURE);
            } else {
                //printf("%s%d/n", "Set LOG_RUN_LEVEL to ", ret);
            }
            break;
        case 'm':
            //parese mount_path here.
            printf ("option -L with value `%s'\n", optarg);
            mount_path = optarg;
            break;
        case 'F':
            printf ("option -F with value `%s'\n", optarg);
            lfs_filename = optarg;
            break;
        case 'h':
            print_usage();
            exit(EXIT_SUCCESS);    //success or failure.
            // break;
        case 'v':
            print_version();
            //break;
            exit(EXIT_SUCCESS);
        case ':':
            printf("%s\n", ":");
            break;
        case '?':       //invalid option,
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


    //Initialization.
    logger_debug("Filename: %s, Mount location: %s", lfs_filename, mount_path);

    //create a file of required size on disk that needs to be used to represent the
    //log structured filesystem.
    int fd;
    //check if file exists

    fd = open(lfs_filename, O_RDWR, 0777);
    if (fd < 0) {   //if not exists, create a new one.
        if (errno != ENOENT) {
            logger_error("Failed to open file. Abort. Error %s",
                    lfs_filename, strerror(errno));
            exit(EXIT_FAILURE);
        } else {
            logger_debug("File %s doesn't exist.", lfs_filename);
            logger_info("Create a new filesystem, Initializing...");
            //TODO: use MARCOS, 777 just fro test.
            //mode_t mode = 0777;
            //TODO: remove flag O_TRUNC;
            fd = open(lfs_filename, O_RDWR|O_CREAT|O_TRUNC, 0777);
            if (fd < 0) {   //if failed to open() file.
                logger_error("Failed to create/open file : %s. Abort. Error: %s",
                                lfs_filename, strerror(errno));
                exit(EXIT_FAILURE);
            }
            //set fields in summary.
            // set # of system reserved block
            summary.sys_reserved_blks = 1;
            // set I/O block size;
            summary.blk_size = c_default_blk_size;
            // set # of blocks per container.
            summary.blks_per_container = c_default_blks_per_container;
            // calculate container size in bytes;
            summary.container_size = summary.blk_size * summary.blks_per_container;
            // set # of containers
            summary.containers = c_default_containers;
            summary.filesystem_size = summary.blk_size * summary.sys_reserved_blks +
                                summary.container_size * summary.containers;
            //alloct buffer
            uint32_t lfs_filesize = summary.filesystem_size;
            char *buffer = malloc(lfs_filesize);
            memset((void*) buffer, 0, lfs_filesize);

            //copy summary to buffer.
            memcpy(buffer, &summary, sizeof(summary));

            // write buffer to disk
            ssize_t nbytes = pwrite(fd, buffer, lfs_filesize, 0);
            if (nbytes < 0) {
                logger_error("pwrite() failed. Abort()");
                exit(EXIT_FAILURE);
            }
            if (nbytes != lfs_filesize) {
                logger_error("pwrite() returns %d bytes which doesn't match file_size", nbytes);
                exit(EXIT_FAILURE);
            }

            //free allocated buffer.
            free(buffer);
        }
    } else {    //if filesystem exists
        //read system reserved blocks from disk.
        logger_info("Found file on disk.");
        //char *buf = (char *)malloc(sizeof(lfs_summary_t));
        int read_nbytes = pread(fd, &summary, sizeof(lfs_summary_t), 0);
        if (read_nbytes < 0) {
            logger_error("Failed to read");
            exit(EXIT_FAILURE);
        }
    }

    //TODO : check if error exists.
    //TODO: assign by getopt_long();

    //set stat fields.
    // set fd.
    stat->fd = fd;
    // set block size.
    stat->blk_size = summary.blk_size;
    // set # of system reserved block
    stat->sys_reserved_blks = summary.sys_reserved_blks;
    // set # of blocks per container.
    stat->blks_per_container = summary.blks_per_container;
    // set container size (in bytes).
    stat->container_size = summary.container_size;
    // set # of containers
    stat->containers = summary.containers;
    // set size of filesystem, in bytes.
    stat->size = summary.filesystem_size;

    //TODO: create a print function
    logger_info("Block size (in bytes)      : %d", stat->blk_size);
    logger_info("Container size (in bytes)  : %d", stat->container_size);
    logger_info("# of system reserved block : %d", stat->sys_reserved_blks);
    logger_info("# of blocks per container  : %d", stat->blks_per_container);
    logger_info("# of containers            : %d", stat->containers);

//    logger_info("LFS file created. location: %s, size: %d", lfs_filename, lfs_filesize);

    //test container operations.
    // print_lfs_stat();
    // test();
    // c_header_test();
    container_test();
    exit(EXIT_SUCCESS);
}

//container size (in Byte) = (I/O block size)　x (# of blocks per container);
//file size (in Byte) = (# of containers) * (container size) + reserved_blocks;
//TODO: 1. take reserved blocks into consideration.(ignored currently).
//TODO: 2. overflow check
uint32_t calculate_lfs_file_size(int blk_size, int container_blk_num, int container_num) {
    uint32_t container_size = blk_size * container_blk_num;
    uint32_t file_size = container_size * container_num;
    return file_size;
}
