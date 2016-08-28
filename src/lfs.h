#ifndef LFS_H_
#define LFS_H_

#include<stdio.h>
#include<unistd.h>
#include<stdint.h>
#include<stdlib.h>
#include<assert.h>
#include<fuse.h>
#include<errno.h>
#include<sys/types.h>
#include "logger.h"
#include"uthash.h"
#include"util.h"
#include"inode.h"
#include"global.h"
#include"directory.h"

//configure struct for lfs.
typedef struct {
    uint32_t fd;
    uint32_t log_level;
} lfs_config_t;

//create a file for, return lfs_config_t.
//int32_t lfs_create (lfs_config_t *lfs_config);

int32_t lfs_file_create(const char *filename);

void lfs_init();
void initialize();

//TODO: call fuse_main();
int lfs_main ();

#endif
