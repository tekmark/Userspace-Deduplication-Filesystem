#include"lfs.h"
/*
const uint32_t c_blk_size = 4096;                 //block size is 4K
const uint32_t c_seg_size = 4096;           //segment equals block size
const uint32_t c_container_blk_num = 8;
const uint32_t c_container_seg_num = 8;
const uint32_t c_container_size = 4096 * 1024;
const uint32_t c_max_container_num = 1024;

*/
static int lfs_getattr(const char *path, struct stat *stbuf){
  printf ("!!Entry: lfs_getattr function\n"); 
  int res = 0;
 
  memset( stbuf, 0, sizeof(struct stat) );
  // if path is root
  if( strcmp(path, lfs_info->cur_path) == 0 ){
    stbuf->st_ino = lfs_info->cur_inode->inode_id;         
    stbuf->st_mode = lfs_info->cur_inode->mode;
    stbuf->st_size = lfs_info->cur_inode->file_size;
    stbuf->st_nlink = lfs_info->cur_inode->link;
    stbuf->st_ctime = lfs_info->cur_inode->ctime;
    stbuf->st_mtime = lfs_info->cur_inode->ctime; 
    res = 0;
    printf ("!! Exit: lfs_getattr function with exit code %u\n", res);
  } 
  else { 
    inode_t inode;
    if (strcmp (path, lfs_info->par_path) == 0) {
      printf ("++++++++++lfs_getattr, path = parent\n");
      res = dir_get_inode ("..", &inode);
    }
    else {

    // get inode of the path
    // when initializing the system, path = "/", get inode of root directory
    // when "mkdir aaa", path = "/aaa", get inode of the directory aaa
    // when "touch bbb", path = "/bbb", get inode of the file bbb
      res = dir_get_inode( path, &inode);
    }
    printf ("****************DEBUG***************************\n");
    printf ("res = %u, path = %s, inode->inode_id= %u\n", res, path, 
        inode.inode_id);
    if (res == 0) {  // if inode exists
      /* TODO: set the stbuf->st_mode based on the type of the inode 
         (file or directory) */
      stbuf->st_ino = inode.inode_id; 
      stbuf->st_mode = inode.mode;
      stbuf->st_mtime = inode.mtime; 
      stbuf->st_nlink = 1;
      stbuf->st_size = inode.file_size;
      printf ("!! Exit: lfs_getattr(), find file %s, filesize: %x\n", path, inode.file_size);
    } else {  // if inode doesn't exist
      res = -ENOENT;
      printf ("!! Exit: lfs_getattr(), no such file  %s\n", path);
    }
  }
  return res;
}

static int lfs_mkdir(const char *path, mode_t mode){
  printf("lfs_mkdir: enter \n"); 
  
  // configure the inode of new created directory
  lfs_info->n_inode++;
  inode_t *new_dir = malloc(sizeof(inode_t)); 
  // TODO: inode_id should be equal to lfs_info->n_inode
  new_dir->inode_id = lfs_info->n_inode;
  new_dir->inode_type = DIRECTORY;
  time(&new_dir->ctime);
  time(&new_dir->mtime); 
  new_dir->file_size = 0x1000;
  new_dir->mode = S_IFDIR | 0777;
  mode = new_dir->mode; 
  
  // add an entry to current directory structure cur_dir
  dir_t *cur_dir = open_cur_dir();
  print_inodemap (lfs_info->imap);
  print_dir_data (cur_dir);
  const char *new_dirname = get_filename (path);
  inode_t *cur_dir_inode = lfs_info->cur_inode;
  dir_add_entry( cur_dir, new_dirname, new_dir->inode_id);
  // commit changes to the container about current directory data
  // and current directory inode
  dir_commit_changes (cur_dir, cur_dir_inode);
  
  //dir_t *test_dir = open_cur_dir();
 
  // create a directory structure holding data of new created directory
  dir_t *dir_data = malloc (sizeof(dir_t));
  dir_data->num = 0; //init
  dir_data->records = malloc( 20 * sizeof(dir_record_t));
  // add two entries to new directory data( .. and . )
  dir_add_entry (dir_data, "..", cur_dir_inode->inode_id); 
  dir_add_entry (dir_data, ".", new_dir->inode_id);
  // commit changes to the container about new created directory data
  // and new created directory inode
  dir_commit_changes (dir_data, new_dir);
  
  // print
  printf ("*************lfs_mkdir: Print inode_map ****************** \n");
  print_inodemap(lfs_info->imap); 
  printf ("*************lfs_mkdir: Print current dir data:************ \n");
  print_dir_data (cur_dir);
  printf ("************lfs_mkdir: Print new dir data: *****************\n");
  print_dir_data (dir_data);

  //free (cur_dir);
  free (new_dir);
  free (dir_data);
  return 0; 
}

