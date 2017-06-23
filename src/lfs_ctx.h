#ifndef LFS_CTX_H
#define LFS_CTX_H

#include "container.h"

struct lfs_ctx {
    container_t *cur_meta_container;
    container_t *cur_data_container;
    int cur_ino;                   //current inode id;
};

typedef struct lfs_ctx lfs_ctx_t;

lfs_ctx_t * get_lfs_ctx();
// int lfs_ctx_init();

#endif
