#ifndef CONTAINER_TYPES_H
#define CONTAINER_TYPES_H

#include <stdint.h>

#include "fingerprint.h"
// #include <container_header.h>

//TODO: consider const instead_of MARCOS. remove.. use fp_t instead.
#define FINGERPRINT_SIZE 20
struct fingerprint {
  uint8_t fingerprint[FINGERPRINT_SIZE];
};
typedef struct fingerprint fingerprint_t;

/*
  fingerprint <-> segment_offset（in bytes) table.
                byte_0 -> | # of entries |
  sizeof(# of entries) -> | seg#0 | fp0 | blk_offset |
                          | seg#1 | fp1 | blk_offset |
                          | seg#2 | fp2 | blk_offset |
                          | ...
 */
 /*
#define SEG_TBL_SIZE_OFFSET 0
#define SEG_TBL_DATA_OFFSET (SEG_TBL_SIZE_OFFSET + sizeof(uint32_t))

#define SEG_TBL_ENTRY_LEN (FINGERPRINT_LEN + sizeof(uint32_t) * 2)
#define SEG_TBL_ENTRY_SEG_NUM_OFFSET 0
#define SEG_TBL_ENTRY_FP_OFFSET (sizeof(uint32_t))
#define SEG_TBL_ENTRY_BLK_OFFSET_OFFSET (SEG_TBL_FP_OFFSET + FINGERPRINT_LEN)

struct seg_tbl_r{
    uint32_t seg_no;
    fp_t fp;
    uint32_t blk_offset;         //segment offset in conatainer (in bytes).
};
typedef struct seg_tbl_r seg_tbl_r_t;

struct seg_tbl {
    uint32_t *offset;
    uint32_t *size;              // # of records in table NOTE: Not in bytes.
    // seg_tbl_r_t *tbl;            // pointer to table.
};
typedef struct seg_tbl seg_tbl_t;
*/
#endif
