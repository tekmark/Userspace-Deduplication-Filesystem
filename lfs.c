#include"lfs.h"

static int lfs_getattr(const char *path, struct stat *stbuf){
     printf ("!!Entry: lfs_getattr function\n"); 
 /*printf("lfs_getattr: path: %s\n", path);
  printf("lfs_getattr: inode num: %ld\n", (long)stbuf->st_ino);
  printf("lfs_getattr: ownership uid=%ld, gid=%ld\n", (long)stbuf->st_uid, (long)stbuf->st_gid); 
  printf("Last file modification:   %s", ctime(&(stbuf->st_mtime)));
  */
  int res = 0;
  
  memset( stbuf, 0, sizeof(struct stat) );
  // if path is root
  if( strcmp(path, "/") == 0 ){        
    //stbuf->st_uid = 1000; 
    stbuf->st_mode = S_IFDIR | 0755; 
    stbuf->st_nlink = 2;
    res = 0;
    printf ("!! Exit: lfs_getattr function with exit code %u\n", res);
  } 
  else {  
    inode_t inode;
    // get inode of the path
    // when initializing the system, path = "/", get inode of root directory
    // when "mkdir aaa", path = "/aaa", get inode of the directory aaa
    // when "touch bbb", path = "/bbb", get inode of the file bbb
    int ret = dir_get_inode(path, &inode);
    printf ("****************DEBUG***************************\n");
    printf ("ret = %u, path = %s, inode->inode_id= %u\n", ret, path, 
        inode.inode_id);
    if (ret == 0) {  // if inode exists
      /* TODO: set the stbuf->st_mode based on the type of the inode 
         (file or directory) */
      stbuf->st_mode = S_IFREG | 0755;
      stbuf->st_nlink = 1;
      stbuf->st_size = inode.file_size;
      res = 0;
      printf ("!! Exit: lfs_getattr(), find file %s\n", path);
    } else {  // if inode doesn't exist
      res = -ENOENT;
      printf ("!! Exit: lfs_getattr(), no such file  %s\n", path);
    }
  }
  return res;
}

static int lfs_mkdir(const char *path, mode_t mode){
  printf("Entry: lfs_mkdir() \n"); 
  
  // configure the inode of new created directory
  lfs_info->n_inode++;
  inode_t *new_dir = malloc(sizeof(inode_t)); 
  // TODO: inode_id should be equal to lfs_info->n_inode
  new_dir->inode_id = lfs_info->n_inode - 1;
  new_dir->inode_type = DIRECTORY;
  new_dir->file_size = 0x1000;
  
  // add an entry to current directory structure cur_dir
  dir_t *cur_dir = open_cur_dir();
  print_inodemap (lfs_info->imap);
  print_dir_data (cur_dir);
  char *new_dirname = get_filename (path);
  inode_t *cur_dir_inode = lfs_info->cur_inode;
  dir_add_entry( cur_dir, new_dirname, new_dir->inode_id);
  dir_commit_changes (cur_dir, cur_dir_inode);
  
  dir_t *test_dir = open_cur_dir();
 
  // modify new directory data
  dir_t *dir_data = malloc (sizeof(dir_t));
  dir_data->num = 0;
  dir_data->records = malloc( 20 * sizeof(dir_record_t));
  // add two entries to new directory data( .. and . )
  dir_add_entry (dir_data, "..", cur_dir_inode->inode_id); 
  dir_add_entry (dir_data, ".", new_dir->inode_id);
  dir_commit_changes (dir_data, new_dir);
  
  printf ("*************lfs_mkdir: Print inode_map ****************** \n");
  print_inodemap(lfs_info->imap); 
  printf ("*************lfs_mkdir: Print current dir data:************ \n");
  print_dir_data (cur_dir);
  printf ("************lfs_mkdir: Print new dir data: *****************\n");
  print_dir_data (dir_data);

  free (cur_dir);
  free (new_dir);
  free (dir_data);
  return 0; 
}

