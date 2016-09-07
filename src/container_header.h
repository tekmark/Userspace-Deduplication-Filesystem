#ifndef CONTAINER_HEADER_H
#define CONTAINER_HEADER_H

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "logger.h"
#include "container_types.h"
#include "constants.h"

#define CONTAINER_TYPE_DATA 0
#define CONTAINER_TYPE_META_DATA 1

//TODO: update format
/*
 container format:
 ---------header----------------------------------------------
           byte_0->| id | type | data_blk_offset | seg_tbl_size | seg_tbl_offset
                   | ...
   seg_tbl_offset->| seg_tbl
                   | ...
 ---------data------------------------------------------------
      data_offset->| container_data
                   | ...
 -------------------------------------------------------------
 c_id           : id of container.
 c_type         : container type (DATA or METADATA).
 c_data_offset  : container data offset.
 fp_tbl_offset  : fingerprint table offset. (METADATA container has no fp table).

 ---------------
 segment table format:
   seg_tbl_offset->| seg#0 | fp0 | blk_offset | seg_size |
                   | seg#1 | fp1 | blk_offset | seg_size |
                   | seg#2 | fp2 | blk_offset | seg_size |
                   | ...
*/

#define CONTAINER_HEADER_ID_OFFSET 0
#define CONTAINER_HEADER_TYPE_OFFSET sizeof(uint32_t)
#define CONTAINER_HEADER_DATA_BLK_OFFSET (CONTAINER_HEADER_TYPE_OFFSET + sizeof(uint32_t))
#define CONTAINER_HEADER_SEG_TBL_SIZE (CONTAINER_HEADER_DATA_BLK_OFFSET + sizeof(uint32_t))
#define CONTAINER_HEADER_SEG_TBL_OFFSET (CONTAINER_HEADER_SEG_TBL_SIZE + sizeof(uint32_t))

#define CONTAINER_HEADER_DEFAULT_SEG_TBL_OFFSET (CONTAINER_HEADER_SEG_TBL_OFFSET + sizeof(uint32_t))

#define SEG_TBL_ENTRY_SEG_NO_OFFSET 0
#define SEG_TBL_ENTRY_FP_OFFSET (SEG_TBL_ENTRY_SEG_NO_OFFSET + sizeof(uint32_t))
#define SEG_TBL_ENTRY_BLK_OFFSET_OFFSET (SEG_TBL_ENTRY_FP_OFFSET + FINGERPRINT_LEN)
#define SEG_TBL_ENTRY_SEG_SIZE_OFFSET (SEG_TBL_ENTRY_BLK_OFFSET_OFFSET + sizeof(uint32_t))
#define SEG_TBL_ENTRY_LEN (SEG_TBL_ENTRY_SEG_SIZE_OFFSET + sizeof(uint32_t))

/*
struct container_header {
    uint32_t c_id;              //container_id; unique identifier of container
    uint32_t c_type;            //type of container. DATA or METADATA.
    uint32_t c_offset;          //first block offset (in bytes)
    uint32_t fp_tbl_offset;     //fingerprint table location. METADATA has no such table.
};
*/
struct seg_tbl {
    uint32_t *offset;
    uint32_t *size;              // # of records in table NOTE: Not in bytes.
    // seg_tbl_r_t *tbl;            // pointer to table.
};
typedef struct seg_tbl seg_tbl_t;

struct container_header {
    uint32_t *id;
    uint32_t *type;
    uint32_t *data_blk_offset;
    seg_tbl_t *seg_tbl;
};
typedef struct container_header container_header_t;

struct seg_tbl_r {
    uint32_t seg_no;
    fp_t fp;
    uint32_t blk_offset;         // segment block offset in conatainer
    uint32_t seg_size;           // segment size (byte)
};
typedef struct seg_tbl_r seg_tbl_r_t;

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

int c_header_get_seg_info(container_header_t *header, int seg_no, seg_tbl_r_t *r);
int c_header_add_seg_info(container_header_t *header, seg_tbl_r_t *r);

int c_header_find_seg_by_fp(container_header_t *header, fp_t *fp, seg_tbl_r_t* r);
int c_header_find_seg_by_id(container_header_t *header, uint32_t seg_no,
                                seg_tbl_r_t *r);
int container_header_to_bytes (char *buf, container_header_t *header);

container_header_t * new_container_header(char *buf);

#endif
