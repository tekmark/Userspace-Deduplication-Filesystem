#include "namespace.h"
#include "lfs_stat.h"
#include "uthash.h" //hash table
#include "util.h"
#include "logger.h"

void namespace_test() {
/*
    printf("%s\n", "namespace test cases : ");
    //alloc buffer for testing.
    char *buffer = (char*)malloc(sizeof(char) * 4096);
    memset(buffer, 0, sizeof(char) * 4096);

    namespace_t namespace;
    namespace.size = (uint32_t*)buffer;
    namespace.records = (namespace_r_t*) (buffer + sizeof(uint32_t));

    namespace_r_t r0, r1, r2, r3;
    //record 0
    //strcpy(r0.fp, "fingerprint_0");
    uint8_t *text = "hello, world!";
    compute_fingerprint(text, strlen(text), r0.fp);
    r0.container_id = 0;
    r0.live = 1;
    //add to table.
    //update size;
    int size = *namespace.size;
    int offset = size * sizeof(namespace_r_t);
    memcpy(namespace.records + offset, &r0, sizeof(r0));
    *namespace.size = 1;
    //record 1
    strcpy(r1.fp, "fingerprint_1");
    r1.container_id = 1;
    r1.live = 1;

    size = *namespace.size;
    offset = size * sizeof(namespace_r_t);
    memcpy(namespace.records + offset, &r1, sizeof(r1));
    *namespace.size = 2;

    //read buffer. use r3;
    int id = 1;
    offset = id * sizeof(r3);
    memcpy(&r3, namespace.records + offset, sizeof(r3));

    printf("Fingerprint: %s, container id: %d\n", r3.fp, r3.container_id);

    //test hashtable
    namespace_hash_tbl_r_t *namespace_hash_tbl = NULL;
    namespace_hash_tbl_r_t *rec = (namespace_hash_tbl_r_t*)malloc(
                                            sizeof(namespace_hash_tbl_r_t));
    //set key
    memcpy(rec->fp, r3.fp, 20);
    rec->pr = &r1;
    //add to hashtable.
    HASH_ADD_STR(namespace_hash_tbl, fp, rec);

    namespace_hash_tbl_r_t *result = (namespace_hash_tbl_r_t*)malloc(
                                            sizeof(namespace_hash_tbl_r_t));

    HASH_FIND_STR(namespace_hash_tbl, r3.fp, result);
    if (result) printf("%s id is %d\n", r3.fp, result->pr->container_id);
    */
}

ns_t * new_namespace() {
    ns_t *ns = (ns_t*)malloc(sizeof(ns_t));
    ns->ns_stat = (ns_stat_t*)malloc(sizeof(ns_stat_t));
    ns->hashtable = NULL;
    //ns->buffer = NULL;
    return ns;
}

void delete_namespace(ns_t *ns) {
    //free namespace stat
    free(ns->ns_stat);
    //free hashtable;
    ns_hashtable_delete_all(&ns->hashtable);

    free(ns);
}

//fill ns_stat
int ns_stat_read_disk (ns_stat_t *ns_stat) {
    lfs_stat_t *stat = get_lfs_stat();
    assert(stat);
    //get addr on disk.
    int fd = stat->fd;
    int offset = stat->ns_stat_offset;
    //TODO: convert ns_stat_t to DEFINE
    uint8_t * ns_stat_buf = (uint8_t*)malloc(NAMESPACE_STAT_LEN);
    int bytes = pread(fd, ns_stat_buf, NAMESPACE_STAT_LEN, offset);
    if (bytes < 0) {
        return -1;
    }
    //assign fields
    ns_stat->size = *(uint32_t*)ns_stat_buf;
    ns_stat->tbl_offset = *(uint32_t*)(ns_stat_buf + sizeof(uint32_t));
    logger_debug("Read ns_stat -> |offset: %d|size : %d|tbl_offset : %d",
                    stat->ns_stat_offset, ns_stat->size, ns_stat->tbl_offset);
    return 0;
}

//wirte update ns_stat to disk.
int ns_stat_write_disk (ns_stat_t *ns_stat) {
    lfs_stat_t *stat = get_lfs_stat();
    assert(stat);

    //get addr on disk.
    int fd = stat->fd;
    int offset = stat->ns_stat_offset;

    uint8_t * ns_stat_buf = (uint8_t*)malloc(NAMESPACE_STAT_LEN);
    //assign
    memcpy(ns_stat_buf, &ns_stat->size, sizeof(uint32_t));
    memcpy(ns_stat_buf + sizeof(uint32_t), &ns_stat->tbl_offset, sizeof(uint32_t));

    int bytes = pwrite(fd, ns_stat_buf, NAMESPACE_STAT_LEN, offset);
    if (bytes < 0) {
        return -1;
    }
    logger_debug("Write ns_stat -> |size : %d|tbl_offset : %d",
                    ns_stat->size, ns_stat->tbl_offset);
    return 0;
}

