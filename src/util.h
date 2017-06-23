#ifndef UTIL_H_
#define UTIL_H_

//#include<openssl/sha.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "lfs_stat.h"
#include "container_types.h"
#include "fingerprint.h"
#include "file_recipe.h"
#include "logger.h"


void util_test();
void print_filerecipe(fr_t *filerecipe);

//calculate address
int calculate_blk_no(lfs_stat_t *stat, int cid, int blk_off);
int calculate_container_id(lfs_stat_t *stat, int addr);
int calculate_blk_offset(lfs_stat_t *stat, int addr);



//helper functions for fingerprint.
// fingerprint_t * fp_cpy(fingerprint_t *dest, fingerprint_t *src);
// void fp_to_readable_hex(fp_t *fp, uint8_t *readable);

//SHA1 wrapper,
//buf -
//bytes -
//uint8_t* compute_fingerprint(uint8_t *buf, size_t bytes, fingerprint_t *fp);

// void fingerprint_print(fingerprint_t *fp);
// const char * get_filename(const char*path);
char* get_parentpath(const char *path);
int32_t get_division_result (uint32_t dividend, uint32_t divisor);

//convert string to uppercases.
void string_toupper(char *str);

// int calculate_blk_no(lfs_stat_t *stat, int cid, int blk_offset);
//file size calculator
//uint32_t calculate_lfs_file_size(int blk_size, int container_blk_num, int container_num);
// void print_lfs_stat(lfs_stat_t *stat);

#endif
