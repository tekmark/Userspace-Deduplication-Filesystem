#include "container_utility.h"

//calculate absolute bytes in file for a container id.
uint32_t calculate_container_offset (uint32_t cid, lfs_stat_t *st) {
    assert(st);
    int offset = cid * st->container_size + st->blk_size * st->sys_reserved_blks;
    return offset;
}

//get container id from buffer
int container_buf_get_id(char *buf) {
    //TODO: use header instead.
    int *cid = (uint32_t*) buf;
    return *cid;
}

//update container id in buffer
int container_buf_update_id(char *buf, uint32_t cid) {
    //container_header_t *header = (container_header_t*) buf;
    uint32_t *id = (uint32_t*)buf;
    *id = cid;
    return 0;
}

//
int container_buf_get_type(char *buf) {
    int type = *(uint32_t*)(buf + CONTAINER_HEADER_TYPE_OFFSET);
    return type;
}
int container_buf_update_type(char *buf, int type) {

}

void container_buf_print(char *buf) {
    int id = container_buf_get_id(buf);
    int type = container_buf_get_type(buf);
    logger_debug("id : %d | type: %d", id, type);
}


void container_header_print(container_header_t *header) {
    logger_debug("Container#%d (type=>%d)", *header->id, *header->type);
    logger_debug("data_blk_offset=>%d, seg_tbl_offset=>%d",
                    *header->data_blk_offset, *header->seg_tbl->offset);

    //get table size (# of entries).
    int size = *header->seg_tbl->size;
    logger_debug("-----Segment Table (size: %d)-----", size);
    int i;
    for (i = 0; i < size; ++i) {
        seg_tbl_r_t r;
        c_header_get_seg_info(header, i, &r);
        char fp_hex_str[FINGERPRINT_READABLE_HEX_STR_LEN];
        fp_to_readable_hex(&r.fp, fp_hex_str);
        logger_debug("Seg#%d:[fp=>%s, blk=>%d]", r.seg_no, fp_hex_str, r.blk_offset);
    }
}
