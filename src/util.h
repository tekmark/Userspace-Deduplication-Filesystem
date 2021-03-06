#ifndef UTIL_H_
#define UTIL_H_

#include<openssl/sha.h>
#include<stdio.h>
#include<stdint.h>
#include<stdlib.h>
#include<string.h>

#define FINGERPRINT_SIZE 20    //sha1 result is 20 bytes 

typedef struct {
  uint8_t fingerprint[FINGERPRINT_SIZE];
} fingerprint_t;

uint8_t* compute_fingerprint(fingerprint_t *fp, uint8_t *buf, uint32_t size);
void fingerprint_print(fingerprint_t *fp); 
const char * get_filename(const char*path); 
char* get_parentpath(const char *path);
int32_t get_division_result (uint32_t dividend, uint32_t divisor);

#endif
