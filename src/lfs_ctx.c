#include "lfs_ctx.h"

static lfs_ctx_t lfs_ctx;

lfs_ctx_t * get_lfs_ctx() {
    return &lfs_ctx;
}

int lfs_ctx_init() {
    lfs_ctx.cur_meta_container = container_alloc();
    lfs_ctx.cur_data_container = container_alloc();
}
