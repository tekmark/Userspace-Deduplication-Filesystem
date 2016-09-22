#include "file_recipe.h"

int fr_add_ent(fr_t *filerecipe, fr_ent_t *entry) {
    int size = filerecipe->size;
    if (size >= FILE_RECIPE_ENTS_PER_BLK) {
        return -1;
    }
    memcpy(&filerecipe->entries[size], entry, sizeof(fr_ent_t));
    filerecipe->size += 1;
    return filerecipe->size;
}

int file_recipe_get(uint8_t *direct_blk, uint8_t *indirect_blk) {

}

int fr_ent_write_buf(uint8_t *buf, uint32_t offset, fr_ent_t *fr_entry) {
    memcpy(buf + offset, fr_entry, sizeof(fr_ent_t));
    return offset + FILE_RECIPE_ENT_LEN;
}

int fr_ent_read_buf(uint8_t *buf, uint32_t offset, fr_ent_t *fr_entry) {
    memcpy(fr_entry, buf + offset, sizeof(fr_ent_t));
    return offset + FILE_RECIPE_ENT_LEN;
}

int file_recipe_get_from_blk(uint8_t *blk, fr_t *recipe) {
    int size = 2;
    //recipe->entries = (fr_ent_t*)malloc(size * sizeof(fr_ent_t));
    //memset(recipe->entries, 0 , size * sizeof(fr_ent_t));
    memcpy(recipe->entries[0].seg_fp.fingerprint, blk, FINGERPRINT_LEN);

    return size;
}

int fr_write_buf(uint8_t *buf, int offset, fr_t *recipe) {
    int size = recipe->size;
    int i = 0;
    for (; i < size; ++i) {
        int j = i * FILE_RECIPE_ENT_LEN;
        // memcpy(buf + offset + j, recipe->entries[0].seg_fp.fingerprint, FINGERPRINT_LEN);
        fr_ent_write_buf(buf, j, &recipe->entries[i]);
    }
}

int fr_read_buf(uint8_t *buf, int offset, fr_t *recipe) {
    int size = 2;
    //recipe->entries = (fr_ent_t*)malloc(size * sizeof(fr_ent_t));
    int i = 0;
    for(; i < size; ++i) {
        int j = i * FILE_RECIPE_ENT_LEN;
        fr_ent_read_buf(buf, j, &recipe->entries[i]);
    }
}

int file_recipe_get_from_indirect_blk() {

}


int fr_read_blk(uint8_t *blk, uint32_t blk_size, fr_t *recipe) {

}
int fr_write_blk(uint8_t *blk, uint32_t blk_size, fr_t *recipe) {
    int size = recipe->size;
    int i = 0;
    //TODO: blk_size edge case check.
    int nbytes = 0;
    for (; i < size; ++i) {
        nbytes = fr_ent_write_buf(blk, nbytes, &recipe->entries[i]);
    }
    return nbytes;
}
