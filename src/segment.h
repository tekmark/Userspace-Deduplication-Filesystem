#ifndef SEGMENT_H
#define SEGMENT_H

#include <stdint.h>
#include <stdlib.h>

#include "fingerprint.h"

struct segment {
    int blks;
    int size;
    uint8_t *data;
};
typedef struct segment segment_t;

segment_t * seg_alloc(int size);
void seg_free(segment_t *seg);

uint8_t* seg_compute_fingerprint (segment_t *seg, fp_t *fp);

#endif
