#ifndef NAMESPACE_H
#define NAMESPACE_H
//fingerprint of segment <-> container_id;
//by searching namespace, find the file data container.
//by searching container header, find blk_offset in container.

#include <stdio.h>
#include <string.h>

#include "uthash.h" //hash table

void namespace_test();
// struct seg_location {
    // uint32_t c_id;      //conatiner id;
    // uint32_t seg_id;    //seg_id in container
// }
/*
  namespace (fingerprint <-> contaienr_id table)
  format on disk.
      byte_0  ->  | size of table |
     record_0 ->  | fingerprint | live | container_id |
     record_1 ->  | fingerprint | live | container_id |
                  | ...
  ----------------------------------------------------------------
*/
//namespace record. see above.
struct namespace_r {
    uint8_t fp[20];
    uint32_t container_id;
    uint32_t live;
};
typedef struct namespace_r namespace_r_t;

struct  namespace_hash_tbl_r {
    uint8_t fp[20];             /* key */
    //uint32_t record_offset      /* record byte offset. => to find/update
    namespace_r_t *pr;          /* pointer to record*/
    UT_hash_handle hh;          /* makes this structure hashable */
};
typedef struct namespace_hash_tbl_r namespace_hash_tbl_r_t;

//TODO: define more fields.
struct namespace {
    uint32_t *size;
    namespace_r_t *records;       // points to namespace (fp - cid table)
};

typedef struct namespace namespace_t;

#endif