static int lfs_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
                       off_t offset, struct fuse_file_info *fi) {
  printf("!! Entry: lfs_readdir() \n");
  (void) offset;
  (void) fi;
  //struct file_inode_hash *s;

  if (strcmp(path, "/") != 0) {
      printf ("!! Exit: lfs_readdir with exit code %u\n", -ENOENT);
      return -ENOENT;
  }
  
  dir_t *dir = open_cur_dir();
  uint32_t i = 0; 
  //fill all files in current dir
  for( i = 0; i != dir->num; i++) {
    filler(buf, dir->records[i].filename, NULL, 0);
  } 
  printf ("!! Exit: lfs_readdir with exit code 0 \n");
  return 0; 
}

static int lfs_create(const char *path, mode_t mode, struct fuse_file_info *fi){
  //int retstat = 0;
  printf("Entry: lfs_create()\n");
  inode_t temp_ino;
  //uint32_t ret = 0;
  if( !dir_get_inode( path, &temp_ino ) ) {
    printf("Error: inode already exists!\n"); 
    return -EEXIST;
  } else {
    inode_t *new_inode = malloc (sizeof(inode_t));
    memset (new_inode, 0, sizeof(inode_t));
    lfs_info->n_inode++;
    // chao -- please change inode_id to be lfs_info->n_inode
    new_inode->inode_id = lfs_info->n_inode-1;
    new_inode->inode_type = REGULAR_FILE;
    new_inode->file_size = 0x1000;
    new_inode->owner = 0;
    new_inode->mode = 755;
    // TODO: set all properties of inode    
    // inode->direct_blk[0] = lfs_info->buf_container->offset;    
    // file_recipe address added in commit;
    char * filename = get_filename (path);
    // change cur dir inode and cur dir data and commit
    dir_t * cur_dir = open_cur_dir();
    dir_add_entry( cur_dir, filename , new_inode->inode_id);
    dir_commit_changes (cur_dir, lfs_info->cur_inode);
    // create a file recipe structure
    //file_recipe_t *file_recipe = malloc (sizeof (file_recipe_t));
    //memset (file_recipe, 0, sizeof (file_recipe_t));
    char *new_inode_seg = malloc (SEG_SIZE);
    memset (new_inode_seg, 0, SEG_SIZE);
    memcpy (new_inode_seg, new_inode, sizeof(inode_t));
    lfs_info->imap->records[lfs_info->n_inode-1].inode_id = new_inode->inode_id;
    lfs_info->imap->records[lfs_info->n_inode-1].inode_addr = lfs_info->buf_container->offset;
    printf ("&&&&&&&&&&&&&&&&&&&&DEBUGDEBUG *********************\n");
    print_inodemap (lfs_info->imap);
    container_add_seg (lfs_info->buf_container, new_inode_seg);
    free (new_inode);
    free(new_inode_seg);


   /****
   // inode_t *new_inode =
        (inode_t*)(lfs_info->buf_container->buf + lfs_info->cur_container->offset );
    printf("lfs_create: current container offset %u \n", lfs_info->cur_container->offset);
    lfs_info->imap->records[lfs_info->n_inode++].inode_id = lfs_info->n_inode; 
    new_inode->inode_id = lfs_info->n_inode;
    //new_inode->inode_addr = lfs_info->cur_container->offset; 
    printf("lfs_create: create inode with id: %u \n",
             new_inode->inode_id); 
    new_inode->inode_type = REGULAR_FILE; 
    **/
  }
  
  return 0; 
}

static int lfs_open(const char *path, struct fuse_file_info *fi){
  return 0; 
}

static int lfs_read(const char *path, char *buf, size_t size, off_t offset,
                    struct fuse_file_info *fi){
  return 0; 
}

