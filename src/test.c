#include "test.h"
#include "logger.h"
#include "container.h"
#include "container_header.h"
#include "container_utils.h"


//test hearder operations.
void test_container_header() {
    logger_debug("TESTING container header operations...");
    //alloc buffer for constructing header.
    char *buf = (char*)malloc(c_default_blk_size);

    c_header_t header;
    c_header_init (buf, &header);

    *header.c_id = 1;
    c_header_set_data_defaults(&header);

    const char *text = "I am segment A.";
    segment_t *seg_a = seg_alloc(strlen(text) + 1);
    strcpy(seg_a->data, text);
    logger_debug("segA size: %d, data : %s", seg_a->size, seg_a->data);

    seg_tbl_ent_t seg_a_ent;
    seg_a_ent.seg_size = strlen(text) + 1;
    seg_compute_fingerprint(seg_a, &seg_a_ent.seg_fp);
    char fp_hex[FINGERPRINT_READABLE_HEX_STR_LEN];
    fp_to_readable_hex(&seg_a_ent.seg_fp, fp_hex);
    logger_debug("segA fingerprint=>[%s]", fp_hex);

    int ret = c_header_add_seg_ent(&header, &seg_a_ent);
    if (ret < 0) {
        logger_error("Failed to add seg A entry to header");
    } else {
        logger_debug("Added segment A to header, pos=>%d", ret);
    }

    const char *text2 = "I am segment B, not segment A!";
    segment_t *seg_b = seg_alloc(strlen(text2) + 1);
    strcpy(seg_b->data, text2);
    logger_debug("segB size: %d, data : %s", seg_b->size, seg_b->data);

    seg_tbl_ent_t seg_b_ent;
    seg_b_ent.seg_size = strlen(text2) + 1;
    seg_compute_fingerprint(seg_b, &seg_b_ent.seg_fp);
    fp_to_readable_hex(&seg_b_ent.seg_fp, fp_hex);
    logger_debug("segB fingerprint=>[%s]", fp_hex);

    ret = c_header_add_seg_ent(&header, &seg_b_ent);
    if (ret < 0) {
        logger_error("Failed to add seg B entry to header");
    } else {
        logger_debug("Added segment B to header, pos=>%d", ret);
    }

    c_utils_print_container_header(&header);

    logger_debug("write header to buffer");
    char buffer2[4096];
    c_header_buf_write(buffer2, 4096, &header);

    logger_debug("read header from buffer");
    c_header_t header2;
    c_header_buf_read(buffer2, 4096, &header2);
    
    c_utils_print_container_header(&header2);

}

/*
void test_file_recipe() {
    logger_debug("TESTING file recipe...");
    logger_debug("ft_ent_t => size:%d", sizeof(fr_ent_t));
    //create a file recipe for testing.
    fr_t recipe;
    //recipe.size = 2;
    //fr_ent_t
    //entry#0;
    char *text1 = "i am text file 1";
    fr_ent_t ent0;
    fp_compute(text1, strlen(text1), &ent0.seg_fp);
    fr_add_ent(&recipe, ent0);
    //entry#1
    fr_ent_t ent1;
    char *text2 = "i am text file 2";
    fp_compute(text2, strlen(text2), &ent1.seg_fp);
    fr_add_ent(&recipe, ent1);

    uint8_t * blk = (uint8_t*)malloc(4096);
    //fr_write_buf(blk, 0, &recipe);
    // fr_t recipe;
    //int size = fr_read_buf(blk, 0, &recipe);
    fr_write_blk(blk, 4096, &recipe);
    print_filerecipe(&recipe);

    fr_t recipe2;
    fr_read_blk(blk, 4096, &recipe2);
    print_filerecipe(&recipe2);

    fr_remove_ent(&recipe, 0);
    print_filerecipe(&recipe);
}

void test_container() {
    logger_debug("TESTING container operations ...");
    container_t *container = container_alloc();

    //init container, set to defaults.
    *container->header->type = 98;
    *container->header->data_blk_offset = 1;
    *container->header->seg_tbl->offset = CONTAINER_HEADER_DEFAULT_SEG_TBL_OFFSET;

    segment_t *s = seg_alloc(4096);
    const char * text = "i am segment, hahaha";
    memcpy(s->data, text, strlen(text));
    // container_write_seg(container, s, 1);
    container_add_seg(container, s);

    container_print(container);
    container_write(container);

    fp_t fp0;
    //fp_compute(text, strlen(text), fp0);
    seg_compute_fingerprint(s, &fp0);
    char fp_readable[FINGERPRINT_READABLE_HEX_STR_LEN];
    fp_to_readable_hex(&fp0, fp_readable);
    logger_debug("FP=>%s", fp_readable);
    segment_t *seg = container_get_seg_by_fp(container, &fp0);
    assert(seg);

    // segment_t *s2 = container_get_seg(container, 0);

    /*container_buf_print(container->buffer);
    container_write(container);
    container_buf_print(container->buffer);
    // container_free(container);
    //*/
    // container_t *c2 = container_alloc();
    // container_print(c2);
    // container_read(0, c2);
    // segment_t *seg2 = container_get_seg(c2, 0);
    // logger_debug("seg_size=>%d", seg2->size);
    // int ret = container_read(1, c2);
    //int cid = container_write(c2);
    // container_buf_print(c2->buffer);
    /*container_t *c3 = container_alloc();
    container_read(0, c3);
    // container_write(c2);
    container_buf_print(c2->buffer);
    segment_t *s2 = container_read_seg(c3, 4096, 4096);
    logger_debug("Segment Data: %s", s2->data);*/
    // container_free(c2);
// }

void test_all() {
    test_container_header();
}
