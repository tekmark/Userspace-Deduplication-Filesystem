#include "lfs_ctx.h"

static lfs_ctx_t lfs_ctx;
static int initialized = 0;     //not initialized;

lfs_ctx_t * get_lfs_ctx() {
    if (initialized == 0) {
        lfs_ctx.cur_meta_container = container_alloc();
        lfs_ctx.cur_data_container = container_alloc();
        initialized = 1;
    }
    return &lfs_ctx;
}

int lfs_ctx_init() {
    // lfs_ctx.cur_meta_container = container_alloc();
    // lfs_ctx.cur_data_container = container_alloc();
}
