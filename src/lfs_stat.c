#include "lfs_stat.h"

// lfs_stat_t sys_stat;
static lfs_stat_t stat;

lfs_stat_t * get_lfs_stat() {
    return &stat;
}

int lfs_stat_init(int fd, lfs_summary_t *summary) {
    //set stat fields.
    // set fd.
    stat.fd = fd;
    // set block size.
    stat.blk_size = summary->blk_size;
    // set # of system reserved block
    stat.sys_reserved_blks = summary->sys_reserved_blks;
    // set # of blocks per container.
    stat.blks_per_container = summary->blks_per_container;
    // set container size (in bytes).
    stat.container_size = summary->container_size;
    // set # of containers
    stat.containers = summary->containers;
    //
    stat.ns_stat_offset = summary->ns_stat_offset;
    // set size of filesystem, in bytes.
    stat.size = summary->filesystem_size;

    //namespace
    //alloc memeory for namespace.
    //ns_t *namespace = new_namespace();
    stat.ns = new_namespace();
    //read namespace status on disk.
    int ret = ns_stat_read_disk(stat.ns->ns_stat);

    int ns_tbl_size = stat.ns->ns_stat->size;
    if (ns_tbl_size > 0) {
        //calcuate bytes
        int ns_size = ns_tbl_size * NAMESPACE_RECORD_LEN;
        logger_debug("Namespace: # of entries : %d, table size on disk is %d bytes",
                      ns_tbl_size, ns_size);
        //build hashtable from disk.
        ns_hashtable_build();
    } else {        //if tbl_size is empty.
        logger_debug("No entry in Namespace.");
    }

    //inode map.
    //stat->inodemap_size = summary.inodemap_size;
    inodemap_t *inodemap = new_inodemap();
    inodemap->stat->tbl_size = summary->inodemap_size;
    inodemap->stat->tbl_offset = summary->inodemap_offset;
    stat.imap = inodemap;

    int imap_tbl_size = stat.imap->stat->tbl_size;
    if (imap_tbl_size > 0) {
        logger_debug("Inode map: # of entries : %d", imap_tbl_size);
    } else {
        logger_debug("No entry in inode map. root direcotry is missing");
    }

        // //logger_debug("Skip. Namespace records is 0.");
        // //TODO: remove test cases.
        // //get a real fingerprint.
        // char *text = "I am a text file ! hash me";
        // fp_t fp0;
        // //compute_fingerprint(text, strlen(text), &fp0);
        // fp_compute(text, strlen(text), &fp0);
        // //fingerprint_print(&fp0);
        //
        // //test fp_cpy();
        // fp_t fp1;
        // fp_cpy(&fp1, &fp0);
        // uint8_t *buffer = (uint8_t*)malloc(FINGERPRINT_READABLE_HEX_STR_LEN);
        // fp_to_readable_hex(&fp0, buffer);
        // logger_debug("FP-> %s", buffer);
        // //fingerprint_print(&fp1);
        //
        // //test hashtale operations.
        // ns_r_t r1;
        // fp_cpy(&r1.fp, &fp1);
        // //fingerprint_print(&r1.fp);
        // r1.c_id = 199;
        // r1.c_stat = 1;
        //
        // ns_add_record(&r1);
        // ns_add_record(&r1);
    // }
}

void lfs_stat_print() {
    logger_info("----------filesystem stats-----------------");
    logger_info("File discriptor (fd)       : %d", stat.fd);
    logger_info("Filesystem size (in bytes) : %d", stat.size);
    logger_info("Block size (in bytes)      : %d", stat.blk_size);
    logger_info("Container size (in bytes)  : %d", stat.container_size);
    logger_info("# of system reserved block : %d", stat.sys_reserved_blks);
    logger_info("# of blocks per container  : %d", stat.blks_per_container);
    logger_info("# of containers            : %d", stat.containers);
    logger_info("namespace status offset    : %d", stat.ns_stat_offset);

    logger_info("inodemap offset            : %d", stat.imap->stat->tbl_offset);
    logger_info("# of inodemap entries      : %d", stat.imap->stat->tbl_size);
    logger_info("-------------------------------------------");
}
