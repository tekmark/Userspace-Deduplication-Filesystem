#ifndef LFS_GLOBAL_H_
#define LFS_GLOBAL_H_

#include<stdint.h>
#include"inode.h"
#include"container.h"
#include"util.h"
#include"uthash.h"
#define MAX_FILENAME_LEN 256

typedef struct container container_t; 

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

  container_t *cur_inode_container;
  container_t *cur_data_container;
  container_t *cur_filerecipe_container;
  container_t *cur_dir_data_container;
  
  inode_t *cur_inode; 
   
  uint32_t fd;
} lfs_global_info_t;

lfs_global_info_t *lfs_info;

#endif