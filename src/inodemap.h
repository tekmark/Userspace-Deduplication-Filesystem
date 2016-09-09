#ifndef INODEMAP_H
#define INODEMAP_H

#include <stdint.h>
#include <stdlib.h>

#define INODEMAP_ENTRY_LEN 12
#define INODEMAP_ENTRY_INO_OFFSET 0
#define INODEMAP_ENTRY_CID_OFFSET (INODEMAP_ENTRY_INO_OFFSET + sizeof(uint32_t))
#define INODEMAP_ENTRY_BLK_OFFSET_OFFSET (INODEMAP_ENTRY_CID_OFFSET + sizeof(uint32_t))

/*
    -----inodemap(table)---------
         | inode_id | container_id | blk_offset|
 */
//inode map entry format on disk.
struct inodemap_r {
    uint32_t ino;
    uint32_t cid;
    uint32_t blk_offset;
    // uint32_t addr;
};
typedef struct inodemap_r inodemap_r_t;

struct inodemap_stat {
    uint32_t tbl_offset;
    uint32_t tbl_size;
};
typedef struct inodemap_stat inodemap_stat_t;
struct inodemap {
    inodemap_stat_t *stat;
    uint8_t *tbl_buf;
};
typedef struct inodemap inodemap_t;

inodemap_t * new_inodemap();
void delete_inodemap(inodemap_t *);

int inodemap_get_record(uint32_t ino, inodemap_r_t *r);
int inodemap_add_record(inodemap_r_t *r);

#endif
