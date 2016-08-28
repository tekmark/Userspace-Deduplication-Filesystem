#ifndef LFS_STAT_H
#define LFS_STAT_H

#include <stdio.h>
//disk
//             container_start_blk
//                      |
// | sys_reserved_block | conatainer 0 | container 1 | ...

struct lfs_stat {
    int fd;

    int size;                             //filesystem size. in bytes.

    int blk_size;                         // Disk I/O block size, in bytes.
    int container_0_blk_off;              // container_0 offset,  in blocks.
    int containers;                       // # of containers

    int blks_per_container;               // # of blocks per container

    int container_size;                   // equals # of blocks per container times blk_size, in bytes.

    int sys_reserved_blks;                // # of system reserved blocks.

    int cur_cid;                          // container id to write. only modify by container_write().

    //TODO: *namespace
};

typedef struct lfs_stat lfs_stat_t;

//status of filesystem, which is global.
// extern lfs_stat_t stat;
// lfs_stat_t *g_stat;

lfs_stat_t * get_lfs_stat();
void print_lfs_stat();

#endif
