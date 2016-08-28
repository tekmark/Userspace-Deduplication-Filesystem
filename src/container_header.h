#ifndef CONTAINER_HEADER_H
#define CONTAINER_HEADER_H

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define CONTAINER_TYPE_DATA 0
#define CONTAINER_TYPE_META_DATA 1

extern const uint32_t c_c_header_size;      //container header size.

typedef struct {
    uint32_t c_id;              //container_id; unique identifier of container
    uint32_t c_type;            //type of container. DATA or METADATA.
    //uint32_t c_blk_num;       //number of blocks in container.
    uint32_t c_offset;          //first block bytes. offset
    uint32_t fp_tbl_offset;     //fingerprint table location. METADATA has no such table.
} container_header_t;

/*
typedef struct {
    uint32_t c_type;
    uint32_t c_id;
} data_container_header_t;

typedef struct {

} metadata_container_header_t;
*/

//test container header operations.
void c_header_test();

//return -1 if failed to read container header.
//read header from buffer bytes.
int c_header_read (char *buf, container_header_t *header);
//write header to dist buffer.
int c_header_write(char *dist_buf, container_header_t *header);

//container operations.
uint32_t c_header_get_id(container_header_t *header);
uint32_t c_header_get_type(container_header_t *header);

void c_header_print(container_header_t *header);


#endif
