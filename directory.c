#include "directory.h"

// get the inode of a path
uint32_t dir_get_inode(const char *path, inode_t * inode){
    // get current directory data
    dir_t * cur_dir = open_cur_dir();
    print_dir_data (cur_dir);
    uint32_t inode_id = 0;
    uint32_t ret;
    // get inode_id based on current filename
    ret = get_inode_id_from_filename ((char *)path, cur_dir, &inode_id);
    if (ret == FAIL) {
        printf ("get_inode_id_From_filename: return false, no such inode\n");
        return ret;
    }

    // get inode structure based on inode id
    inode = get_inode_from_inode_id (inode_id);
    if (inode == NULL) {
        ret = FAIL;
    }
    
    return ret;
}

dir_t *open_cur_dir(){
    uint32_t seg_offset;
    if (lfs_info->cur_inode == NULL)  {
      // obtain current directory inode from disk
      printf("open_cur_dir: no cur_inode \n");
      return NULL; 
    }
   
    //create dir struct for return statement
    dir_t * ret_dir = malloc (sizeof(dir_t));
    memset (ret_dir, 0, sizeof(dir_t));
    
    // calculate how many blocks the directory data have
    uint32_t blk_num = get_division_result (
        lfs_info->cur_inode->file_size, c_blk_size);
    
    printf("open_cur_dir: block_num result: %u\n", blk_num); 

    uint32_t dir_data_addr, i;
    // create a data buffer with file size
    char * dir_data_buf = malloc (c_blk_size * blk_num);
    // loop by each block 
    for (i = 0; i < blk_num; i++) {
        // fetch the address of dir data segment from direct block of inode
	    dir_data_addr = lfs_info->cur_inode->direct_blk[i];
        
        printf("open_cur_dir(): dir data seg #%u, addr:%x\n",i, dir_data_addr); 
        
        // calculate container id of required dir data segment
        uint32_t cid = (dir_data_addr - c_blk_size) / c_container_size;
        // calculate segment offset of required dir data segment
        seg_offset = ((dir_data_addr - c_blk_size) % c_container_size) / c_blk_size;
        printf("open_cur_dir: container_id: %u, segment_offset %u\n", cid, seg_offset); 
        // if cid not equal to current container id, read from disk
        // TODO: if buf_container id == cur_container id, read from buf_container
        
        if (cid == lfs_info->buf_container->header->container_id) {
          printf("open_cur_dir: read data from buf container\n"); 
          memcpy( dir_data_buf + i * c_blk_size, 
                  lfs_info->buf_container->buf + seg_offset * c_blk_size, c_blk_size ); 
        } else if (cid != lfs_info->cur_container->header->container_id) {
	      container_read (lfs_info->cur_container, cid);
          printf("open_cur_dir: required container is in disk, load it to mem\n"); 
          memcpy (dir_data_buf + i * c_blk_size, 
                  lfs_info->cur_container->buf + seg_offset * c_blk_size, c_blk_size );
        } else {                                 //if target blk is in cur container
           memcpy (dir_data_buf + i * c_blk_size, 
                  lfs_info->cur_container->buf + seg_offset * c_blk_size, c_blk_size );
        }
    }
    // create a structure holding the directory
    ret_dir->records = (dir_record_t *)dir_data_buf;
    int cnt =0;
    // calculate the counts of records in dir data
    for (i = 0; i < c_container_blk_num * c_blk_size / sizeof(dir_record_t); i++) {
	if (strlen(ret_dir->records[i].filename) >0) {
        printf("filename %s\n", ret_dir->records[i].filename); 
	    cnt++;
        }
    }
    ret_dir->num = cnt;
    printf("open_cur_dir: ret dir's has %u entries\n", cnt); 
    print_dir_data(ret_dir); 
    return ret_dir;
}

dir_t *open_parent_dir(){
    return NULL; 
}

