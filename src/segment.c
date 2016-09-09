#include "segment.h"

segment_t * seg_alloc(int seg_size) {
    segment_t *seg = (segment_t*)malloc(sizeof(segment_t));
    seg->size = seg_size;
    seg->data = (uint8_t*)malloc(seg_size * sizeof(uint8_t));
    return seg;
}

void seg_free(segment_t *seg) {
    free(seg->data);
    free(seg);
}

uint8_t* seg_compute_fingerprint (segment_t *seg, fp_t *fp){
    return fp_compute(seg->data, seg->size, fp);
}
