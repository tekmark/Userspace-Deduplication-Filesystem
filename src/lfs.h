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
#include"uthash.h"
#include"util.h"
#include"inode.h"
#include"global.h"
#include"directory.h"


void lfs_init(); 

#endif
