#include "container_header.h"
#include "logger.h"

// const uint32_t c_c_header_size = sizeof(container_header_t);

//init container header from allocated buffer
int c_header_init(uint8_t *buf, c_header_t *header) {
// c_header_t * new_container_header(char *buf) {
    // c_header_t *header = (c_header_t*)malloc(sizeof(c_header_t));
    //memset(header, 0, sizeof(c_header_t));
    //seg header pointers to buffer.
    header->buf = buf;
    header->c_id = (uint32_t*)buf;
    header->c_type = (uint32_t*)(buf + C_HEADER_TYPE_OFF);
    header->data_blk_offset = (uint32_t*)(buf + C_HEADER_DATA_BLK_OFF);
    header->seg_tbl_size = (uint32_t*)(buf + C_HEADER_SEG_TBL_SIZE_OFF);
    header->seg_tbl_offset = (uint32_t*)(buf + C_HEADER_SEG_TBL_OFFSET_OFF);

    return 0;
}

int c_header_set_data_defaults(c_header_t *header) {
    *header->c_type = CONTAINER_TYPE_DATA;
    *header->seg_tbl_size = 0;
    *header->seg_tbl_offset = C_HEADER_DEFAULT_SEG_TBL_OFFSET;
}

int c_header_set_metadata_defaults(c_header_t *header) {
    *header->c_type = CONTAINER_TYPE_METADATA;
    *header->seg_tbl_size = 0;
    *header->seg_tbl_offset = 0;
}

// void free_container_header(container_header_t *header) {
    // free(header->seg_tbl);
    // free(header);
// }

//TODO: return header size in bytes.
int c_header_buf_read (uint8_t *buf,size_t buf_size, c_header_t *header) {
    c_header_init(buf, header);
    return 0;
}

//write to buffer
int c_header_buf_write(uint8_t *buf, size_t buf_size, c_header_t *header) {
    //calcualte header size.
    // int header_size = calculate_header_size(header);
    int header_size = 4096;  //TODO：fix.
    memcpy(buf, header->buf, header_size);
    return header_size;
}


uint32_t c_header_get_id(container_header_t *header) {
    return *header->c_id;
}

uint32_t c_header_get_type(container_header_t *header) {
    return *header->c_type;
}

//NOTO: update when fp_seg_tbl format changes.
// calculate fingerprint - seg_offset table size (in bytes);
// struct fp_seg_tbl,
// return size of fp_seg_tbl in bytes.
int calculate_seg_tbl_size(seg_tbl_t *tbl) {
    // int entry_size = sizeof(uint32_t) + FINGERPRINT_SIZE;
    // int num_of_entries = tbl->size;

    // int bytes = sizeof(uint32_t) + num_of_entries * entry_size;
    // return bytes;
    return 0;
}

//NOTE: update when header format changes.
//calculate container header in bytes.
// header size = fp_tbl_offset + size of (fp table);
/*
int calculate_container_header_size(c_header_t *header) {
    uint32_t seg_tbl_size = *header->seg_tbl_size;
    if (seg_tbl_size == 0) {
        return CONTAINER_HEADER_SEG_TBL_OFFSET + sizeof(uint32_t);
    }

    uint32_t seg_tbl_offset = *header->seg_tbl->offset;
    int bytes;                      //size of header to return.

    bytes = seg_tbl_offset + seg_tbl_size * SEG_TBL_ENTRY_LEN;

    return bytes;
}
*/

//returns size of container header in bytes.
//int container_header_to_bytes (char *buf, container_header_t *header) {
//    int header_size = calculate_container_header_size(header);
//
// }

//get segment table entry by positon.
//return positon.
int c_header_get_seg_ent(c_header_t *header, int pos, seg_tbl_ent_t *ent) {
    uint32_t tbl_size = *header->seg_tbl_size;
    if (pos > tbl_size) {
        // logger_error("Invalid Segement No.(%d)", seg_no);
        return -1;
    }
    int seg_tbl_offset = *header->seg_tbl_offset;
    int ent_off = seg_tbl_offset + pos * SEG_TBL_ENT_LEN;
    ent->seg_no = pos;
    //copy fingerprint
    memcpy(ent->seg_fp.fingerprint, header->buf + ent_off + SEG_TBL_ENT_FP_OFF,
                FINGERPRINT_LEN);
    ent->seg_blk_off = *(uint32_t*)(header->buf + ent_off + SEG_TBL_ENT_BLK_OFFSET_OFF);
    ent->seg_size = *(uint32_t*)(header->buf + ent_off + SEG_TBL_ENT_SEG_SIZE_OFF);

    return pos;
}

//return seg_no of added segment.
//NOTE: seg_no is assigned by container_header.
int c_header_add_seg_ent(container_header_t *header, seg_tbl_ent_t *ent){
    uint32_t seg_tbl_size = *(header->seg_tbl_size);
    uint32_t seg_tbl_offset = *(header->seg_tbl_offset);
    //calcuate offset of entry.
    int seg_offset = seg_tbl_offset + seg_tbl_size * SEG_TBL_ENT_LEN;

    memcpy(header->buf + seg_offset + SEG_TBL_ENT_SEG_NO_OFF, &seg_tbl_size,
            sizeof(uint32_t));
    memcpy(header->buf + seg_offset + SEG_TBL_ENT_FP_OFF, ent->seg_fp.fingerprint,
            FINGERPRINT_LEN);
    memcpy(header->buf + seg_offset + SEG_TBL_ENT_BLK_OFFSET_OFF, &ent->seg_blk_off,
            sizeof(ent->seg_blk_off));
    memcpy(header->buf + seg_offset + SEG_TBL_ENT_SEG_SIZE_OFF, &ent->seg_size,
        sizeof(ent->seg_size));

    //update seg_tbl_size;
    *(header->seg_tbl_size) = seg_tbl_size + 1;

    return seg_tbl_size;
}

//return r
int c_header_get_seg_ent_by_fp(c_header_t *header, fp_t *fp, seg_tbl_ent_t *ent) {
    uint32_t tbl_size = *header->seg_tbl_size;
    int seg_tbl_offset = *header->seg_tbl_offset;
    int i;
    // uint8_t fp_hex[FINGERPRINT_READABLE_HEX_STR_LEN];
    for (i = 0; i < tbl_size; ++i) {
        uint32_t entry_offset = seg_tbl_offset + i * SEG_TBL_ENT_LEN;
        int j = 0;
        // logger_debug("entry_offset=>%d", entry_offset);
        //compare
        memcpy(ent->seg_fp.fingerprint, header->buf + entry_offset + SEG_TBL_ENT_FP_OFF,
            FINGERPRINT_LEN);
        // fp_to_readable_hex(&r->fp, fp_hex);

        if (!fp_cmp(fp, &ent->seg_fp)) {      //if found.
            ent->seg_no = *(uint32_t*)(header->buf + entry_offset);
            ent->seg_blk_off = *(uint32_t*)(header->buf + entry_offset + SEG_TBL_ENT_BLK_OFFSET_OFF);
            ent->seg_size = *(uint32_t*)(header->buf + entry_offset + SEG_TBL_ENT_SEG_SIZE_OFF);
            // logger_debug("fp=>%s, seg size: %d", fp_hex, r->seg_size);
            return ent->seg_no;
        }
    }
    return -1;
}
