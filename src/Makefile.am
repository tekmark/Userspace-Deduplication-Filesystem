#flags
AM_CFLAGS=-g -Wall
#AM_LDFLAGS=-lfuse -lcrypt

bin_PROGRAMS=lfs
lfs_CFLAGS=-D_FILE_OFFSET_BITS=64 -DFUSE_USE_VERSION=26
#lfs_LDFLAGS=-lfuse -lssl -lcrypto
lfs_LDADD=-lfuse -lcrypto
lfs_SOURCES=main.c logger.h logger.c container.c directory.c global.h inode.h lfs.h uthash.h  util.h container.h directory.h inode.c lfs.c util.c
