#ifndef INODE_H_
#define INODE_H_

#include<time.h>
#include"util.h"

#define RECIPE_NUM 8
#define MAX_INODE_NUM 2048
#define DIRECT_BLK_NUM 12

enum {
  REGULAR_FILE = 1,
  DIRECTORY,
  SYMBOLIC_LINK
};

typedef struct {
  uint32_t container_id;
  uint32_t blk_offset; 
} indirect_blk_t; 

typedef struct {
  uint32_t inode_id;
  uint32_t inode_type; 
  uint32_t file_size;        //maximum file size is 4gb
  uint32_t owner;            //uid of owner
  uint32_t mode;             //rwx, file or directory 
  time_t ctime;              //inode change time
  time_t mtime;              //file content last modification time
  time_t atime;              //file content last access time
  uint32_t direct_blk[DIRECT_BLK_NUM]; 
  uint32_t file_recipe;      //position of file recipe
  indirect_blk_t recipe_blk; 
} inode_t; 


typedef struct {
  uint32_t seg_num;
  fingerprint_t fingerprint; 
} file_recipe_record_t; 

typedef struct {
  file_recipe_record_t records[RECIPE_NUM]; 
} file_recipe_t; 

typedef struct {
  uint32_t inode_id;
  uint32_t inode_addr;        //postion of inode, use it can calculate and find
                              //inode positon 
} inode_map_record_t;  

typedef struct {
  inode_map_record_t records[MAX_INODE_NUM]; 
} inode_map_t; 

#endif
