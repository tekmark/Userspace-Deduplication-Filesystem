#ifndef CONTAINER_H_
#define CONTAINER_H_

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include"global.h"
#include"util.h"
#include"lfs.h"

const uint32_t c_blk_size = 4096;                 //block size is 4K
const uint32_t c_seg_size = 4096;           //segment equals block size
const uint32_t c_container_blk_num = 8;           
const uint32_t c_container_seg_num = 8;
const uint32_t c_container_size = 4096 * 1024;
const uint32_t c_max_container_num = 1024;

typedef struct {
  fingerprint_t fp;
  uint32_t seg; 
} fingerprint_seg_record_t; 

typedef struct {
  uint32_t container_id;
  uint32_t container_type; 
  fingerprint_seg_record_t records[8];   //should be same as container_seg_num
} container_header_t; 

typedef struct container{
  container_header_t *header; 
  char *buf;
  uint32_t seg_offset; 
} container_t; 

//initialize a container, return a new container.
container_t* container_init();
void container_free(container_t *); 

uint32_t container_copy(container_t * dst_container, 
                    container_t * src_container); 

uint32_t container_write( container_t *container, uint32_t *new_id); 

uint32_t container_read( container_t *container, uint32_t container_id); 

void container_clean( uint32_t container_id, uint32_t *live_seg_vec, 
                      uint32_t size); 

uint32_t container_add_seg( container_t *container, char *seg_buf);
uint32_t container_get_seg( container_t *container, uint32_t seg_offset, 
                            char *seg_buf); 
#endif
