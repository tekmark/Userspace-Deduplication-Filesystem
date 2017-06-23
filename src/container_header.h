#ifndef CONTAINER_HEADER_H
#define CONTAINER_HEADER_H

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "container_types.h"
#include "constants.h"

#define CONTAINER_TYPE_DATA 0
#define CONTAINER_TYPE_METADATA 1

/*
 * container format:
 * ---------header----------------------------------------------
 *           byte_0->| c_id | c_type | data_blk_offset | seg_tbl_size | seg_tbl_offset
 *                   | ...
 *   seg_tbl_offset->| seg_tbl
 *                   | ...
 *---------data------------------------------------------------
 *      data_offset->| container_data
 *                   | ...
 * -------------------------------------------------------------
 * c_id              : id of container.
 * c_type            : container type (DATA or METADATA).
 * c_data_blk_offset : container data offset.
 * seg_tbl_offset    : fingerprint table offset. (METADATA container has no fp table).

 * --------------------------------------------------------------
 * segment table format:
 *  seg_tbl_offset->| seg#0 | fp0 | blk_offset | seg_size |
 *                  | seg#1 | fp1 | blk_offset | seg_size |
 *                  | seg#2 | fp2 | blk_offset | seg_size |
 *                  | ...
 */

//TODO: comments
// #define CONTAINER_HEADER_ID_OFFSET 0
// #define CONTAINER_HEADER_TYPE_OFFSET sizeof(uint32_t)
// #define CONTAINER_HEADER_DATA_BLK_OFFSET (CONTAINER_HEADER_TYPE_OFFSET + sizeof(uint32_t))
// #define CONTAINER_HEADER_SEG_TBL_SIZE (CONTAINER_HEADER_DATA_BLK_OFFSET + sizeof(uint32_t))
// #define CONTAINER_HEADER_SEG_TBL_OFFSET (CONTAINER_HEADER_SEG_TBL_SIZE + sizeof(uint32_t))

//C is short for container.
#define C_HEADER_ID_OFF 0
#define C_HEADER_TYPE_OFF (C_HEADER_ID_OFF + sizeof(uint32_t))
#define C_HEADER_DATA_BLK_OFF (C_HEADER_TYPE_OFF + sizeof(uint32_t))
#define C_HEADER_SEG_TBL_SIZE_OFF (C_HEADER_DATA_BLK_OFF + sizeof(uint32_t))
#define C_HEADER_SEG_TBL_OFFSET_OFF (C_HEADER_SEG_TBL_SIZE_OFF + sizeof(uint32_t))

#define SEG_TBL_ENT_SEG_NO_OFF 0
#define SEG_TBL_ENT_FP_OFF (SEG_TBL_ENT_SEG_NO_OFF + sizeof(uint32_t))
#define SEG_TBL_ENT_BLK_OFFSET_OFF (SEG_TBL_ENT_FP_OFF + FINGERPRINT_LEN)
#define SEG_TBL_ENT_SEG_SIZE_OFF (SEG_TBL_ENT_BLK_OFFSET_OFF + sizeof(uint32_t))

#define SEG_TBL_ENT_LEN (SEG_TBL_ENT_SEG_SIZE_OFF + sizeof(uint32_t))

#define C_HEADER_DEFAULT_SEG_TBL_OFFSET (C_HEADER_SEG_TBL_OFFSET_OFF + sizeof(uint32_t))

struct seg_tbl_ent {
    uint32_t seg_no;
    fp_t seg_fp;
    uint32_t seg_blk_off;          // segment block offset in conatainer
    uint32_t seg_size;             // segment size (byte)
};

struct container_header {
    uint8_t *buf;                  //points to buffer, normally equals to container->buffer
    uint32_t *c_id;                //container_id; unique identifier of container
    uint32_t *c_type;              //type of container. DATA or METADATA.
    uint32_t *data_blk_offset;     //first block offset (in bytes)
    uint32_t *seg_tbl_offset;      // 0 if METADATA(not in use).
    uint32_t *seg_tbl_size;        // 0 if METADATA(not in use).
    // struct seg_tbl_ent_t *seg_tbl;
};

typedef struct container_header c_header_t;
typedef c_header_t container_header_t;
typedef struct seg_tbl seg_tbl_t;
typedef struct seg_tbl_ent seg_tbl_ent_t;
// typedef struct seg_tbl_r_t seg_tbl_ent_t;

// container_header_t * new_container_header(char *buf);
// c_header_t * new_container_header(uint8_t * buf);

int c_header_init(uint8_t *buf, c_header_t *header);

//set defaults.
int c_header_set_data_defaults(c_header_t *header);
int c_header_set_metadata_defaults(c_header_t *header);

//return -1 if failed to read container header.
//read header from buffer bytes.
int c_header_buf_read (uint8_t *buf, size_t buf_size, c_header_t *header);
//write header to dist buffer.
int c_header_buf_write(uint8_t *buf, size_t buf_size, c_header_t *header);

//container operations.
uint32_t c_header_get_id(c_header_t *header);
uint32_t c_header_get_type(c_header_t *header);

uint32_t c_header_seg_tbl_size(c_header_t *header);

int c_header_get_seg_ent(c_header_t *header, int pos, seg_tbl_ent_t *ent);
int c_header_get_seg_ent_by_fp(c_header_t *header, fp_t *fp, seg_tbl_ent_t* ent);
//add entry to the end of seg table.
int c_header_add_seg_ent(c_header_t *header, seg_tbl_ent_t *ent);

//currently not in use. TODO: implement.
int c_header_del_seg_ent(c_header_t *header, int pos);
int c_header_del_seg_ent_by_fp(c_header_t *header, fp_t *fp);
// int container_header_to_bytes (char *buf, container_header_t *header);

#endif
