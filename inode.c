#include"inode.h"
#include"lfs.h"
void print_inodemap(inode_map_t * inodemap){
   int i = 0;
   while(i <= 10){
      printf("inode:%d,%d", inodemap->records[i]);
      i++
   }
}
