#ifndef LFS_CONSTANTS_H
#define LFS_CONSTANTS_H

#include <stdint.h>

/*
  relationship between I/O block and container.
 */
//c_blk_size(BLOCK_SIZE) is the most basic disk I/O size
extern const uint32_t c_default_blk_size;
extern const uint32_t c_default_blks_per_container;
// extern const uint32_t c_default_container_blk_num;

//c_seg_size（SEG_SIZE) is the size of segment of file data
extern const uint32_t c_default_seg_size;            //segment equals block size
extern const uint32_t c_default_containers;          //default # of containers.
extern const uint32_t c_max_container_num;


extern const uint32_t c_container_header_size;      //container header size.


#endif