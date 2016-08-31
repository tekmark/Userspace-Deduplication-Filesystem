#ifndef NAMESPACE_H
#define NAMESPACE_H
//fingerprint of segment <-> container_id;
//by searching namespace, find the file data container.
//by searching container header, find blk_offset in container.

#include <stdio.h>
#include <string.h>

#include "uthash.h" //hash table
#include "util.h"
#include "logger.h"

void namespace_test();
void namespace_test2();
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
// struct namespace_r {
    // uint8_t fp[20];
    // uint32_t container_id;
    // uint32_t live;
// };
// typedef struct namespace_r namespace_r_t;


// struct  namespace_hash_tbl_r {
    // uint8_t fp[20];             /* key */
    //uint32_t record_offset      /* record byte offset. => to find/update
    // namespace_r_t *pr;          /* pointer to record*/
    // UT_hash_handle hh;          /* makes this structure hashable */
// };
// typedef struct namespace_hash_tbl_r namespace_hash_tbl_r_t;

//TODO: define more fields.
// struct namespace {
    // uint32_t *size;
    // namespace_r_t *records;       // points to namespace (fp - cid table)
// };
// typedef struct namespace namespace_t;

//namespace record struct
struct ns_r {
    fp_t fp;              //pointer points to fingerprint;
    uint32_t c_id;                 //container id
    uint32_t c_stat;               //conatiner status.
};
typedef struct ns_r ns_r_t;

//used in uthash method.
struct ns_ht_r {
    fp_t fp;                 // Key
    uint32_t rec_num;
    UT_hash_handle hh;
};

typedef struct ns_ht_r ns_ht_r_t;

struct ns_stat {
    uint32_t size;
    uint32_t tbl_offset;
};
typedef struct ns_stat ns_stat_t;

struct ns {
    ns_stat_t *ns_stat;
    ns_ht_r_t *hashtable;
    uint8_t *buffer;
};
typedef struct ns ns_t;

int calculate_ns_size();

ns_t *new_namespace();
void delete_namespace(ns_t *namespace);

//int ns_update_record();
// get
int ns_get_record();
// add / update
int ns_add_record();

int ns_process_tbl_buffer(uint8_t *buffer, uint32_t record_count);

int ns_write_record_to_buffer(uint8_t *buf, uint32_t record_num, ns_r_t *r);
int ns_get_record_from_buffer(uint8_t *buffer, uint32_t record_num, ns_r_t *r);

int ns_hashtable_add(ns_ht_r_t **hashtable, ns_ht_r_t *r);
ns_ht_r_t * ns_hashtable_find(ns_ht_r_t **hashtable, fingerprint_t *fp);

void ns_hashtable_delete_all(ns_ht_r_t **hashtable);
int ns_hashtable_count(ns_ht_r_t **hashtable);

#endif
