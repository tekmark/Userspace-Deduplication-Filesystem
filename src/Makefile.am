#flags
AM_CFLAGS=-g -Wall -D_FILE_OFFSET_BITS=64 -DFUSE_USE_VERSION=26
AM_LDFLAGS=-lfuse -lcrypto

bin_PROGRAMS=lfs
lfs_SOURCES=container.c directory.c global.h inode.h lfs.h uthash.h  util.h container.h directory.h inode.c lfs.c util.c