static int lfs_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
                       off_t offset, struct fuse_file_info *fi){
  printf("lfs_readdir: enter\n"); 
  (void) offset;
  (void) fi;
  //struct file_inode_hash *s;
  
  //TODO no / situation
  //if (strcmp(path, "/") != 0) {
  //    printf ("!! Exit: lfs_readdir with exit code %u\n", -ENOENT);
  //    return -ENOENT;
  //}
  if (lfs_info->cur_inode == NULL) {
    printf ("!!Exit: lfs_readdir exit with ret number %u\n", -ENOENT);
    return -ENOENT;
  }
  
  // get the current directory data from container
  dir_t *dir = open_cur_dir();
  uint32_t i = 0; 
  //fill all file names in current dir to buf
  for( i = 0; i != dir->num; i++) {
    filler(buf, dir->records[i].filename, NULL, 0);
  } 
  printf ("!! Exit: lfs_readdir with exit code 0 \n");
  return 0; 
}

static int lfs_create(const char *path, mode_t mode, struct fuse_file_info *fi){
  printf("Entry: lfs_create()\n");
  inode_t temp_ino;
  if( !dir_get_inode( path, &temp_ino ) ) { // if inode of path already exists
    printf("Error: inode already exists!\n"); 
    return -EEXIST;
  } else {
    // configure inode of new created file
    inode_t *new_inode = malloc (c_seg_size);
    memset (new_inode, 0, c_seg_size);
    lfs_info->n_inode++;

    new_inode->inode_id = lfs_info->n_inode;
    new_inode->inode_type = REGULAR_FILE;
    new_inode->file_size = 0; 
    new_inode->link = 1; 
    new_inode->owner = 1;
    new_inode->mode = S_IFREG | 0777;
    time(&new_inode->ctime); 
    time(&new_inode->mtime); 
    // TODO: file recipe address in this inode might not need to be set
    // at this time, because no file content exists. Probably file recipe
    // should be created and write to container when we write sth to file
   
    // add an entry to the current directory about the new created file
    const char * filename = get_filename (path);
    dir_t * cur_dir = open_cur_dir();
    dir_add_entry( cur_dir, filename , new_inode->inode_id);
    // commit changes to container about the current directory data
    // and current directory inode
    dir_commit_changes (cur_dir, lfs_info->cur_inode);
    
    // commit changes of new created file inode 
    // (at this time no file data or file recipe) to container
    // TODO: might consider create a function
    // file_commit_changes (filedata, file_inode, file_recipe) to write
    // changes of file-related info to container
    //char *new_inode_seg = malloc (c_seg_size);
    //memset (new_inode_seg, 0, c_seg_size);
    //memcpy (new_inode_seg, new_inode, sizeof(inode_t));
    // update inode map about the new created file inode
    lfs_info->imap->records[lfs_info->n_inode-1].inode_id = new_inode->inode_id;
    lfs_info->imap->records[lfs_info->n_inode-1].inode_addr =  c_blk_size
                       + lfs_info->buf_container->header->container_id * c_container_size
                       + lfs_info->buf_container->seg_offset * c_blk_size ;
    printf ("&&&&&&&&&&&&&&&&&&&&DEBUGDEBUG *********************\n");
    print_inodemap (lfs_info->imap);
    new_inode->file_recipe = 0; 
    //new_inode->file_recipe = lfs_info->buf_container->header->container_id * c_container_size
    //                  + lfs_info->buf_container->seg_offset * c_blk_size
    //                  + c_blk_size + c_blk_size;
    printf("lfs_create: file_recipe addr %x\n", new_inode->file_recipe); 
    container_add_seg (lfs_info->buf_container, (void*)new_inode);
    //file_recipe_t *file_recipe = malloc( c_seg_size );
    //memset( file_recipe, 0 , c_seg_size); 
    //container_add_seg (lfs_info->buf_container, (void*)file_recipe); 
    free (new_inode);
    //free (file_recipe); 
  }
  
  return 0; 
}

