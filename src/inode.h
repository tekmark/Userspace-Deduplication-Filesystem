#ifndef INODE_H_
#define INODE_H_

#include<time.h>
#include<sys/types.h>
#include<string.h>
#include<fuse.h>

#include "util.h"
#include "file_recipe.h"

#define RECIPE_NUM 8
#define MAX_INODE_NUM 2048
#define DIRECT_BLK_NUM 12

/*
enum {
  REGULAR_FILE = 1,
  DIRECTORY,
  SYMBOLIC_LINK
};

typedef struct {
  uint32_t container_id;
  uint32_t blk_offset;
} indirect_blk_t;
*/
 /*
typedef struct inode {
  uint32_t inode_id;
  uint32_t inode_type;
  uint32_t file_size;        //maximum file size is 4gb
  uint32_t owner;            //uid of owner
  uint32_t mode;             //rwx, file or directory
  uint32_t link;
  time_t ctime;              //inode change time
  time_t mtime;              //file content last modification time
  time_t atime;              //file content last access time
  uint32_t direct_blk[DIRECT_BLK_NUM];
  uint32_t file_recipe;      //position of file recipe
  indirect_blk_t recipe_blk;

  time_t ctime;              //inode change time
  time_t mtime;              //file content last modification time
  time_t atime;              //file content last access time

} inode_t;
*/

/* struct of inode
 * see struct stat in <sys/stat.h>
 */
struct inode {
    /*in use*/
    uint32_t st_ino;
    uint32_t st_mode;               //rwx, file or directory
    uint32_t st_size;               //file size in byte.

    uint32_t direct_blk_no;         //blk_no of direct block, file_recipe if file.
    uint32_t indirect_blk_no;       //blk no of indirect block.
    /*TODO: to implement*/
    uint32_t st_nlink;
    uint32_t owner;                 //uid of owner
    time_t st_ctm;                  //inode change time
    time_t st_mtm;                  //file content last modification time
    time_t st_atm;                  //file content last access time

  // uint32_t direct_blk[DIRECT_BLK_NUM];
  // uint32_t file_recipe;      //position of file recipe
  // indirect_blk_t recipe_blk;
};
typedef struct inode inode_t;

int inode_write_buf(char *buf, int off, inode_t *inode);
int inode_read_buf(char *buf, int off, inode_t *inode);

int inode_root(inode_t *inode);
/*
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

void print_inodemap( inode_map_t * inodemap);
void print_filerecipe( file_recipe_t *);
void root_inode_init(inode_t *);
uint32_t filerecipe_add_entry( file_recipe_t *, file_recipe_record_t *);
*/
#endif