// add an entry to the current directory structure (in mem)
// dir: current directory data
// filename: filename to be added to the directory data
// inode_id: inode id of the filename to be added to the directory data
uint32_t dir_add_entry( dir_t *dir, const char* filename, uint32_t inode_id) {
    uint32_t index;
    uint32_t flag = SUCCESS;
    // compare filename with existed filenames in the directory data
    for (index = 0; index < dir->num; index++) {
        if (strcmp(dir->records[index].filename, filename) == 0) {
            printf ("Fail adding entry to the directory \n");
            flag = FAIL;
            break;
        }
    }
    
    // if filename exists, return
    if (flag == FAIL) {
        return flag;
    }
    // add an entry to dir data, increase dir entry num by one
    uint32_t entry_num = dir->num;
    strcpy(dir->records[entry_num].filename, filename);
    dir->records[entry_num].inode_id = inode_id;
    dir->num++;
    printf("entry is added successfully, entry#%u\n", dir->num);  
    return flag;
}

// write changes of directory data, along with its directory inode
// to the container, and modify inode_map
uint32_t dir_commit_changes(dir_t *dir, inode_t *inode) {

    uint32_t i, j, ret;
    if (dir == NULL || inode == NULL) {
        printf("dir_commit_changes: bad input arguements\n"); 
        ret = -1;
        return ret;
    }
    // 1:  clear all direct_blk entries in inode
    for (i = 0; i < DIRECT_BLK_NUM; i++) {
        inode->direct_blk[i] = 0;
    }

    //calculate how many segments the directory data have
    uint32_t num_of_rec_per_seg = NUM_OF_RECORD_PER_SEG;
    uint32_t num_of_seg = get_division_result (dir->num, num_of_rec_per_seg);
    char *dir_data_seg_buf = malloc (c_seg_size);

    // 2: add dir data into buf container seg by seg

    //----------------------------------------------------------------------
    // TODO: the current method of adding entries to segment will cause error
    // when there are more than 15 entries to be added. A better way is to create
    // a buffer with directory data size (e.g. 2 segments), put all entries into 
    // the buffer, and then cut it segment by segment and add to container.
    //-----------------------------------------------------------------------
    for (i = 0; i < num_of_seg; i++) {
        // for each segment, add at most 15 entry into the segment buffer
        memset ((void *)dir_data_seg_buf, 0, c_seg_size);
        for (j = i * num_of_rec_per_seg; 
            (j < (i + 1) * num_of_rec_per_seg) && (j < dir->num);
            j++) {
            // add entries to the segment buffer one by one
            memcpy (dir_data_seg_buf + 
                (j % num_of_rec_per_seg) * sizeof (dir_record_t), 
                (char *)&dir->records[j], sizeof (dir_record_t));
        }
        // 2.1: modify direct_blk in inode first, we can get the address of the 
        // current dir data segment because we write dir data segment into container
        // first. 
        inode->direct_blk[i] = c_blk_size + 
            c_container_size * lfs_info->buf_container->header->container_id  + 
            lfs_info->buf_container->seg_offset * c_seg_size;
        printf ("dir_commit_changes: inode->direct_blk[%u] is %x \n",
            i, inode->direct_blk[i]);
        // 2.2 then add dir data segment into container one by one
        container_add_seg (lfs_info->buf_container, dir_data_seg_buf);
    }
    // 3. add inode segment to buf container
    char *inode_seg_buf = malloc (c_seg_size);
    memset ((void *)inode_seg_buf, 0, c_seg_size);
    memcpy (inode_seg_buf, (char *)inode, sizeof(inode_t));
    // calculate the new address of dir inode
    uint32_t inode_new_addr = c_blk_size +
        c_container_size * lfs_info->buf_container->header->container_id +
        lfs_info->buf_container->seg_offset * c_seg_size;
    container_add_seg (lfs_info->buf_container, inode_seg_buf); 
    
    uint32_t flag  = 0;
    // modify the dir inode entry in inode_map 
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
    // if the dir inode entry doesn't exist in the inode map (meaning the directory
    // is new created), add an entry into the inodemap.
    if (flag == 0) {
        lfs_info->imap->records[lfs_info->n_inode-1].inode_id = inode->inode_id;
        lfs_info->imap->records[lfs_info->n_inode-1].inode_addr = inode_new_addr;
    }
    int index = 0; 
    // if cur container id == buf container id, copy buf container to cur_container
    // TODO: save this step by making modifications in open_cur_dir()
    if (lfs_info->buf_container->header->container_id ==
        lfs_info->cur_container->header->container_id) {
        container_copy(lfs_info->cur_container, lfs_info->buf_container);
        printf ("dir_commit_changes: copy from buf container to cur container\n");
    }
    
    // print all data in cur_container
    for( index = 0; index < c_container_size; index++){
      printf("%c", *(lfs_info->cur_container->buf+index)); 
    }
    free (inode_seg_buf);
    free (dir_data_seg_buf);
    ret = 0;
    return ret;
}