static int lfs_access (const char *path, int mask) {
  printf ("++++++++++++++++++++++++++++++++++Entry: lfs_access()\n");
  printf ("++++++++++++++++++++++++++++++++++Path = %s\n", path);
  if (strcmp (path, lfs_info->cur_path) == 0) {
    //get_inode_from_inode_id (lfs_info->cur_inode, 1);
    printf ("+++++++++++++++++++++++++++++++++++++++we do not change cur inode\n");
    return 0;
  }
  
  inode_t *inode = (inode_t *)malloc (sizeof (inode_t));
  memset (inode, 0, sizeof (inode_t));
  uint32_t ret;
  printf ("++++++++++++before comapring, parentpath is %s\n", lfs_info->par_path);
  //char * parent_path = get_parentpath (lfs_info->cur_path);
  //printf ("+++++++++++++++++++++++Parent Path == %s\n", parent_path);
  if (strcmp (lfs_info->par_path, path) == 0) {
    printf ("+++++++++++++++++++++++++++++++++PARENT Path\n");
    ret = dir_get_inode ("..", inode);
    if (ret != 0) {
        printf ("Exit: Entry doesn't exist\n");
        return -ENOENT;
    }
    
    lfs_info->cur_inode = inode;
    strcpy (lfs_info->cur_path, lfs_info->par_path);
    printf ("++++++++++++++DEBUGGGGG: cur_path now is: %s\n", lfs_info->cur_path);
    char *new_parpath = get_parentpath (lfs_info->cur_path);
    strcpy (lfs_info->par_path, new_parpath);
    printf ("+++++++++++++after comparing and .., global par_path = %s, global cur_path = %s\n", lfs_info->par_path, lfs_info->cur_path);
    return ret;
  }
  else {
    printf ("+++++++++++++++++++++++++++++++++REGULAR Path\n");
    ret = dir_get_inode (path, inode);
  }
  if (ret != 0) {
    printf ("++++++++++++++++++++++++++++++Exit: lfs_access exit. inode not found\n");
    return -ENOENT;
  }
  if (inode->inode_type == DIRECTORY) {
    lfs_info->cur_inode = inode;
    //lfs_info->par_path = lfs_info->cur_path;
    //lfs_info->cur_path = path;
    strcpy (lfs_info->par_path, lfs_info->cur_path);
    strcpy (lfs_info->cur_path, path);
  }
  printf ("+++++++++++++++++++++++++++lfs_access: Set new cur_inode in lfs_info\n");
  printf ("+++++++++++++++++++++++++++lfs_create: new inode num is: %u, par_path is %s, cur_path is %s\n", lfs_info->cur_inode->inode_id, lfs_info->par_path, lfs_info->cur_path);
  printf ("++++++++++++++++++++++++++++Exit: lfs_access exit. inode found\n");
  return 0;
}

static int lfs_open(const char *path, struct fuse_file_info *fi){
  printf("lfs_open: enter\n"); 
  
  return 0; 
}

