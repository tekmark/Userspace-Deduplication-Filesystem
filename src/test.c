#include "test.h"

void test_file_recipe() {
    logger_debug("TESTING file recipe...");
    logger_debug("ft_ent_t => size:%d", sizeof(fr_ent_t));
    //create a file recipe for testing.
    fr_t recipe;
    recipe.size = 2;
    //fr_ent_t
    //entry#0;
    char *text1 = "i am text file 1";
    fp_compute(text1, strlen(text1), &recipe.entries[0].seg_fp);
    //entry#1
    char *text2 = "i am text file 2";
    fp_compute(text2, strlen(text2), &recipe.entries[1].seg_fp);

    uint8_t * blk = (uint8_t*)malloc(4096);
    fr_write_buf(blk, 0, &recipe);
    // fr_t recipe;
    int size = fr_read_buf(blk, 0, &recipe);
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
    container_t *c2 = container_alloc();
    container_read(0, c2);
    container_print(c2);
    segment_t *seg2 = container_get_seg(c2, 0);
    logger_debug("seg_size=>%d", seg2->size);
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
}