// remove an entry in directory
uint32_t dir_remove_entry( dir_t *dir, const char* filename) {
    uint32_t index;
    uint32_t entry_index;
    uint32_t flag = FAIL;
    // try to find the entry in the dir records
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
    
    // shift entries after the removed entry to the left
    for (index = entry_index; index < dir->num - 1; index++) {
        strcpy(dir->records[index].filename,dir->records[index+1].filename);
        dir->records[index].inode_id = dir->records[index+1].inode_id;
    }
    dir->num--;
    return flag;
}

// get the parent path from current path
// function not tested yet
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

    while(*p_end != '/') {
		length--;
        p_end--;
    }

	parent_path = malloc (length);
	memcpy (parent_path, path, length);
        return parent_path;
}

// obtain inode id from the file name
uint32_t get_inode_id_from_filename (char *fname, dir_t * dir_data,
                                     uint32_t *pinode_id) {
    uint32_t flag = FAIL;
    uint32_t index;
    const char *filename = get_filename((const char *)fname);
    printf ("get_inode_id_from_fn: filename is: %s\n", filename);
    printf ("get_inode_id_from_fn: dir_data->num: %u\n", dir_data->num);
    // compare the filename with each entry of directory data records
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

// get inode from inode id by looking up in the inode map
inode_t * get_inode_from_inode_id (uint32_t inode_id) {
    uint32_t index;
    uint32_t inode_addr = 0;
    char * inode_buf = malloc (sizeof (inode_t));
    inode_t * inode;
    // look up in the inode map
    for (index = 0; index < MAX_INODE_NUM; index++) {
        if (lfs_info->imap->records[index].inode_id == inode_id) {
            inode_addr = lfs_info->imap->records[index].inode_addr;
            break;
        }
    }
    
    // entry not found
    if (inode_addr == 0) {
        printf ("cannot find inode address from inode_id.\n");
        return NULL;
    }

    // calculate container id and segment offset from the inode address
    uint32_t cid = inode_addr / c_container_size;
    uint32_t seg_offset = inode_addr % c_container_size / c_blk_size;
    // if cid not equal to current id, fetch container from disk
    // TODO: if buf container is has the same cid as the required one, 
    // read directly from buf container (saves the resource of copying from
    // buf container to cur container)
    if (cid != lfs_info->cur_container->header->container_id) {
        container_read (lfs_info->cur_container, cid);
    }
    // copy from current container to inode buf
    memcpy (inode_buf, 
        lfs_info->cur_container->buf + seg_offset * c_seg_size,
        sizeof(inode_t));
    // cast buf to inode type
    inode = (inode_t *)inode_buf;
    return inode;
}

void print_dir_data (dir_t * dir) {
    uint32_t index;
    for (index = 0; index < 5; index++) {
       // if (strlen(dir->records[index].filename) > 0) {
            printf ("dir_print: filename: %s, inode_id: %u\n", 
                dir->records[index].filename,
                dir->records[index].inode_id);
       // }
    }

}