static int lfs_read(const char *path, char *buf, size_t size, off_t offset,
                    struct fuse_file_info *fi){
  printf("lfs_read: enter, input para: size = %u, offset=%u\n", (uint32_t)size, (uint32_t)offset); 
  uint32_t ret = 0;
  inode_t inode;
  ret = dir_get_inode(path, &inode);
  if ( ret != 0 ) {
    printf("lfs_read: cannot get inode by using dir_get_inode()\n"); 
    return -ENOENT; 
  }
  
  printf("lfs_read: find target inode: inode_id = %u, file_recipe addr: %x\n", 
    inode.inode_id, inode.file_recipe);

  if( inode.file_recipe == 0 && inode.file_size !=0 ) {
    printf("lfs_read: filesize and file recipe doesn't match, return 0\n");   //this shouldn't happen
    return 0; 
  }
  //calcuate cid and seg_offset of file recipe
  uint32_t cid = (inode.file_recipe - c_blk_size)/c_container_size;
  uint32_t seg_offset = (inode.file_recipe - c_blk_size)%c_container_size/c_blk_size;
  printf("lfs_read: calculate file recipe cid : %u, seg offset :%u\n", 
         cid, seg_offset);
  
  //locate file recipe
  file_recipe_t *file_recipe; 
  if ( lfs_info->buf_container->header->container_id == cid ) {
     printf("lfs_read: find file recipe in buf container\n"); 
     file_recipe = (file_recipe_t*)(lfs_info->buf_container->buf + seg_offset * c_seg_size);
  } else if( lfs_info->cur_container->header->container_id == cid) {
     printf("lfs_read: find file recipe in cur container\n"); 
     file_recipe = (file_recipe_t*)(lfs_info->cur_container->buf + seg_offset * c_seg_size);
  } else {
     printf("lfs_read: read container from disk\n"); 
     container_read(lfs_info->cur_container, cid);
     file_recipe = (file_recipe_t*)(lfs_info->cur_container->buf + seg_offset * c_seg_size);
  }
  print_filerecipe(file_recipe);
  
  //find file recipe in namespace
  namespace_record_t record;
  namespace_record_t *result;
  uint32_t index = 0; 
  for( index = 0; index != size/c_blk_size; index++) {
    memcpy( &record.fp, &file_recipe->records[index].fingerprint, sizeof( fingerprint_t) ); 
    HASH_FIND(hh, lfs_info->lfs_namespace, &record.fp, sizeof(fingerprint_t), result);
    if( result == NULL ) {                                        //if no record in namespace
      printf("lfs_read: cannot find fingerprint in namespace\n"); 
      return strlen(buf);                                                   //error; 
    } else {                                                      
      printf("lfs_read: find fingerprint in namespace, container_id = %u\n", result->container_id);
      uint32_t seg_num = 0; 
      if( result->container_id == lfs_info->buf_container->header->container_id ) {
        
        seg_num = container_header_find_fingerprint(lfs_info->buf_container, &record.fp);
        container_print_header( lfs_info->buf_container);
        memcpy(buf + index*c_blk_size, lfs_info->buf_container->buf + seg_num * c_blk_size, c_blk_size);

      } else if ( result->container_id == lfs_info->cur_container->header->container_id) {
        seg_num = container_header_find_fingerprint(lfs_info->cur_container, &record.fp);
        container_print_header( lfs_info->cur_container);
        memcpy(buf + index*c_blk_size, lfs_info->cur_container->buf + seg_num * c_blk_size, c_blk_size);

      } else {
        container_read(lfs_info->cur_container, result->container_id);
        seg_num = container_header_find_fingerprint(lfs_info->cur_container, &record.fp);
        container_print_header( lfs_info->cur_container);
        memcpy(buf + index*c_blk_size, lfs_info->cur_container->buf + seg_num * c_blk_size, c_blk_size);

      }
      printf("find seg offset in container header: %u\n", seg_num); 
      printf("lsf_read: buf size : %u\n", (uint32_t)strlen(buf)); 
    }
  }
  return strlen(buf); 
}

