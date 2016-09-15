#ifndef FILE_RECIPE_H
#define FILE_RECIPE_H

#include <stdint.h>

#include "fingerprint.h"

#define FILE_RECIPE_ENTRY_LEN 24;

struct file_recipe_entry {
    fp_t seg_fp;
    uint32_t blk_no;
};

typedef struct file_recipe_entry file_recipe_entry_t;
#endif
