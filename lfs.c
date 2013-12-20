#include"lfs.h"

static int lfs_getattr(const char *path, struct stat *stbuf){
  /*printf("lfs_getattr: path: %s\n", path);
  printf("lfs_getattr: inode num: %ld\n", (long)stbuf->st_ino);
  printf("lfs_getattr: ownership uid=%ld, gid=%ld\n", (long)stbuf->st_uid, (long)stbuf->st_gid); 
  printf("Last file modification:   %s", ctime(&(stbuf->st_mtime)));
  */
  int res = 0;
  
  memset( stbuf, 0, sizeof(struct stat) );
  printf ("lfs_getAttr: path: %s\n", path);
  if( strcmp(path, "/") == 0 ){          //if path is root
    printf("lfs_getattr: path is root \n");
    //stbuf->st_uid = 1000; 
    stbuf->st_mode = S_IFDIR | 0777; 
    stbuf->st_nlink = 2;
    res = 0;
    return res;
  } else {
    printf("lfs_getattr: path is %s \n", path);
    printf("lfs_getattr: filename is %s\n", get_filename(path)); 
    dir_t *dir = open_cur_dir();
    printf("dir records[0]: %s\n", dir->records[0].filename); 
    uint32_t i = 0; 
    for( i = 0; i != dir->num; i++) {
      if(!strcmp (get_filename(path), dir->records[i].filename)  ) {
        printf("existing entry in dir, filename is %s\n", get_filename(path));
        stbuf->st_mode = S_IFREG | 0777; 
        stbuf->st_nlink = 1;
        return 0; 
      }
    } 
  }
  res = -ENOENT; 
  printf("-ENOENT = %u \n", -ENOENT); 
  return res; 
}

static int lfs_mkdir(const char *path, mode_t mode){
  printf("lfs_mkdir: enter \n"); 
  
  return 0; 
}

static int lfs_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
                       off_t offset, struct fuse_file_info *fi){
  printf("lfs_readdir: enter\n"); 
  (void) offset;
  (void) fi;
  //struct file_inode_hash *s;

  if (strcmp(path, "/") != 0) {
     return -ENOENT;
  }
  
  dir_t *dir = open_cur_dir();
  uint32_t i = 0; 
  //fill all files in current dir
  for( i = 0; i != dir->num; i++) {
    filler(buf, dir->records[i].filename, NULL, 0);
    printf("lfs_readdir: fill %s to dir\n", dir->records[i].filename); 
  }
   
  return 0; 
}

static int lfs_create(const char *path, mode_t mode, struct fuse_file_info *fi){
  int retstat = 0;
  printf("lfs_create: debug****************************************\n");  
  inode_t temp_ino;
  uint32_t ret = 0;
  if( !dir_get_inode( path, &temp_ino ) ) {
    printf("lfs_create: inode exists!\n"); 
  } else {
    inode_t *new_inode = (inode_t*)(lfs_info->cur_container->buf + lfs_info->cur_container->offset );
    printf("lfs_create: current container offset %u \n", lfs_info->cur_container->offset);
    lfs_info->imap->records[lfs_info->n_inode++].inode_id = lfs_info->n_inode; 
    new_inode->inode_id = lfs_info->n_inode;
    //new_inode->inode_addr = lfs_info->cur_container->offset; 
    printf("lfs_create: create inode with id: %u \n",
             new_inode->inode_id); 
    new_inode->inode_type = REGULAR_FILE; 
    
  }
  
  return 0; 
}

static int lfs_open(const char *path, struct fuse_file_info *fi){
  printf("lfs_open: enter\n"); 
  
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
  dir_add_entry( root_dir, "../", root_inode->inode_id); //add current and parent directory
  dir_add_entry( root_dir, "./", root_inode->inode_id);  //for root, they are both itself
  
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
