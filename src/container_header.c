#include "container_header.h"
#include "container_utility.h"

const uint32_t c_c_header_size = sizeof(container_header_t);

//test hearder operations.
void c_header_test() {
    logger_debug("TESTING container header ...");

    // printf("header size: %d\n", c_c_header_size);
    // container_header_t header;

    //conatiner should be allocated by conatiner alloc function.
    //char * container_buf = (char *)malloc()
    char *buffer = (char*)malloc(c_default_blk_size * c_default_blks_per_container);
    container_header_t *header = new_container_header(buffer);
    logger_debug("DEBUG");
    *header->id = 1;
    *header->type = CONTAINER_TYPE_DATA;
    *header->data_blk_offset = 1;
    *header->seg_tbl->size = 0;
    *header->seg_tbl->offset = CONTAINER_HEADER_DEFAULT_SEG_TBL_OFFSET;
    logger_debug("DEBUG@");
    seg_tbl_r_t r;
    r.seg_no = 0;
    r.blk_offset = 1;

    char * text = "I am segment. Okay?";
    fp_compute(text, strlen(text), &r.fp);

    c_header_add_seg_info(header, &r);

    char * text2 = "i am a new segment";
    fp_compute(text2, strlen(text2), &r.fp);
    r.blk_offset = 2;
    c_header_add_seg_info(header, &r);

    seg_tbl_r_t r1;
    c_header_get_seg_info(header, 0, &r1);
    char fp_hex[FINGERPRINT_READABLE_HEX_STR_LEN];
    fp_to_readable_hex(&r1.fp, fp_hex);
    logger_debug("fp: %s", fp_hex);
    container_header_print(header);
}

//init container header from allocated buffer
// void c_header_init(char *buf, container_header_t *header) {

// }
container_header_t * new_container_header(char *buf) {
    container_header_t *header = (container_header_t*)malloc(sizeof(container_header_t));
    memset(header, 0, sizeof(container_header_t));
    header->buffer = buf;
    header->id = (uint32_t*)buf;
    header->type = (uint32_t*)(buf + CONTAINER_HEADER_TYPE_OFFSET);
    header->data_blk_offset = (uint32_t*)(buf + CONTAINER_HEADER_DATA_BLK_OFFSET);

    //seg tbl;
    header->seg_tbl = (seg_tbl_t*)malloc(sizeof(seg_tbl_t));

    header->seg_tbl->size = (uint32_t*)(buf + CONTAINER_HEADER_SEG_TBL_SIZE);
    header->seg_tbl->offset = (uint32_t*)(buf + CONTAINER_HEADER_SEG_TBL_OFFSET);

    return header;
}

void free_container_header(container_header_t *header) {
    free(header->seg_tbl);
    free(header);
}

//TODO: return header size in bytes.
int c_header_read (char *buf, container_header_t *header) {
    //c_header_print(header);
    header->id = (uint32_t*) buf;
    header->type = (uint32_t*) (buf + CONTAINER_HEADER_TYPE_OFFSET);
    //int header_size;
    return 0;
}

//write to buffer
int c_header_write(char *buf, container_header_t *header) {
    //calcualte header size.
    // int header_size = calculate_header_size(header);
    int header_size = 4096;  //TODO：fix.
    char *header_buf = (char*)malloc(header_size);
    //memcpy(buf, (void*)header, c_c_header_size);
    uint32_t c_id = *header->id;
    uint32_t c_type = *header->type;

    return 0;
}

uint32_t c_header_get_id(container_header_t *header) {
    return *header->id;
}