static int lfs_write(const char *path, const char *buf, size_t size,
                     off_t offset, struct fuse_file_info *fi){

  printf("lfs_write: enter, default para: size=%u, offset=%u\n", 
                            (uint32_t)size, (uint32_t)offset);
  printf("lfs_write: buffer data to write %s, size: %u\n", buf, (uint32_t)size);
  printf("lfs_write: path to write %s\n", path); 
  
  int ret = 0; 
  inode_t inode;
 
  //get inode of that inode
  ret = dir_get_inode(path, &inode);
  if (ret != 0 ){
    printf("lfs_write: cannot get inode by using dir_get_inode()\n"); 
    return -ENOENT; 
  }
  printf("lfs_write: find target inode, inode_id = %u, file_recipe addr= %x\n", 
           inode.inode_id, inode.file_recipe);

  file_recipe_t *recipe = NULL;
  if( inode.file_recipe == 0 ){                          //if the file has no file recipe
     //allocate mem for file recipe
     recipe = (file_recipe_t*)malloc(c_seg_size ); 
     memset( recipe, 0 , c_seg_size); 
     //container_add_seg (lfs_info->buf_container, (void*)file_recipe); 
  } else {                                              //if file has file recipe
    //calculate file recipe cid and offset
    uint32_t recipe_cid = (inode.file_recipe - c_blk_size)/c_container_size;
    uint32_t recipe_offset = (inode.file_recipe - c_blk_size)%c_container_size/c_blk_size; 

    //find target file recipe 
    if( lfs_info->buf_container->header->container_id == recipe_cid ) {
      printf("lfs_write: file recipe is in buf container\n"); 
      recipe = (file_recipe_t*)( lfs_info->buf_container->buf + recipe_offset * c_seg_size);
    } else if( lfs_info->cur_container->header->container_id == recipe_cid ) {
      recipe = (file_recipe_t*)( lfs_info->cur_container->buf + recipe_offset * c_seg_size);
    } else {
      container_read( lfs_info->cur_container, recipe_cid );
      recipe = (file_recipe_t*)( lfs_info->cur_container->buf + recipe_offset * c_seg_size);
    }
  }
   

  //inode modification time
  time(&inode.mtime);
  //allocate memory for file recipe entry; 
  file_recipe_record_t *recipe_entry = malloc( sizeof(file_recipe_record_t) );
  //file recipe
  fingerprint_seg_record_t *header_record = malloc( sizeof( fingerprint_seg_record_t));
  //name space entry
  namespace_record_t *item = (namespace_record_t*)malloc(sizeof(namespace_record_t));
  uint32_t i = 0; 
  uint32_t seg_cnt = 1 ; //size/c_seg_size;
  while( seg_cnt != 0) {
    //compute fingerprint of $4KB
    compute_fingerprint(&recipe_entry->fingerprint, (uint8_t*)(buf + i*c_seg_size),
                        c_seg_size);
    recipe_entry->seg_num = offset/c_blk_size;
    //add entry to file recipe
    filerecipe_add_entry( recipe, recipe_entry); 
    
    //add fingerprint to container header
    //container_header_add_fingerprint(lfs_info->buf_container, header_record); 
    
    fingerprint_print(&recipe_entry->fingerprint);
    
    namespace_record_t *result; 
    HASH_FIND(hh, lfs_info->lfs_namespace, &recipe_entry->fingerprint,sizeof(fingerprint_t), result);
    if( result == NULL ) {
      printf("lfs_write: no duplicated seg\n");
      //header fingerprint entry; 
      memset(header_record, 0, sizeof(fingerprint_seg_record_t) );
      memcpy(header_record->fp.fingerprint, recipe_entry->fingerprint.fingerprint, FINGERPRINT_SIZE);
      header_record->seg = lfs_info->buf_container->seg_offset;
      printf("lfs_write: fingerprint header seg_offset %u\n", header_record->seg);
      //add fingerprint to container header
      container_header_add_fingerprint(lfs_info->buf_container, header_record);
 
      //fingerprint_print(&recipe_entry->fingerprint);
      //add fingerprint to namespace;
      memset( item, 0, sizeof(namespace_record_t) ); 
      memcpy( item->fp.fingerprint, recipe_entry->fingerprint.fingerprint, sizeof(fingerprint_t) );
      item->container_id = lfs_info->buf_container->header->container_id;  
      HASH_ADD(hh, lfs_info->lfs_namespace, fp, sizeof(fingerprint_t), item);
      //add data to the block 
      container_add_seg(lfs_info->buf_container, (char*)buf + i*c_seg_size);
    } else {
      printf("lfs_write: find seg in disk\n");
    }
    
    seg_cnt--;
    i++;

  }
  
  print_filerecipe(recipe); 
  printf(" lfs_write: file recipe entry cnt: %u\n", HASH_CNT(hh, lfs_info->lfs_namespace));
  
  //add file recipe to mem
  inode.file_recipe = lfs_info->buf_container->header->container_id * c_container_size
                    + lfs_info->buf_container->seg_offset * c_blk_size + c_blk_size;
  printf("lfs_write: new file recipe is added to mem addr %x\n", inode.file_recipe); 
  container_add_seg(lfs_info->buf_container, (char*)recipe); 
  
  //update inode map
  for (i = 0; i < MAX_INODE_NUM; i++) {
    if (lfs_info->imap->records[i].inode_id == inode.inode_id) {
      lfs_info->imap->records[i].inode_addr = 
          lfs_info->buf_container->header->container_id * c_container_size
          + lfs_info->buf_container->seg_offset * c_seg_size + c_blk_size;
      printf("lfs_write: inode new addr is %x\n", 
        lfs_info->imap->records[i].inode_addr); 
          break;
    }
  }
  inode.file_size = size; 
  container_add_seg(lfs_info->buf_container, (char*)&inode);
  print_inodemap(lfs_info->imap);
  free(recipe_entry); 
  free(header_record); 
  return size; 
}

