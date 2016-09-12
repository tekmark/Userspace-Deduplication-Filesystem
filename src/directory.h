#ifndef DIRECTORY__H
#define DIRECTORY__H

#include <stdint.h>

// #include "global.h"
#include "util.h"
#include "inode.h"
#include "logger.h"
#include "lfs_stat.h"

/*
    | # of files in direcory |

 */

#define MAX_FILENAME_LEN 255
#define DIR_RECORD_LEN 260

#define DIR_RECORD_OFFSET sizeof(uint32_t);
#define DIR_RECORD_FILENAME_OFFSET 0
#define DIR_RECORD_INO_OFFSET 256
//#define NUM_OF_RECORD_PER_SEG 15

#define DIR_ROOT_INO 0

extern const char *cur_dir_str;
extern const char *parent_dir_str;
extern const char *root_dir_str;
extern const char *root_path;

#define SUCCESS 0
#define FAIL   -1

struct dir_record {                         //260bytes per entry, 4096/260 = 15
    char filename[MAX_FILENAME_LEN + 1];            //255 + 1
    uint32_t inode_id;                              //4 byte;
};
typedef struct dir_record dir_record_t;


// structure to hold directory data
struct dir {
    uint8_t *buffer;
    uint32_t *size;          // number of records
    // dir_record_t *records; // records
    uint8_t *records;
};
typedef struct dir dir_t;


int dir_add_record(dir_t *dir, const char* filename, uint32_t inode_id);
int dir_get_record_by_filename(dir_t *dir, const char * filename);

int dir_find_record_by_filename(dir_t *dir, const char * filename);

int dir_del_record_by_filename(dir_t *dir, const char* filename);
int dir_del_record_by_ino(dir_t *dir, uint32_t ino);

// int dir_get_record();

void dir_print(dir_t *);

void dir_test();

dir_t * new_dir(int parent_ino, int cur_ino);
// int dir_read_from_buffer(uint8_t *buffer, dir_t *dir);

dir_t * new_dir_by_buf(uint8_t *buf);
int dir_read_from_buf(uint8_t *buffer, dir_t *dir);


void dir_create_root();
//
// int32_t dir_get_inode(const char *path, inode_t * inode);
// dir_t * open_root_dir();
// dir_t *open_cur_dir();
// dir_t *open_parent_dir();
//
// int32_t get_division_result(uint32_t file_size, uint32_t blk_size);
//
// int32_t dir_add_entry(dir_t *dir, const char* filename, uint32_t inode_id);
// int32_t dir_remove_entry(dir_t *dir, const char* filename);
// int32_t dir_change_entry(dir_t *dir, const char* old_filename, const char* new_filename);
// void print_dir_data(dir_t *dir);
//
// int32_t get_inode_id_from_filename (const char *fname, dir_t * dir_data,
//     uint32_t *pinode_id);
// int32_t get_inode_from_inode_id (inode_t *, uint32_t inode_id);
// int32_t dir_commit_changes(dir_t *dir, inode_t *inode);

#endif
