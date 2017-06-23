#ifndef LFS_ADDR_H
#define LFS_ADDR_H

#include <stdint.h>

struct lfs_addr {
    uint32_t cid;               /*container id*/
    uint32_t blk_off;           /*block offset in container*/
    uint32_t byte_off;          /*byte offset in block*/
}

typedef struct lfs_addr addr_t;

addr_t addr_abs_to_lfs(lfs_stat_t *stat, uint32_t abs_addr);

int addr_lfs_to_abs(lfs_stat_t *stat, addr_t addr);

#endif
