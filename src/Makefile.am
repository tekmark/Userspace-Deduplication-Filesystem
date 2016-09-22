#flags
AM_CFLAGS=-g -Wall
#AM_LDFLAGS=-lfuse -lcrypt
bin_PROGRAMS=lfs
lfs_CFLAGS=-D_FILE_OFFSET_BITS=64 -DFUSE_USE_VERSION=26
#lfs_LDFLAGS=-lfuse -lssl -lcrypto
lfs_LDADD=-lfuse -lcrypto
lfs_SOURCES=main.c test.h test.c lfs_utils.h lfs_utils.c simpleclogger.h simpleclogger.c lfs_ctx.h lfs_ctx.c lfs_lowlevel_ops.h lfs_lowlevel_ops.c file_recipe.h file_recipe.c lfs_operations.h lfs_operations.c inodemap.h inodemap.c segment.h segment.c fingerprint.h fingerprint.c namespace.h namespace.c lfs_reserved.h lfs_reserved.c conatainer_types.h lfs_stat.h lfs_stat.c container_utility.h container_utility.c container_header.h container_header.c constants.h constants.c logger.h container.c directory.c global.h inode.h lfs.h uthash.h  util.h container.h directory.h inode.c lfs.c util.c
