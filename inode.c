#include"inode.h"
//#include"lfs.h"
void print_inodemap(inode_map_t * inodemap){
   int i = 0;
   while(i <= 10){
      printf("inode_id:%u. inode_addr:%x\n", inodemap->records[i].inode_id, inodemap->records[i].inode_addr);
      i++;
   }
}

void root_inode_init(inode_t *root) {
  root->inode_id = 1;
  root->inode_type = DIRECTORY;
  root->mode = S_IFDIR | 0755; 
  root->file_size = 0x1000;
  time(&root->ctime);
  root->link = 2; 
}
