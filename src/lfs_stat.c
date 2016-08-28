#include "lfs_stat.h"

// lfs_stat_t sys_stat;
lfs_stat_t stat;

lfs_stat_t * get_lfs_stat() {
    return &stat;
}

void print_lfs_stat() {
    printf("File discriptor (fd)       : %d\n", stat.fd);
    printf("filesystem size (in bytes) : %d\n", stat.size);
    printf("block size (in bytes)      : %d\n", stat.blk_size);
    printf("container size (in bytes)  : %d\n", stat.container_size);
    printf("# of system reserved block : %d\n", stat.sys_reserved_blks);
    printf("# of blocks per container  : %d\n", stat.blks_per_container);
    printf("# of containers            : %d\n", stat.containers);
}
