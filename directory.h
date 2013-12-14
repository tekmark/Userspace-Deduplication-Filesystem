#ifndef DIRECTORY__H
#define DIRECTORY__H

#include <stdint.h>

#define MAX_FILENAME_LENGTH 256

#define SUCCESS 0
#define FAIL    -1

typedef struct {                      //260bytes per entry, 4096/260 = 15
  char filename[MAX_FILENAME_LENGTH];     //255 + 1
  uint32_t inode_id;                  //4 byte; 
} dir_record_t; 

typedef struct {
  uint32_t num; 
  dir_record_t *records; 
} dir_t; 

// public method: get inode id by the path of a file
// Todo: disambiguate the path is a directory path or file path
int dir_get_inode_id(dir_t *dir, char *filename, uint32_t *inode_id);

// obtain path of parent folder
char* get_parentpath(const char *path);

// obtain the map from by using the path
int get_name_inodenum_map (char *path, struct dirmap_entry_t[] *map);

// obtain inode id from the map, if not exist return -1, succeed return 0
int get_inodeid_from_map (char *fname, struct dirmap_entry_t[] *map, 
	uint32_t size, uint32_t *pinode_id);

#endif
