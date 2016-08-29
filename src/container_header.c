#include "container_header.h"

const uint32_t c_c_header_size = sizeof(container_header_t);

//test hearder operations.
void c_header_test() {
    // printf("header size: %d\n", c_c_header_size);
    // container_header_t header;

    //conatiner should be allocated by conatiner alloc function.
    //char * container_buf = (char *)malloc()

    // header.c_id = 1;
    // header.c_type = CONTAINER_TYPE_DATA;
    // c_header_print(&header);
    // char buffer[c_c_header_size];
    //
    // c_header_write(buffer, &header);
    //
    // container_header_t header2;
    // c_header_read(buffer, &header2);

    // c_header_print(&header2);
}

//init container header from allocated buffer
// void c_header_init(char *buf, container_header_t *header) {

// }
container_header_t * new_container_header(char *buf) {
    container_header_t *header = (container_header_t*)malloc(sizeof(container_header_t));
    memset(header, 0, sizeof(container_header_t));
    header->id = (uint32_t*)buf;
    header->type = (uint32_t*)(buf + CONTAINER_HEADER_TYPE_OFFSET);
    header->data_offset = (uint32_t*)(buf + CONTAINER_HEADER_DATA_OFFSET);
    header->fp_tbl_offset = (uint32_t*)(buf + CONTAINER_HEADER_FP_TBL_OFFSET);
    return header;
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
int calculate_fp_seg_tbl_size(fp_seg_tbl_t *tbl) {
    int entry_size = sizeof(uint32_t) + FINGERPRINT_SIZE;
    int num_of_entries = tbl->size;
    int bytes = sizeof(uint32_t) + num_of_entries * entry_size;
    return bytes;
}

//NOTE: update when header format changes.
//calculate container header in bytes.
// header size = fp_tbl_offset + size of (fp table);
int calculate_container_header_size(container_header_t *header) {
    uint32_t fp_tbl_offset = *header->fp_tbl_offset;
    int bytes;      //size of header to return.
    if (fp_tbl_offset == 0) {
        return CONTAINER_HEADER_FP_TBL_OFFSET + sizeof(uint32_t);
    }

    uint32_t fp_seg_tbl_size = calculate_fp_seg_tbl_size(header->fp_seg_tbl);
    bytes = fp_tbl_offset + fp_seg_tbl_size;

    return bytes;
}

//returns size of container header in bytes.
int container_header_to_bytes (char *buf, container_header_t *header) {
    int header_size = calculate_container_header_size(header);

}
