#include "fingerprint.h"

fp_t * fp_cpy(fp_t *dest, fp_t *src) {
    memcpy(dest->fingerprint, src->fingerprint, sizeof(fp_t));
    return dest;
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
