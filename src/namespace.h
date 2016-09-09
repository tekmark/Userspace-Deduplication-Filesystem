#ifndef NAMESPACE_H
#define NAMESPACE_H
//fingerprint of segment <-> container_id;
//by searching namespace, find the file data container.
//by searching container header, find blk_offset in container.

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <fcntl.h>
#include <unistd.h>

#include "uthash.h" //hash table
#include "util.h"
#include "logger.h"


/*
  namespace (fingerprint <-> contaienr_id table)
  format on disk.
      byte_0  ->  | size of table | ...
     record_0 ->  | fingerprint | live | container_id |
     record_1 ->  | fingerprint | live | container_id |
                  | ...
  ----------------------------------------------------------------
*/

//MARCOs are problematic.
#define NAMESPACE_STAT_LEN (sizeof(uint32_t) * 2)
#define NAMESPACE_RECORD_LEN (FINGERPRINT_LEN + sizeof(uint32_t) * 2)

// extern const int c_namespace_record_len;
// extern const int c_namespace_stat_len;

void namespace_test();

//TODO: merge ns_r_t and ns_ht_r_t.
//namespace record struct
struct ns_r {
    fp_t fp;                                            //fingerprint
    uint32_t c_id;                                      //container id
    uint32_t c_stat;                                    //conatiner status
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

int ns_stat_read_disk (ns_stat_t *ns_stat);
int ns_stat_write_disk (ns_stat_t *ns_stat);

//int ns_update_record();
// get
// int ns_get_record();
// add / update to hashtable and write to disk.
int ns_add_record(ns_r_t *t);

// int ns_process_tbl_buffer(uint8_t *buffer, uint32_t record_count);
// int ns_write_record_to_buffer(uint8_t *buf, uint32_t record_num, ns_r_t *r);
// int ns_get_record_from_buffer(uint8_t *buffer, uint32_t record_num, ns_r_t *r);

//hashtable operations.
int ns_hashtable_build();

int ns_hashtable_add(ns_ht_r_t **hashtable, ns_ht_r_t *r);
ns_ht_r_t * ns_hashtable_find(ns_ht_r_t **hashtable, fp_t *fp);
void ns_hashtable_delete_all(ns_ht_r_t **hashtable);
int ns_hashtable_count(ns_ht_r_t **hashtable);

#endif
