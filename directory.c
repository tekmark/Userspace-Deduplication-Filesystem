#include "directory.h"
#include "global.h"

uint32_t dir_get_inode(const char *path, inode_t * inode){
    //use path to find the target inode
    dir_t * cur_dir = open_cur_dir();
    uint32_t inode_id = 0;
    uint32_t ret;
    ret = get_inode_id_from_filename ((char *)path, cur_dir, &inode_id);
    if (ret == FAIL) {
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
  dir_t *ret_dir = malloc (sizeof (dir_t)); 
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
    memset (ret_dir, 0, sizeof(dir_t));
    
    // get file recipe
    // find segments from file recipe, and put into a buffer
    uint32_t blk_num = get_division_result (
        lfs_info->cur_inode->file_size, BLK_SIZE);
    uint32_t dir_data_addr, i;
    char * dir_data_buf = malloc (BLK_SIZE * blk_num);
    for (i = 0; i < blk_num; i++) {
        //? fetch the address from direct block
        //  or fetch from file receipe 
	dir_data_addr = lfs_info->cur_inode->direct_blk[i];
        printf("open_cur_dir(): dir data seg #%u, addr:%x\n",i, dir_data_addr); 
        uint32_t container_size = CONTAINER_SIZE;
        uint32_t blk_size = BLK_SIZE;
        uint32_t cid = (dir_data_addr - blk_size) / container_size;
        
        seg_offset = ((dir_data_addr - blk_size) % container_size) / blk_size;
        printf("open_cur_dir: container_id: %u, segment_offset %u\n", cid, seg_offset); 
        if (cid != lfs_info->cur_container->header->container_id)  {
	   container_read (lfs_info->cur_container, cid);
           printf("open_cur_dir: required container is in disk, load it to mem\n"); 
        }
        
        uint32_t index,k;

        memcpy (dir_data_buf + i * BLK_SIZE, 
            lfs_info->cur_container->buf + seg_offset * BLK_SIZE, BLK_SIZE);

        printf ("************PRINT SEGMENT CONTENT OF CURRENT CONTAINER********\n");
        for (index = 0; index < 20; index++) {
           printf ("******************SEGMENT %u******************\n", index);
           for (k = 0; k < SEG_SIZE; k++) {
               printf ("%c", *(lfs_info->cur_container->buf + index * SEG_SIZE +k));
           }
           printf ("\n");
        }
    }
    // create a structure holding the directory
    ret_dir->records = (dir_record_t *)dir_data_buf;
    int cnt =0;
    for (i = 0; i < blk_num * BLK_SIZE / sizeof(dir_record_t); i++) {
	if (strlen(ret_dir->records[i].filename) > 0) {
	    cnt++;
        }
    }
    ret_dir->num = cnt;
    printf("open_cur_dir: ret dir's has %u entries\n", cnt); 
    return ret_dir;
}

dir_t *open_parent_dir(){
    return NULL;     
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
    printf("entry is added successfully, entry#%u\n", dir->num);  
    return flag;
}

uint32_t dir_commit_changes(dir_t *dir, inode_t *inode) {

    uint32_t i, j, ret;
    if (dir == NULL || inode == NULL) {
        ret = -1;
        return ret;
    }
    // 1:  clear all direct_blk in inode
    for (i = 0; i < DIRECT_BLK_NUM; i++) {
        inode->direct_blk[i] = 0;
    }

    uint32_t num_of_rec_per_seg = NUM_OF_RECORD_PER_SEG;
    uint32_t num_of_seg = get_division_result (dir->num, num_of_rec_per_seg);
    char *dir_data_seg_buf = malloc (SEG_SIZE);
    // 2: dd dir data into buf container seg by seg
    for (i = 0; i < num_of_seg; i++) {
        memset ((void *)dir_data_seg_buf, 0, SEG_SIZE);
        for (j = i * num_of_rec_per_seg; 
            (j < (i + 1) * num_of_rec_per_seg) && (j < dir->num);
            j++) {
            memcpy (dir_data_seg_buf + 
                (j % num_of_rec_per_seg) * sizeof (dir_record_t), 
                (char *)&dir->records[j], sizeof (dir_record_t));
        }
        //2.1: modify direct_blk in inode first
        inode->direct_blk[i] = BLK_SIZE + 
            CONTAINER_SIZE * lfs_info->buf_container->header->container_id  + 
            lfs_info->buf_container->offset;
        printf ("dir_commit_changes: inode->direct_blk[%u] is %x \n",
            i, inode->direct_blk[i]);
        // 2.2 then add segment one by one
        container_add_seg (lfs_info->buf_container, dir_data_seg_buf);
    }
    // 3. add inode segment to buf container
    char *inode_seg_buf = malloc (SEG_SIZE);
    memset ((void *)inode_seg_buf, 0, SEG_SIZE);
    memcpy (inode_seg_buf, (char *)inode, sizeof(inode_t));
    uint32_t inode_new_addr = BLK_SIZE +
        CONTAINER_SIZE * lfs_info->buf_container->header->container_id +
        lfs_info->buf_container->offset;
    container_add_seg (lfs_info->buf_container, inode_seg_buf); 
    
    uint32_t flag  = 0;
    // modify inode_map
    for (i = 0; i < MAX_INODE_NUM; i++) {
        if (lfs_info->imap->records[i].inode_id == inode->inode_id) {
            lfs_info->imap->records[i].inode_addr = inode_new_addr;
            //ret = 0;
            flag = 1;
            break;
        }
    }
    printf("container commit: inode_id: %u, inode addr: %x \n", inode->inode_id, 
           inode_new_addr);
    if (flag == 0) {
        lfs_info->imap->records[lfs_info->n_inode-1].inode_id = inode->inode_id;
        lfs_info->imap->records[lfs_info->n_inode-1].inode_addr = inode_new_addr;
    }
    int index = 0; 
    if (lfs_info->buf_container->header->container_id ==
        lfs_info->cur_container->header->container_id) {
        container_copy(lfs_info->cur_container, lfs_info->buf_container);
        printf ("dir_commit_changes: copy from buf container to cur container\n");
    }
     
    for( index = 0; index < CONTAINER_SIZE; index++){
      printf("%c", *(lfs_info->cur_container->buf+index)); 
    }
    free (inode_seg_buf);
    free (dir_data_seg_buf);
    ret = 0;
    return ret;
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

void print_dir_data (dir_t * dir) {
    uint32_t index;
    for (index = 0; index < dir->num; index++) {
        if (strlen(dir->records[index].filename) > 0) {
            printf ("dir_print: filename: %s, inode_id: %u\n", 
                dir->records[index].filename,
                dir->records[index].inode_id);
        }
    }
}

//get the parent path from current path
char* get_parentpath(const char *path)
{
	if (!strcmp ((char *)path, "/"))
	    return "/";

        char *p_end;
	int length = 0;
 	char *parent_path;

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
    char *filename = get_filename((const char *)fname);
    printf ("get_inode_id_from_fn: filename is: %s\n", filename);
    printf ("get_inode_id_from_fn: dir_data->num: %u\n", dir_data->num);
    for (index = 0; index < dir_data->num; index++) {
        printf ("get_inode_id_From_fn: filename: %s, id: %u\n", 
            dir_data->records[index].filename, 
            dir_data->records[index].inode_id);
        if (strcmp (dir_data->records[index].filename, filename) == 0) {
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
