#ifndef FILE_RECIPE_H
#define FILE_RECIPE_H

#include <stdint.h>

#include "fingerprint.h"

#define FILE_RECIPE_ENT_LEN 32
#define FILE_RECIPE_ENTS_PER_BLK 128

// 4096 = 32 * 128
/* 0                                  32
 * |--------file recipe block---------|
 * | entry#0
 * | entry#1
 * | entry#2 ...
 */

typedef uint32_t blk_no_t;

struct file_recipe_ent {
    fp_t seg_fp;
    uint32_t seg_size;              //seg size in byte.
    blk_no_t blk_no;                //blk no.
};
typedef struct file_recipe_ent fr_ent_t;

struct file_recipe {
    uint32_t size;
    fr_ent_t entries[FILE_RECIPE_ENTS_PER_BLK];
    // struct file_recipe *next;
};
typedef struct file_recipe fr_t;

int fr_add_ent(fr_t *filerecipe, fr_ent_t *entry);
int fr_modify_ent(fr_t *filerecipe, uint32_t ent_seq_no, fr_ent_t *entry);
int fr_remove_ent(fr_t *filerecipe, uint32_t ent_seq_no);

int fr_read_blk(uint8_t *blk, uint32_t blk_size, fr_t *recipe);
int fr_write_blk(uint8_t *blk, uint32_t blk_size, fr_t *recipe);

int file_recipe_get_from_blk(uint8_t *blk, fr_t *recipe);

int fr_write_buf(uint8_t *buf, int offset, fr_t *recipe);
int fr_read_buf(uint8_t *buf, int offset, fr_t *recipe);

#endif