static int lfs_rename(const char* old_path, const char* new_path) {
  printf("lfs_rename: enter\n");
  printf("lfs_rename: new name: %s, old name %s\n", new_path, old_path);
  const char *new_filename = get_filename(new_path);
  const char *old_filename = get_filename(old_path);
  dir_t *cur_dir = open_cur_dir(); 
  int32_t ret = 0; 
  ret = dir_change_entry( cur_dir, old_filename, new_filename);
  if(ret != 0) {
    return -ENOENT; 
  }
  print_dir_data( cur_dir); 
  dir_commit_changes(cur_dir, lfs_info->cur_inode); 
  return 0; 
}

static int lfs_unlink(const char* path){
  printf("lfs_unlink(remove): enter\n");
  const char* filename = get_filename( path );
  printf("lfs_unlink: rm %s\n", filename);
  dir_t *cur_dir = open_cur_dir();
  int32_t ret = 0; 
  uint32_t inode_id = 0; 
  ret = get_inode_id_from_filename( filename, cur_dir, &inode_id); 
  if( ret != 0 ) {
     return -ENOENT; 
  } 
  printf("lfs_unlink: remove inode id %u\n", inode_id); 
  ret = dir_remove_entry(cur_dir, filename); 
  if( ret != 0 ) {
     return -ENOENT; 
  }
  uint32_t i;
  for( i = 0; i != lfs_info->n_inode; i++) {
    if(lfs_info->imap->records[i].inode_id  == inode_id) {
      lfs_info->imap->records[i].inode_id = 0; 
      lfs_info->imap->records[i].inode_addr = 0; 
    } 
  }
  printf("lfs_unlink: print dir data\n"); 
  print_dir_data(cur_dir);
  //container_add_seg(lfs_info->buf_container, (char*)cur_dir->records); 
  dir_commit_changes(cur_dir, lfs_info->cur_inode);
  print_inodemap( lfs_info->imap); 
  
  return 0; 
}



//should do rm recursively, much more compliated
static int lfs_rmdir(const char * path){
  printf("lfs_rmdir: enter\n");
  const char *dir_name = get_filename(path);
  printf("lfs_rmdir: remove directory %s\n", dir_name); 
  dir_t *cur_dir = open_cur_dir(); 
  int32_t ret = 0;
  uint32_t inode_id = 0;
  ret = get_inode_id_from_filename( dir_name, cur_dir, &inode_id);
  if( ret != 0 ) { 
    return -ENOENT;
  }
  printf("lfs_rmdir: remove inode id %u\n", inode_id);
  
  ret = dir_remove_entry(cur_dir, dir_name);
  if( ret != 0 ) {
     return -ENOENT;
  }
  uint32_t i;
  for( i = 0; i != lfs_info->n_inode; i++) {
    if(lfs_info->imap->records[i].inode_id  == inode_id) {
       lfs_info->imap->records[i].inode_id = 0;
       lfs_info->imap->records[i].inode_addr = 0;
     }
  } 
  printf("lfs_unlink: print dir data\n");
  print_dir_data(cur_dir);
  //container_add_seg(lfs_info->buf_container, (char*)cur_dir->records); 
  dir_commit_changes(cur_dir, lfs_info->cur_inode);
  print_inodemap( lfs_info->imap);
  return 0; 
} 


