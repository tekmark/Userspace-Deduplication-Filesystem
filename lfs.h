#ifndef LFS_H_
#define LFS_H_

#include<stdio.h>
#include<unistd.h>
#include<stdint.h>
#include<stdlib.h>
#include<assert.h>
#include<fuse.h>
#include<sys/types.h>
#include"uthash.h"
#include"util.h"
#include"inode.h"
#include"container.h"

#define BLK_SIZE 4096                             //block size is 4K
#define SEG_SIZE BLK_SIZE                         //segment equals block size
#define CONTAINER_BLK_NUM 8                       //8 blocks per container
#define CONTAINER SEG_NUM CONTAINER_BLK_NUM-1     //1st block is for container 
                                                  //header
#define CONTAINER_SIZE BLK_SIZE*CONTAINER_BLK_NUM //

#define MAX_FILENAME_LEN 256
#define MAX_CONTAINER_NUM 1024         


typedef struct {
  fingerprint_t fp;            //fingerprint as key in hashtable
  uint32_t container_id;       //container_id as value in hashtable
  UT_hash_handle hh; 
} namespace_record_t; 

namespace_record_t *lfs_namespace; 

typedef struct {
  inode_map_t *imap;
  uint32_t n_inode;
  
  namespace_record_t *lfs_namespace; 
  
  container_t *cur_metadata_container;
  container_t *cur_data_container;
  container_t *cur_filerecipe_container;

  uint32_t fd; 
} lfs_global_info_t; 

lfs_global_info_t *lfs_info; 

void lfs_init(); 

#endif