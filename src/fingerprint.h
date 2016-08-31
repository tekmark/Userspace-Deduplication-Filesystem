#ifndef FINGERPRINT_H
#define FINGERPRINT_H

#include <stdint.h>
#include <string.h>
#include <stdio.h>

#include <openssl/sha.h>

#define SHA1_LEN 20
#define FINGERPRINT_LEN SHA1_LEN

#define FINGERPRINT_READABLE_HEX_STR_LEN 2 * FINGERPRINT_LEN + 1

struct fp {
  uint8_t fingerprint[FINGERPRINT_LEN];
};
typedef struct fp fp_t;

struct fp_readable {
    uint8_t hex[FINGERPRINT_READABLE_HEX_STR_LEN];
};
typedef struct fp_readable fp_readable_t;


fp_t * fp_cpy(fp_t *dest, fp_t *src);
void fp_to_readable_hex(fp_t *fp, uint8_t *readable);

//SHA1 wrapper,
//buf -
//bytes -
uint8_t * fp_compute(uint8_t *buf, size_t bytes, fp_t *fp);

#endif
