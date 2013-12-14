#include "directory.h"

inode_t * dir_get_inode(char *path){
  //use path to find the target inode
}


uint32_t dir_get_inode_id(dir_t *dir, char *filename, uint32_t *inode_id) {	
  uint32_t i; 
  for( i = 0; i != dir->num; i++ ) {
    if(!strcmp(dir->records[i]->filename,filename)) {
      *inode_id = dir->records[i]->inode_id; 
      return 0; 
    }
  }
  return -1;
}

dir_t * open_root_dir() {
  uint32_t cid;
  uint32_t seg_offset;
  inode_t *inode; 
  dir *ret_dir; 
  uint32_t root_addr = imap->records[0]->inode_addr;
  //calculate container_id and seg offset of root directory inode; 
  cid = root_addr/CONTAINER_SIZE;
  seg_offset = root_addr%CONTAINER_SIZE/BLK_SIZE;
  
  if( cid == cur_inode_container->header->container_id ) {        //if inode is already in memory
    inode = (inode_t*)(cur_inode_container->buf + seg_offset);    //read it from memory
  } else {                                                        //if not, read container to memory
    container_read( cur_inode_container, cid);                    //target container should be on disk
    inode = (inode_t*)(cur_inode_container->buf + seg_offset);    
  }
  if( inode->type != DIRECTORY ) {
    printf("not a directory");  
    return NULL;
  }
    
  uint32_t blk_num = inode->file_size/BLK_SIZE; 
  uint32_t dir_data_addr; 
  uint32_t i;
  char* dir_data_buf = malloc( BLK_SIZE * blk_num ); 
  for(i = 0; i != blk_num; i++) {
    dir_data_addr = inode->direct_blk[i];
    cid = dir_data_addr/CONTAINER_SIZE;
    seg_offset = dir_data_addr%CONTAINER_SIZE/BLK_SIZE;
    if( cid != cur_inode_container->header->container_id ) {
      container_read( cur_inode_container, cid);
    }
    memcpy(dir_data_buf + i * BLK_SIZE, cur_inode_container->buf + seg_offset, BLK_SIZE); 
  }
  ret_dir->records = (dir_record_t*)dir_data_buf;
  int cnt;
  for(i = 0; i != blk_num * BLK_SIZE/sizeof(dir_record_t); i++) {
    if( strlen(ret_dir->records[i]->filename) > 0) {
      cnt++; 
    }  
  }
  ret_dir->num = cnt; 
  return ret_dir; 
}


dir_t *open_cur_dir(){
}

dir_t *open_parent_dir(){
}


uint32_t dir_add_entry( dir_t *dir, const char* filename, uint32_t inode_id) {
  uint32_t origin_num = *dir->num;
  //check existence first
  strcpy(dir_records[origin_num].filename, filename);
  dir_records[origin_num].inode_id = inode_id;
  *dir->num++;  
}

uint32_t dir_remove_entry( dir_t *dir, const char* filename) {
  uint32_t origin_num = *dir->num;
  //check existence first, if exist delete it add move all entries after it. if not show does't exist. 
}
/*
dir_t *open_dir( const char * path) {
  if( !strcmp( path, "/") ) {
    return open_root_dir(); 
  } else if( !strcmp( path, "./") ) {
  } else if( !strcmp( path, "../") ) {
  } else {
    char *p = path; 
    while
  }
}
*/
// obtain the map from by using the path
int get_name_inodenum_map (char *path, struct dirmap_entry_t *map[])
{	
  // TODO
  return SUCCESS;
}

//get the parent path from current path
char* get_parentpath(const char *path)
{
	if (path == "/")
	    return NULL;

        char *p_end;
	int length = 0;
 	char *parent_path;
        p_start = path;

        while(*p_end != '\0') {
		length++;
                p_end++;
        }

        while(*p_end != '/')
        {
		length--;
                p_end--;
        }

	parent_path = malloc (length);
	memcpy (parent_path, path, length);
        return parent_path;
}

// obtain inode id from the map
int get_inodeid_from_map (char *fname, struct dirmap_entry_t *map[], 
	uint32_t size, uint32_t *pinode_id)
{
	int index;
	int flag = FAIL;
	for (index = 0; index < size; index++) 
	{
		if (map[index]->name == fname)
		{
			*pinode_id = map[index]->inode_num;
			flag = SUCCESS;
			break;
		}
	}
	return flag;
}
