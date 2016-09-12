#include "lfs_reserved.h"

void sys_rsvd_set_to_defaults (lfs_summary_t *summary) {
    //set fields in summary.
    // set # of system reserved block
    summary->sys_reserved_blks = 1;
    // set I/O block size;
    summary->blk_size = c_default_blk_size;
    // set # of blocks per container.
    summary->blks_per_container = c_default_blks_per_container;
    // calculate container size in bytes;
    summary->container_size = summary->blk_size * summary->blks_per_container;
    // set # of containers
    summary->containers = c_default_containers;
    summary->filesystem_size = summary->blk_size * summary->sys_reserved_blks +
                summary->container_size * summary->containers;

    //TODO: don't hard coded here.
    summary->ns_stat_offset = 1024;
    //inode
    summary->inodemap_size = 0;
    summary->inodemap_offset = 2048;
}
