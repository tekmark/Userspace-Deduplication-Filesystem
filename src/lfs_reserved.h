#ifndef LFS_RESERVED_H
#define LFS_RESERVED_H

#include <stdint.h>

//this is used when mount an existing file
//this struct is stored at (blk_0, byte_0).
//use this to generate lfs_stat.
struct lfs_summary {
    uint32_t filesystem_size;            // disk size in bytes.
    uint32_t blk_size;                   // I/O block size
    uint32_t container_size;             // conatiner size in bytes.
    uint32_t blks_per_container;         // # of blocks per container
    uint32_t containers;                 // # of containers
    uint32_t sys_reserved_blks;          // # of reversed blocks
    uint32_t container_0_blk_offset;     // first container offset in blocks.

    uint32_t ns_stat_offset;             // namespace_offset in bytes.
};
typedef struct lfs_summary lfs_summary_t;

#endif
