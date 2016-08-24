#include"inode.h"
//#include"lfs.h"

uint32_t filerecipe_add_entry(file_recipe_t *filerecipe, file_recipe_record_t *entry){
  uint32_t entry_num = entry->seg_num;
  filerecipe->records[entry_num].seg_num = entry_num; 
  memcpy(filerecipe->records[entry_num].fingerprint.fingerprint, 
         entry->fingerprint.fingerprint, FINGERPRINT_SIZE);
  printf("filerecipe_add_entry: seg_num = %u\n", entry_num); 
  return 0; 
}

void print_filerecipe( file_recipe_t *filerecipe) {
  int i = 0;
  for( i = 0; i != RECIPE_NUM; i++) {
    printf("file recipe entry %u seg num : %u, fingerprint", 
             i, filerecipe->records[i].seg_num);
    uint32_t j = 0; 
    for( j = 0; j != FINGERPRINT_SIZE; j++) {
      printf("%x ", filerecipe->records[i].fingerprint.fingerprint[j]);  
    }
    printf("\n"); 
  }
}


void print_inodemap(inode_map_t * inodemap){
   int i = 0;
   while(i <= 10){
      printf("inode_id:%u. inode_addr:%x\n", inodemap->records[i].inode_id, 
         inodemap->records[i].inode_addr);
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
