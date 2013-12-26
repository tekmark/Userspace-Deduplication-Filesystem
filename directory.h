#ifndef DIRECTORY__H
#define DIRECTORY__H

#include <stdint.h>
#include "global.h"
#include "util.h"
#include "inode.h"
#define MAX_FILENAME_LENGTH 256
#define NUM_OF_RECORD_PER_SEG 15

#define SUCCESS 0
#define FAIL    -1

typedef struct {                          //260bytes per entry, 4096/260 = 15
  char filename[MAX_FILENAME_LENGTH];     //255 + 1
  uint32_t inode_id;                      //4 byte; 
} dir_record_t; 

// structure to hold directory data
typedef struct {
  uint32_t num;          // number of records
  dir_record_t *records; // records
} dir_t; 

uint32_t dir_get_inode(const char *path, inode_t * inode);
dir_t * open_root_dir();
dir_t *open_cur_dir();
dir_t *open_parent_dir();

uint32_t get_division_result(uint32_t file_size, uint32_t blk_size);

uint32_t dir_add_entry(dir_t *dir, const char* filename, uint32_t inode_id);
uint32_t dir_remove_entry(dir_t *dir, const char* filename);
void print_dir_data(dir_t *dir); 

uint32_t get_inode_id_from_filename (char *fname, dir_t * dir_data,
    uint32_t *pinode_id);
inode_t * get_inode_from_inode_id (uint32_t inode_id);
uint32_t dir_commit_changes(dir_t *dir, inode_t *inode);

#endif