//add recort to the end of table
int ns_add_record(ns_r_t *r) {
    lfs_stat_t *stat = get_lfs_stat();
    assert(stat);
    //get addr on disk.
    int fd = stat->fd;
    int size = stat->ns->ns_stat->size;

    //calculate addr
    //TODO :
    // int offset = stat->ns->ns_stat->tbl_offset;
    int offset = stat->ns_stat_offset + sizeof(ns_stat_t);

    ns_ht_r_t *ht_r = (ns_ht_r_t*)malloc(sizeof(ns_ht_r_t));
    fp_cpy(&ht_r->fp, &r->fp);
    ht_r->rec_num = size;

    ns_hashtable_add(&stat->ns->hashtable, ht_r);
    //write to disk.
    offset += size * (20 + sizeof(uint32_t) + sizeof(uint32_t));

    uint8_t *r_buf = (uint8_t*)malloc(NAMESPACE_RECORD_LEN);
    memcpy(r_buf, &r->fp, FINGERPRINT_LEN);
    //TODO: fill remaiming field.
    int bytes = pwrite (fd, r_buf, NAMESPACE_RECORD_LEN, offset);
    if (bytes < 0) {
        return -1;
    }
    logger_debug("Write record. offset: %d", offset);

    //update size. (size + 1)
    ++stat->ns->ns_stat->size;
    ns_stat_write_disk(stat->ns->ns_stat);

    //int count = ns_hashtable_count(&stat->ns->hashtable);
    //logger_debug("Hashtable count: %d", count);
    return 0;
}

//use this after lfs_stat->ns.
int ns_hashtable_build() {
    lfs_stat_t *stat = get_lfs_stat();
    assert(stat || stat->ns);
    ns_t *ns = stat->ns;
    //# of record (tbl_size);
    int size = ns->ns_stat->size;
    logger_debug("Building hashtable(# of records: %d|addr: %#010x)",
                        size, stat->ns_stat_offset + sizeof(ns_stat_t));

    // int offset = stat->ns_stat_offset + sizeof(ns_stat_t);  //tbl_offset.
    //int ns_r_size = 20 + sizeof(uint32_t) + sizeof(uint32_t);
    int tbl_size_bytes = size * NAMESPACE_RECORD_LEN;
    uint32_t tbl_offset = stat->ns_stat_offset + NAMESPACE_STAT_LEN;

    uint8_t * buffer = (uint8_t*)malloc(tbl_size_bytes * sizeof(uint8_t));
    //TODO: use tbl_offset in ns_stat.
    int bytes = pread(stat->fd, buffer, tbl_size_bytes, tbl_offset);
    if (bytes < 0) {
        logger_error("Failed to read namespace tbl on disk");
    }
    printf("offset: %d\n", tbl_offset);
    logger_debug("Read namespace table on disk (Addr: %#010x - %#010x)",
                      tbl_offset, tbl_offset + tbl_size_bytes);
    int i = 0;
    for (i ; i < size; ++i) {
        ns_r_t *r = (ns_r_t*) malloc(NAMESPACE_RECORD_LEN);
        int offset = i * NAMESPACE_RECORD_LEN;
        printf("i : %d, offset: %d\n",i, offset);
        //TODO: read other fields.
        memcpy(r->fp.fingerprint, buffer + offset, FINGERPRINT_SIZE);
        //hex string to print.
        uint8_t fp_hex[FINGERPRINT_READABLE_HEX_STR_LEN];
        fp_to_readable_hex(&r->fp, fp_hex);
        logger_debug("Add #%d[offset:%d][fp:%s] to hashtable", i, offset, fp_hex);
    }
    return size;
}


int get_ns_stat(uint8_t *buf, ns_stat_t *stat) {
    // stat->size = *(uint32_t*)buf;
    // stat->tbl_offset = (uint32_t*) (buf + sizeof(uint32_t));
    return 0;
}


int update_ns_stat(ns_stat_t *stat) {
    //get ns_stat_offset

}

int ns_hashtable_add (ns_ht_r_t **hashtable, ns_ht_r_t *r) {
    HASH_ADD(hh, *hashtable, fp, sizeof(fp_t), r);
}

ns_ht_r_t * ns_hashtable_find (ns_ht_r_t **hashtable, fp_t *fp) {
    ns_ht_r_t *result = NULL;
    HASH_FIND(hh, *hashtable, fp, sizeof(fp_t), result);
    return result;
}

void ns_hashtable_delete_all(ns_ht_r_t **hashtable) {
    ns_ht_r_t *r, *tmp;
    HASH_ITER(hh, *hashtable, r, tmp) {     // loop
        HASH_DEL(*hashtable ,r);
        free(r);
    }
}

int ns_hashtable_count(ns_ht_r_t **hashtable) {
    int count = HASH_COUNT(*hashtable);
    return count;
}
/*
int ns_process_tbl_buffer(uint8_t *buffer, uint32_t record_count) {
    //read records
    int i = 0;
    for (i ; i <= record_count; ++i) {
        ns_r_t r;
        ns_get_record_from_buffer(buffer, i, &r);
        //add a record to hashtable
    }
}

int ns_get_record_from_buffer(uint8_t *buffer, uint32_t record_num, ns_r_t *r) {
    //TODO: use Marcos or const
    int ns_r_size = 20 + sizeof(uint32_t) + sizeof(uint32_t);
    int offset = record_num * ns_r_size;
    //copy fp
    memcpy(r->fp.fingerprint, buffer + offset, 20);
    //c_id, c_stat;
    r->c_id = *(uint32_t*)(buffer + offset + 20);
    r->c_stat = *(uint32_t*)(buffer + offset + 20 + sizeof(uint32_t));
}
//buf - dist_buf
int ns_write_record_to_buffer(uint8_t *buf, uint32_t record_num, ns_r_t *r) {
    int ns_r_size = 20 + sizeof(uint32_t) + sizeof(uint32_t);
    int offset = record_num * ns_r_size;
    //write to buffer.
    memcpy(buf + offset, r->fp.fingerprint, 20);
    memcpy(buf + offset + 20, &r->c_id, sizeof(uint32_t));
    memcpy(buf + offset + 20 + sizeof(uint32_t), &r->c_stat, sizeof(uint32_t));
}*/
