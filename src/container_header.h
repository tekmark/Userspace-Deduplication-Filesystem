#ifndef CONTAINER_HEADER_H
#define CONTAINER_HEADER_H

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "container_types.h"

#define CONTAINER_TYPE_DATA 0
#define CONTAINER_TYPE_META_DATA 1

#define CONTAINER_HEADER_ID_OFFSET 0
#define CONTAINER_HEADER_TYPE_OFFSET sizeof(uint32_t)
#define CONTAINER_HEADER_DATA_OFFSET (CONTAINER_HEADER_TYPE_OFFSET + sizeof(uint32_t))
#define CONTAINER_HEADER_FP_TBL_OFFSET (CONTAINER_HEADER_DATA_OFFSET + sizeof(uint32_t))

/*
 container format:
 ---------header----------------------------------------------
 byte_0->          | c_id | c_type | c_data_offset | fp_tbl_offset
                   | ...
 fp_tbl_offset->   |fp_tbl
                   | ...
 ---------data------------------------------------------------
 c_data_offset->   | container_data
                   | ...
 -------------------------------------------------------------
 c_id           : id of container.
 c_type         : container type (DATA or METADATA).
 c_data_offset  : container data offset.
 fp_tbl_offset  : fingerprint table offset. (METADATA container has no fp table).

 */

/*
struct container_header {
    uint32_t c_id;              //container_id; unique identifier of container
    uint32_t c_type;            //type of container. DATA or METADATA.
    uint32_t c_offset;          //first block offset (in bytes)
    uint32_t fp_tbl_offset;     //fingerprint table location. METADATA has no such table.
};
*/

struct container_header {
    uint32_t *id;
    uint32_t *type;
    uint32_t *data_offset;
    uint32_t *fp_tbl_offset;
    fp_seg_tbl_t *fp_seg_tbl;
};
typedef struct container_header container_header_t;

//test container header operations.
void c_header_test();

//return -1 if failed to read container header.
//read header from buffer bytes.
int c_header_read (char *buf, container_header_t *header);
//write header to dist buffer.
int c_header_write(char *buf, container_header_t *header);

//container operations.
uint32_t c_header_get_id(container_header_t *header);
uint32_t c_header_get_type(container_header_t *header);

int container_header_to_bytes (char *buf, container_header_t *header);

container_header_t * new_container_header(char *buf);

#endif
