#include"lfs.h"

static int lfs_getattr(const char *path, struct stat *stbuf){
}

static int lsf_mkdir(const char *path, mode_t mode){
}

static int lsf_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
                       off_t offset, struct fuse_file_info *fi){
}

static int lsf_open(const char *path, struct fuse_file_info *fi){
}

static int lsf_read(const char *path, char *buf, size_t size, off_t offset,
                    struct fuse_file_info *fi){

}

static int lsf_write(const char *path, const char *buf, size_t size,
                     off_t offset, struct fuse_file_info *fi){
  
  uint32_t recipe_seg_index = offset/SEG_SIZE;
  uint32_t align = SEG_SIZE - offset%SEG_SIZE;
  uint32_t seg_cnt = size/SEGMENT_SIZE;
  //get inode of the file
  uint32_t inode_id; 
  get_inode_id(path, &id);
  inode_t *inode; 
  get_inode( inode_id, inode);  
  fingerprint_t fp;
  uint32_t i = 0; 
  while( seg_cnt != 0) {
    //compute fingerprint of $4KB
    compute_fingerprint(&fp, (uint8_t*)(buf + align + i*SIGMENT_SIZE),
                        SEGMENT_SIZE);
    seg_cnt--;
    i++; 
    namespace_record_t *item; 
    HASH_FIND(hh, name_space, &fp, sizeof(fingerprint_t), item);
    if(item != NULL) {                             //if segment exists
    //uint32_t container_id = item->container_id;    
      recipe_set_fingerprint( inode->file_recipe, 
                              recipe_seg_index, &fingerpirnt);
      recipe_seg_index++; 
    } else {
      
  }
  
  
}

int lfs_create(const char *path, mode_t mode,struct fuse_file_info *fi){
  
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
    



}



int main ( uint32_t argc, char *argv[] ) {
  lfs_init(); 
  return fuse_main( argc, argv, &lfs_oper, NULL); 
}
