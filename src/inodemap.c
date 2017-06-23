#include "inodemap.h"
#include "lfs_stat.h"
#include "logger.h"

static int imap_init_flag = 0;
static inodemap_t inodemap;

inodemap_t * get_inodemap() {
    if (imap_init_flag == 0) {
        inodemap.stat = (inodemap_stat_t*)malloc(sizeof(inodemap_stat_t));
        imap_init_flag = 1;
    }
    return &inodemap;
}

inodemap_t * new_inodemap() {
    inodemap_t *inodemap = (inodemap_t*)malloc(sizeof(inodemap_t));
    inodemap->stat = (inodemap_stat_t*)malloc(sizeof(inodemap_stat_t));
    return inodemap;
}
void delete_inodemap(inodemap_t *inodemap) {
    free(inodemap->stat);
    free(inodemap);
}

int inodemap_get_record(uint32_t ino, inodemap_r_t *r) {
    lfs_stat_t *stat = get_lfs_stat();
    assert (stat);
    //calcuate addr
    int size = stat->imap->stat->tbl_size;
    if (ino > size) {
        logger_error("inode%d doesn't exist", ino);
        return -1;
    }
    int offset = stat->imap->stat->tbl_offset + ino * INODEMAP_ENTRY_LEN;
    uint8_t *r_buf = (uint8_t*)malloc(INODEMAP_ENTRY_LEN);
    int bytes = pread(stat->fd, r_buf, INODEMAP_ENTRY_LEN, offset);
    if (bytes < 0) {
        return -1;
    }
    memcpy(&r->ino, r_buf + INODEMAP_ENTRY_INO_OFFSET, sizeof(uint32_t));
    memcpy(&r->cid, r_buf + INODEMAP_ENTRY_CID_OFFSET, sizeof(uint32_t));
    memcpy(&r->blk_offset, r_buf + INODEMAP_ENTRY_BLK_OFFSET_OFFSET, sizeof(uint32_t));
    logger_debug("Read inodemap #%d", r->ino);
    return 0;
}

int inodemap_add_record(inodemap_r_t *r) {
    lfs_stat_t *stat = get_lfs_stat();
    assert (stat);

    //record buffer to write to disk.
    uint8_t *r_buf = (uint8_t*)malloc(INODEMAP_ENTRY_LEN);
    memcpy(r_buf, &r->ino, sizeof(uint32_t));
    memcpy(r_buf + INODEMAP_ENTRY_CID_OFFSET, &r->cid, sizeof(uint32_t));
    memcpy(r_buf + INODEMAP_ENTRY_BLK_OFFSET_OFFSET, &r->blk_offset, sizeof(uint32_t));

    //calcuate addr
    int size = stat->imap->stat->tbl_size;
    int offset = stat->imap->stat->tbl_offset + size * INODEMAP_ENTRY_LEN;

    //write to disk.
    int bytes = pwrite(stat->fd, r_buf, INODEMAP_ENTRY_LEN, offset);
    if (bytes < 0) {
        return -1;
    }
    logger_debug("Add [ino=>%d, cid=>%d, blk_offset=>%d] to inodemap.(Addr:%#010X)",
        r->ino, r->cid, r->blk_offset, offset);

    //update inodemap size;
    stat->imap->stat->tbl_size += 1;
    //TODO: write to disk.
    free(r_buf);
    return 0;
}
