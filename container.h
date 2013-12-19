#ifndef CONTAINER_H_
#define CONTAINER_H_

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include"global.h"
#include"util.h"
#include"lfs.h"

#define BLK_SIZE 4096                             //block size is 4K
#define SEG_SIZE BLK_SIZE                         //segment equals block size
#define CONTAINER_BLK_NUM 8                       //8 blocks per container
#define CONTAINER_SEG_NUM CONTAINER_BLK_NUM-1     //1st block is for container 
                                                  //header
#define CONTAINER_SIZE BLK_SIZE*CONTAINER_BLK_NUM //

#define MAX_CONTAINER_NUM 1024      //disk size is 1024 * 4K * 8 + 4K      

#define DATA 0
#define METADATA 1

typedef struct {
  fingerprint_t fp;
  uint32_t seg; 
} fingerprint_seg_record_t; 

typedef struct {
  uint32_t container_id;
  uint32_t container_type; 
  fingerprint_seg_record_t records[CONTAINER_SEG_NUM]; 
} container_header_t; 

typedef struct container{
  container_header_t *header; 
  char *buf;
  uint32_t offset; 
} container_t; 

//initialize a container
container_t* container_init();
uint32_t container_copy(container_t * dst_container, 
                    container_t * src_container); 
void container_free(container_t *); 

uint32_t container_write( container_t *container, uint32_t *new_id); 

uint32_t container_read( container_t *container, uint32_t container_id); 

void container_clean( uint32_t container_id, uint32_t *live_seg_vec, 
                      uint32_t size); 

uint32_t container_add_seg( container_t *container, char *seg_buf);
uint32_t container_get_seg( container_t *container, uint32_t seg_offset, 
                            char *seg_buf); 
#endif
