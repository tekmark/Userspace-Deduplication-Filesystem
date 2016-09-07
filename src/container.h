#ifndef CONTAINER_H_
#define CONTAINER_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "container_header.h"
#include "container_utility.h"
#include "global.h"
#include "util.h"
#include "lfs.h"
#include "lfs_stat.h"
#include "logger.h"
#include "segment.h"


/*
  relationship between I/O block and container.
 */
//c_blk_size(BLOCK_SIZE) is the most basic disk I/O size
// extern const uint32_t c_blk_size;                 //block size is 4K
// extern const uint32_t c_container_blk_num;

//c_seg_size（SEG_SIZE) is the size of segment of file data
// extern const uint32_t c_seg_size;                 //segment equals block size
// extern const uint32_t c_container_seg_num;
// extern const uint32_t c_container_size;
// extern const uint32_t c_max_container_num;

/*
  buffer fomart:
  byte_0 (*buffer / *header)--> | container header ...
                    *data   --> | container data ...
 ------------------------------------------------------------------
 buffer     : pointer to byte_0 in buffer.
 header     : pointer to container header. see details in container_header.h
 data       : pointer to container data.
 */
struct container {
    //pointers.
    char *buffer;
    container_header_t *header;
    char *data;
    //size_t buf_size;
};
typedef struct container container_t;

/*
 container (8 blocks), segment(1 block);
 |-----------------------------------------------------------|
 |                         container                         |
 |-----------------------------------------------------------|
 |  blk0  |  blk1  |  blk2  |  blk3  |  blk4  | ... |  blk7  |
 |-header-|------------data (data / metadata)----------------|   //data is container data => file data/metadata.
 |--------|  seg0  |  seg1  |  seg2  |  seg3  | ... |  seg7  |   //if seg_size if fixed, and seg size = blk_size
          |
     offset_byte (in header);
 */

/*
 API:
    1. write from buffer to disk
    2. read from disk to buffer
    3. clean invalid containers.
memory operations: (in container.c)
    1. memory alloc
    2. memory free
 */

void container_test();

//memeory operations.
//container_alloc() and container_free()
container_t *container_alloc();
void container_free(container_t *);

//container operations.
int container_write (container_t *container);
int container_read (uint32_t container_id, container_t *container);

//uint32_t container_copy(container_t * dst_container, container_t * src_container);

void container_clean(uint32_t container_id, uint32_t *seg_ids, uint32_t size);


int container_write_seg(container_t *container, segment_t *seg, int blk_offset);
segment_t * container_read_seg(container_t *container, int blk_offset, int count);


int container_add_seg(container_t *container, segment_t *seg);
segment_t * container_get_seg(container_t *container, int seg_no);
// uint32_t container_get_seg(container_t *container, uint32_t seg_offset,
                            // char *seg_buf);
/*
uint32_t container_header_add_fingerprint(container_t*, fingerprint_seg_record_t*);
uint32_t container_header_find_fingerprint(container_t*, fingerprint_t*);
void container_print_header(container_t * container);
*/

segment_t * container_get_seg_by_fp(container_t *container, fp_t *fp);

void container_print(container_t *container);

#endif