static struct fuse_operations lfs_oper = {
    .getattr    = lfs_getattr,
    .mkdir      = lfs_mkdir,
    .readdir    = lfs_readdir,
    .open       = lfs_open,
    .read       = lfs_read,
    .write      = lfs_write,
    .create     = lfs_create,
    .access     = lfs_access,
    .unlink     = lfs_unlink,
    .rmdir      = lfs_rmdir,
    .rename     = lfs_rename
};



void lfs_usage() {

}

void lfs_init() {
  lfs_info = (lfs_global_info_t*)malloc(sizeof(lfs_global_info_t));
  lfs_info->imap = (inode_map_t*)malloc(sizeof(inode_map_t));
  lfs_info->n_inode = 0; 
  
  lfs_info->cur_container = container_init();
  lfs_info->cur_container->header->container_id = 0; 
  lfs_info->buf_container = container_init();
  lfs_info->buf_container->header->container_id = 0;   
  //allocate memory for namespace  
  lfs_info->lfs_namespace = NULL; 
  lfs_info->fd = open("./lfslog", O_RDWR|O_CREAT|O_TRUNC);
  assert(lfs_info->fd>0);
  
 
  //create a file of required size on disk that needs to be used to represent the
  //log structured filey system
  //uint32_t file_size = MAX_CONTAINER_NUM * CONTAINER_BLK_NUM * BLK_SIZE + BLK_SIZE;
  uint32_t file_size = c_max_container_num * c_container_blk_num * c_blk_size + c_blk_size;
  char *buf = malloc(file_size);
  memset((void*) buf, 0, file_size);
  pwrite(lfs_info->fd, buf, file_size, 0);
  free(buf); 

  //add dir / to file system
  //first build the root inode;
  inode_t *root_inode = malloc(sizeof( inode_t ));
  /*
  root_inode->inode_id = 1;                    //for root inode, inode_id = 0
  root_inode->inode_type = DIRECTORY;          //inode_type = DIRECTORY
  root_inode->file_size = 0x1000;              //for directory files size could be 4K, 8K, ...
  */
  root_inode_init(root_inode); 
  lfs_info->n_inode++;                         //first root node
  lfs_info->imap->records[0].inode_id = root_inode->inode_id;
  lfs_info->imap->records[0].inode_addr = c_blk_size + c_blk_size;//put the root node at the container 1 seg 1
  
  dir_t *root_dir = malloc( sizeof(dir_t) );   //build directory struct for the root inode
  root_dir->num = 0; 
  //root_dir->num = 2; 
  root_dir->records = malloc( 20 * sizeof(dir_record_t));  
  dir_add_entry( root_dir, "..", root_inode->inode_id); //add current and parent directory
  dir_add_entry( root_dir, ".", root_inode->inode_id);  //for root, they are both itself
  
  root_inode->direct_blk[0] = c_blk_size + 2 * c_blk_size;          //header is 0, inode is the 1 
  printf("init: root_inode->direct_blk[0] %x\n", root_inode->direct_blk[0]); 
  container_add_seg( lfs_info->cur_container, (char*)root_inode );
  container_add_seg( lfs_info->cur_container, (char*)root_dir->records);
  
  container_copy( lfs_info->buf_container, lfs_info->cur_container ); 
  
  lfs_info->cur_inode = root_inode;
  lfs_info->cur_path = (char *)malloc (256);
  lfs_info->par_path = (char *)malloc (256);
  strcpy (lfs_info->cur_path,"/");
  strcpy (lfs_info->par_path, "/");
  printf("lfs_init: current inode id is %u\n", lfs_info->cur_inode->inode_id);
  print_inodemap(lfs_info->imap);
  printf("lfs_init: current_container_id %u\n", lfs_info->cur_container->header->container_id); 
}


int main ( int  argc, char *argv[] ) {
  lfs_init();  
  return fuse_main( argc, argv, &lfs_oper, NULL); 
}
