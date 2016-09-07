#include "fingerprint.h"


//const int c_sha1_len = 20;
//const int c_fingerprint_len = c_sha1_len;
//const int c_fingerprint_readable_hex_str_len = c_fingerprint_len + 2 * sizeof(uint32_t);

fp_t * fp_cpy(fp_t *dest, fp_t *src) {
    memcpy(dest->fingerprint, src->fingerprint, sizeof(fp_t));
    return dest;
}

int fp_cmp(fp_t *fp1, fp_t *fp2) {
    int i;
    for (i = 0; i < FINGERPRINT_LEN; ++i) {
        if (fp1->fingerprint[i] != fp2->fingerprint[i]) {
            return -1;
        }
    }
    return 0;
}

void fp_to_readable_hex(fp_t *fp, uint8_t *readable) {
    int i;
    for (i = 0; i < FINGERPRINT_LEN; ++i) {
        sprintf(&readable[i * 2], "%02X", fp->fingerprint[i]);
    }
    readable[FINGERPRINT_READABLE_HEX_STR_LEN - 1] = 0;
}

uint8_t * fp_compute(uint8_t *buf, size_t bytes, fp_t *fp) {
    return SHA1(buf, bytes, fp->fingerprint);
}
