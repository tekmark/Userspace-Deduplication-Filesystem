#include"lfs.h"

static int lfs_getattr(const char *path, struct stat *stbuf){
  printf("lfs_getattr: path: %s\n", path);
  printf("lfs_getattr: inode num: %ld\n", (long)stbuf->st_ino);
  printf("lfs_getattr: ownership uid=%ld, gid=%ld\n", (long)stbuf->st_uid, (long)stbuf->st_gid); 
  printf("Last file modification:   %s", ctime(&(stbuf->st_mtime)));
  return 0; 
}

static int lfs_mkdir(const char *path, mode_t mode){
  return 0; 
}

static int lfs_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
                       off_t offset, struct fuse_file_info *fi){
  return 0; 
}

static int lfs_create(const char *path, mode_t mode, struct fuse_file_info *fi){
  int retstat = 0;
  
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
};



void lfs_usage() {

}

void lfs_init() {
  lfs_info = (lfs_global_info_t*)malloc(sizeof(lfs_global_info_t));
  lfs_info->imap = (inode_map_t*)malloc(sizeof(inode_map_t));
  lfs_info->n_inode = 0; 
  
  lfs_info->cur_container = container_init();

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
  //first build the inode;
  inode_t *root_inode = malloc(sizeof( inode_t ));
  root_inode->inode_id = 0;
  root_inode->inode_type = DIRECTORY;
  
  dir_t *root_dir = malloc( sizeof(dir_t) );
  
  //root_dir->num = 2; 
  root_dir->records = malloc( 2 * sizeof(dir_record_t)); 
  dir_add_entry( root_dir, "../", root_inode->inode_id); 
  dir_add_entry( root_dir, "./", root_inode->inode_id);
  
  root_inode->direct_blk[0] = BLK_SIZE + 2 * SEG_SIZE;          //header is 0, inode is the 1 
  container_add_seg( lfs_info->buf_container, (char*)root_inode );
  container_add_seg( lfs_info->buf_container, (char*)root_dir->records);
   
}


int main ( int  argc, char *argv[] ) {
  lfs_init();  
  return fuse_main( argc, argv, &lfs_oper, NULL); 
}