uint32_t c_header_get_type(container_header_t *header) {
    return *header->type;
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
int calculate_container_header_size(container_header_t *header) {
    uint32_t seg_tbl_size = *header->seg_tbl->size;
    if (seg_tbl_size == 0) {
        return CONTAINER_HEADER_SEG_TBL_OFFSET + sizeof(uint32_t);
    }

    uint32_t seg_tbl_offset = *header->seg_tbl->offset;
    int bytes;                      //size of header to return.

    bytes = seg_tbl_offset + seg_tbl_size * SEG_TBL_ENTRY_LEN;

    return bytes;
}

//returns size of container header in bytes.
int container_header_to_bytes (char *buf, container_header_t *header) {
    int header_size = calculate_container_header_size(header);

}

//return r -
int c_header_get_seg_info(container_header_t *header, int seg_no, seg_tbl_r_t *r) {
    uint32_t tbl_size = *header->seg_tbl->size;
    if (seg_no > tbl_size) {
        logger_error("Invalid Segement No.(%d)", seg_no);
        return -1;
    }
    int seg_tbl_offset = *header->seg_tbl->offset;
    int entry_offset = seg_tbl_offset + seg_no * SEG_TBL_ENTRY_LEN;
    r->seg_no = seg_no;
    //copy fingerprint
    memcpy(r->fp.fingerprint, header->buffer + entry_offset + SEG_TBL_ENTRY_FP_OFFSET,
        FINGERPRINT_LEN);
    int blk_offset = *(uint32_t*)(header->buffer + entry_offset + SEG_TBL_ENTRY_BLK_OFFSET_OFFSET);
    r->blk_offset = blk_offset;
    int seg_size = *(uint32_t*)(header->buffer + entry_offset + SEG_TBL_ENTRY_SEG_SIZE_OFFSET);
    r->seg_size = seg_size;

    return seg_size;
}

//return seg_no of added segment.
//NOTE: seg_no is assigned by container_header.
int c_header_add_seg_info(container_header_t *header, seg_tbl_r_t *r){
    uint32_t seg_tbl_size = *(header->seg_tbl->size);
    uint32_t seg_tbl_offset = *(header->seg_tbl->offset);
    //calcuate offset of entry.
    int seg_offset = seg_tbl_offset + seg_tbl_size * SEG_TBL_ENTRY_LEN;
    logger_debug("seg offset->%d", seg_offset);
    assert(header->id);
    memcpy(header->buffer + seg_offset + SEG_TBL_ENTRY_SEG_NO_OFFSET, &seg_tbl_size,
            sizeof(uint32_t));
    memcpy(header->buffer + seg_offset + SEG_TBL_ENTRY_FP_OFFSET, r->fp.fingerprint,
            FINGERPRINT_LEN);
    memcpy(header->buffer + seg_offset + SEG_TBL_ENTRY_BLK_OFFSET_OFFSET, &r->blk_offset,
            sizeof(r->blk_offset));
    memcpy(header->buffer + seg_offset + SEG_TBL_ENTRY_SEG_SIZE_OFFSET, &r->seg_size,
        sizeof(r->seg_size));

    //update seg_tbl_size;
    *(header->seg_tbl->size) = seg_tbl_size + 1;

    return seg_tbl_size;
}

//return r
int c_header_find_seg_by_fp(container_header_t *header, fp_t *fp, seg_tbl_r_t *r) {
    uint32_t tbl_size = *header->seg_tbl->size;
    int seg_tbl_offset = *header->seg_tbl->offset;
    int i;
    uint8_t fp_hex[FINGERPRINT_READABLE_HEX_STR_LEN];
    for (i = 0; i < tbl_size; ++i) {
        uint32_t entry_offset = seg_tbl_offset + i * SEG_TBL_ENTRY_LEN;
        int j = 0;
        // logger_debug("entry_offset=>%d", entry_offset);
        //compare
        memcpy(r->fp.fingerprint, header->buffer + entry_offset + SEG_TBL_ENTRY_FP_OFFSET,
            FINGERPRINT_LEN);
        // fp_to_readable_hex(&r->fp, fp_hex);

        if (!fp_cmp(fp, &r->fp)) {      //if found.
            r->seg_no = *(uint32_t*)(header->buffer + entry_offset);
            r->blk_offset = *(uint32_t*)(header->buffer + entry_offset + SEG_TBL_ENTRY_BLK_OFFSET_OFFSET);
            r->seg_size = *(uint32_t*)(header->buffer + entry_offset + SEG_TBL_ENTRY_SEG_SIZE_OFFSET);
            // logger_debug("fp=>%s, seg size: %d", fp_hex, r->seg_size);
            return r->seg_no;
        }
    }
    return -1;
}
