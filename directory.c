#include "directory.h"
#include "global.h"

uint32_t dir_get_inode(const char *path, inode_t * inode){
    //use path to find the target inode
    dir_t * cur_dir = open_cur_dir();
    uint32_t inode_id = 0;
    uint32_t ret;
    ret = get_inode_id_from_filename (path, cur_dir, &inode_id);
    if (ret == FAIL) {
        printf ("Fail obtaining inode id from filename. \n");
        return ret;
    }

    inode = get_inode_from_inode_id (inode_id);
    if (inode == NULL) {
        ret = FAIL;
    }
    
    return ret;
}

dir_t * open_root_dir() {
  uint32_t cid;
  uint32_t seg_offset;
  inode_t *inode; 
  dir_t *ret_dir; 
  uint32_t root_addr = lfs_info->imap->records[0].inode_addr;
  //calculate container_id and seg offset of root directory inode; 
  cid = root_addr/CONTAINER_SIZE;
  seg_offset = root_addr%CONTAINER_SIZE/BLK_SIZE;
  
  if( cid == lfs_info->cur_container->header->container_id ) {        //if inode is already in memory
    inode = (inode_t*)(lfs_info->cur_container->buf + seg_offset);    //read it from memory
  } else {                                                        //if not, read container to memory
    container_read(lfs_info->cur_container, cid);                    //target container should be on disk
    inode = (inode_t*)(lfs_info->cur_container->buf + seg_offset);    
  }
  if( inode->inode_type != DIRECTORY ) {
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
    if( cid != lfs_info->cur_container->header->container_id ) {
      container_read( lfs_info->cur_container, cid);
    }
    memcpy(dir_data_buf + i * BLK_SIZE, lfs_info->cur_container->buf + seg_offset, BLK_SIZE); 
  }
  ret_dir->records = (dir_record_t*)dir_data_buf;
  int cnt;
  for(i = 0; i != blk_num * BLK_SIZE/sizeof(dir_record_t); i++) {
    if( strlen(ret_dir->records[i].filename) > 0) {
      cnt++; 
    }  
  }
  ret_dir->num = cnt; 
  return ret_dir; 
}

dir_t *open_cur_dir(){
    uint32_t seg_offset;
    if (lfs_info->cur_inode == NULL)  {
        // obtain current directory inode from disk
      printf("open_cur_dir: cur_inode == NULL\n");
      return NULL; 
    }

    // Assume we already know the inode of current directory
    //inode_t *cur_dir_inode = lfs_info->cur_inode;
    dir_t * ret_dir = malloc (sizeof(dir_t)); 
    
    // get file recipe
    // find segments from file recipe, and put into a buffer
    uint32_t blk_num = get_division_result (
        lfs_info->cur_inode->file_size, BLK_SIZE);
    printf("open_cur_dir: block_num result: %u\n", blk_num); 
    uint32_t dir_data_addr, i;
    char * dir_data_buf = malloc (BLK_SIZE * blk_num);
    for (i = 0; i < blk_num; i++) {
        //? fetch the address from direct block
        //  or fetch from file receipe 
	dir_data_addr = lfs_info->cur_inode->direct_blk[i];
        printf("open_cur_dir: direct_blk#%u, addr:%x\n",i, dir_data_addr); 
        int32_t cid = 0; // (int32_t)dir_data_addr/(int32_t)CONTAINER_SIZE;
        printf("open_cur_dir: container_size = %x\n", CONTAINER_SIZE);
        seg_offset = dir_data_addr % CONTAINER_SIZE / BLK_SIZE;
        printf("open_cur_dir: container_id: %u, segment_offset %u\n", cid, seg_offset); 
        if (cid != lfs_info->cur_container->header->container_id)  {
	   container_read (lfs_info->cur_container, cid);
           printf("open_cur_dir: container is not in mem, load it to mem\n"); 
        }
        memcpy (dir_data_buf + i * BLK_SIZE, 
            lfs_info->cur_container->buf + seg_offset * BLK_SIZE, BLK_SIZE);
    }
    // create a structure holding the directory
    ret_dir->records = (dir_record_t *)dir_data_buf;
    int cnt;
    for (i = 0; i < blk_num * BLK_SIZE / sizeof(dir_record_t); i++) {
	if (strlen(ret_dir->records[i].filename) >0) {
	    cnt++;
        }
    }
    ret_dir->num = cnt;
    printf("open_cur_dir: ret container's has %u entries\n", cnt); 
    return ret_dir;
}

dir_t *open_parent_dir(){
      
}


uint32_t dir_add_entry( dir_t *dir, const char* filename, uint32_t inode_id) {
    uint32_t index;
    uint32_t flag = SUCCESS;
    for (index = 0; index < dir->num; index++) {
        if (strcmp(dir->records[index].filename, filename) == 0) {
            printf ("Fail adding entry to the directory \n");
            flag = FAIL;
            break;
        }
    }
    
    if (flag == FAIL) {
        return flag;
    }

    uint32_t entry_num = dir->num;
    strcpy(dir->records[entry_num].filename, filename);
    dir->records[entry_num].inode_id = inode_id;
    dir->num++; 
    return flag;
}

uint32_t dir_remove_entry( dir_t *dir, const char* filename) {
    uint32_t index;
    uint32_t entry_index;
    uint32_t flag = FAIL;
    for (index = 0; index < dir->num; index++) {
        if (strcmp (dir->records[index].filename, filename) == 0) {
            entry_index = index;
            flag = SUCCESS;
        }
    }
    
    if (flag == FAIL) {
        // entry doesn't exist
        return flag;
    }
    
    for (index = entry_index; index < dir->num - 1; index++) {
        strcpy(dir->records[index].filename,dir->records[index+1].filename);
        dir->records[index].inode_id = dir->records[index+1].inode_id;
    }
    dir->num--;
    return flag;
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

//get the parent path from current path
char* get_parentpath(const char *path)
{
	if (path == "/")
	    return NULL;

        char *p_end;
	int length = 0;
 	char *parent_path;
        char *p_start = path;

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
uint32_t get_inode_id_from_filename (char *fname, dir_t * dir_data,
    uint32_t *pinode_id) {
    uint32_t flag = FAIL;
    uint32_t index;
    for (index = 0; index < dir_data->num; index++) {
        if (strcmp (dir_data->records[index].filename, fname) == 0) {
	    *pinode_id = dir_data->records[index].inode_id;
	    flag = SUCCESS;
	    break;
	}
    }
    return flag;
}

inode_t * get_inode_from_inode_id (uint32_t inode_id) {
    uint32_t index;
    uint32_t inode_addr = 0;
    char * inode_buf = malloc (sizeof (inode_t));
    inode_t * inode;
    for (index = 0; index < MAX_INODE_NUM; index++) {
        if (lfs_info->imap->records[index].inode_id == inode_id) {
            inode_addr = lfs_info->imap->records[index].inode_addr;
            break;
        }
    }
    
    if (inode_addr == 0) {
        printf ("cannot find inode address from inode_id.\n");
        return NULL;
    }

    uint32_t cid = inode_addr / CONTAINER_SIZE;
    uint32_t seg_offset = inode_addr % CONTAINER_SIZE / BLK_SIZE;
    if (cid != lfs_info->cur_container->header->container_id) {
        container_read (lfs_info->cur_container, cid);
    }
    memcpy (inode_buf, 
        lfs_info->cur_container->buf + seg_offset * SEG_SIZE,
        sizeof(inode_t));
    inode = (inode_t *)inode_buf;
    return inode;
}
