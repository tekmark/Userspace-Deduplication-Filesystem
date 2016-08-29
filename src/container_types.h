#ifndef CONTAINER_TYPES_H
#define CONTAINER_TYPES_H

#include <stdint.h>
// #include <container_header.h>

//TODO: consider const instead_of MARCOS
#define FINGERPRINT_SIZE 20

struct fingerprint {
  uint8_t fingerprint[FINGERPRINT_SIZE];
};
typedef struct fingerprint fingerprint_t;

/*
  fingerprint <-> segment_offset（in bytes) table.
                byte_0 -> | # of entries |
  sizeof(# of entries) -> | fp0 | seg_offset |
                          | fp1 | seg_offset |
                          | ...
 */
struct fp_seg_r {
    fingerprint_t fp;
    uint32_t seg_offset;         //segment offset in conatainer (in bytes).
};
typedef struct fp_seg_r fp_seg_r_t;

struct fp_seg_tbl {
    uint32_t size;          // # of records in table NOTE: Not in bytes.
    fp_seg_r_t *tbl;        // pointer to table.
};
typedef struct fp_seg_tbl fp_seg_tbl_t;


#endif
