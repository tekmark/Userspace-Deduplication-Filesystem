#ifndef UTIL_H_
#define UTIL_H_

#include<openssl/sha.h>
#include<stdio.h>
#include<stdint.h>
#include<stdlib.h>
#include<string.h>
#include <ctype.h>

#include "container_types.h"

void util_test();

//SHA1 wrapper,
//buf -
//bytes -
uint8_t* compute_fingerprint(uint8_t *buf, size_t bytes, fingerprint_t *fp);

void fingerprint_print(fingerprint_t *fp);
const char * get_filename(const char*path);
char* get_parentpath(const char *path);
int32_t get_division_result (uint32_t dividend, uint32_t divisor);

//convert string to uppercases.
void string_toupper(char *str);

//file size calculator
//uint32_t calculate_lfs_file_size(int blk_size, int container_blk_num, int container_num);

#endif
