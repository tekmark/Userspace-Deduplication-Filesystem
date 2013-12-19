#include"inode.h"
#include"lfs.h"
void print_inodemap(inode_map_t * inodemap){
   int i = 0;
   while(i <= 10){
      printf("inode_id:%u. inode_addr:%x\n", inodemap->records[i].inode_id, inodemap->records[i].inode_addr);
      i++;
   }
}