static int lfs_write(const char *path, const char *buf, size_t size,
                     off_t offset, struct fuse_file_info *fi){
/*  
  uint32_t recipe_seg_index = offset/SEG_SIZE;
  uint32_t align = SEG_SIZE - offset%SEG_SIZE;
  uint32_t seg_cnt = size/SEG_SIZE;
  //get inode of the file
  uint32_t inode_id; 
  get_inode_id( path, &id);
  inode_t *inode; 
  get_inode( inode_id, inode);  
  fingerprint_t fp;
  uint32_t i = 0; 
  while( seg_cnt != 0) {
    //compute fingerprint of $4KB
    compute_fingerprint(&fp, (uint8_t*)(buf + align + i*SEG_SIZE),
                        SEG_SIZE);
    seg_cnt--;
    i++; 
    namespace_record_t *item; 
    HASH_FIND(hh, name_space, &fp, sizeof(fingerprint_t), item);
    if(item != NULL) {                             //if segment exists
    //uint32_t container_id = item->container_id;    
      recipe_set_fingerprint( inode->file_recipe, 
                              recipe_seg_index, &fp);
      recipe_seg_index++; 
    } else {
    }   
  }
*/ 
  return 0; 
}




static struct fuse_operations lfs_oper = {
    .getattr    = lfs_getattr,
    .mkdir      = lfs_mkdir,
    .readdir    = lfs_readdir,
    .open       = lfs_open,
    .read       = lfs_read,
    .write      = lfs_write,
    .create     = lfs_create
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
    
  
  lfs_info->fd = open("./lfslog", O_RDWR|O_CREAT|O_TRUNC);
  assert(lfs_info->fd>0);
  
 
  //create a file of required size on disk that needs to be used to represent the
  //log structured filey system
  uint32_t file_size = MAX_CONTAINER_NUM * CONTAINER_BLK_NUM * BLK_SIZE + BLK_SIZE;
  char *buf = malloc(file_size);
  memset((void*) buf, 0, file_size);
  pwrite(lfs_info->fd, buf, file_size, 0);
  free(buf); 

  //add dir / to file system
  //first build the root inode;
  inode_t *root_inode = malloc(sizeof( inode_t ));
  root_inode->inode_id = 0;                    //for root inode, inode_id = 0
  root_inode->inode_type = DIRECTORY;          //inode_type = DIRECTORY
  root_inode->file_size = 0x1000; 
  lfs_info->n_inode++; 
  lfs_info->imap->records[0].inode_id = root_inode->inode_id;
  lfs_info->imap->records[0].inode_addr = BLK_SIZE;  
  dir_t *root_dir = malloc( sizeof(dir_t) );   //build directory struct for the root inode
  root_dir->num = 0; 
  //root_dir->num = 2; 
  root_dir->records = malloc( 20 * sizeof(dir_record_t));  
  dir_add_entry( root_dir, "..", root_inode->inode_id); //add current and parent directory
  dir_add_entry( root_dir, ".", root_inode->inode_id);  //for root, they are both itself
  
  root_inode->direct_blk[0] = BLK_SIZE + 2 * BLK_SIZE;          //header is 0, inode is the 1 
  printf("init: root_inode->direct_blk[0] %x\n", root_inode->direct_blk[0]); 
  container_add_seg( lfs_info->cur_container, (char*)root_inode );
  container_add_seg( lfs_info->cur_container, (char*)root_dir->records);
  
  container_copy( lfs_info->buf_container, lfs_info->cur_container ); 
  
  lfs_info->cur_inode = root_inode; 
  printf("lfs_init: current inode id is %u\n", lfs_info->cur_inode->inode_id);
  print_inodemap(lfs_info->imap);
  printf("lfs_init: current_container_id %u\n", lfs_info->cur_container->header->container_id); 
}


int main ( int  argc, char *argv[] ) {
  lfs_init();
  return fuse_main( argc, argv, &lfs_oper, NULL); 
}
