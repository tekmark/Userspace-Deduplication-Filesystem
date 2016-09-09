#include "lfs_stat.h"

// lfs_stat_t sys_stat;
static lfs_stat_t stat;

lfs_stat_t * get_lfs_stat() {
    return &stat;
}

void lfs_stat_print() {
    logger_info("-------------------------------------------");
    logger_info("File discriptor (fd)       : %d", stat.fd);
    logger_info("Filesystem size (in bytes) : %d", stat.size);
    logger_info("Block size (in bytes)      : %d", stat.blk_size);
    logger_info("Container size (in bytes)  : %d", stat.container_size);
    logger_info("# of system reserved block : %d", stat.sys_reserved_blks);
    logger_info("# of blocks per container  : %d", stat.blks_per_container);
    logger_info("# of containers            : %d", stat.containers);
    logger_info("namespace status offset    : %d", stat.ns_stat_offset);

    logger_info("inodemap offset            : %d", stat.imap->stat->tbl_offset);
    logger_info("# of inodemap entries      : %d", stat.imap->stat->tbl_size);
    logger_info("-------------------------------------------");
}
