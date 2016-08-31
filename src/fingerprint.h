#ifndef FINGERPRINT_H
#define FINGERPRINT_H

#define SHA1_LEN 20
#define FINGERPRINT_LEN SHA1_LEN

extern const uint32_t c_fp_len;

struct fp {
  uint8_t data[FINGERPRINT_LEN];
};
typedef struct fp fp_t;


#endif
