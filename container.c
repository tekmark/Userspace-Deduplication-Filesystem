#include"container.h"
#include"global.h"

container_t* container_init(){
  container_t *container = (container_t*)malloc( sizeof(container_t) );
  container->blks[0] = malloc( CONTAINER_SIZE * sizeof(char) );
  memset( container->blks[0] , 0 , CONTAINER_SIZE); 
  uint32_t i;
  for( i = 1; i != CONTAINER_BLK_NUM; i++) {
    container->blks[i] = container->blks[0] + i * BLK_SIZE;
  }
  container->header = (container_header_t*)container->blks[0];
  container->offset = sizeof(container_header_t);  //container is full
  return container; 
} 

void container_free(container_t *container){
  if( container != NULL ) {
     free(container); 
  } 
}


//write container into disk 
uint32_t container_write( container_t *container, uint32_t *new_id) {
  uint32_t container_id = container->header->container_id;
  uint32_t offset = container_id * CONTAINER_SIZE + BLK_SIZE;  
  uint32_t i = 0; 
  for( i = 0; i != CONTAINER_BLK_NUM; i++ ) {
    pwrite( lfs_info->fd, container->blks[i], BLK_SIZE, offset + i * BLK_SIZE);
  } 
  return container_id;  
}

//read a container from disk 
uint32_t container_read(container_t *container, uint32_t container_id) {
  
  uint32_t offset = container_id * CONTAINER_SIZE + BLK_SIZE; 
  pread(lfs_info->fd, container->blks[0], CONTAINER_SIZE, offset);
  container->offset = CONTAINER_SIZE;  //container is full
  return 0; 
}

void container_clean( uint32_t container_id, uint32_t *live_seg, 
                      uint32_t size) {
  
}

uint32_t container_add_seg( container_t *container, char *seg_buf) {
  uint32_t seg_num = container->offset/SEG_SIZE + 1;  //+1 because next seg
  if ( seg_num > CONTAINER_SEG_NUM ) {
    printf("cannot add seg to container because no available seg\n");
    return -1;                           
  } else {
    container->blks[seg_num] = seg_buf;
    printf("seg is added to container with seg_no %u\n", seg_num); 
    return  0; 
  }
  
  
}

uint32_t container_get_seg( container_t *container, uint32_t seg_offset, 
                       char *seg_buf) {
  if ( seg_offset * SEG_SIZE> container->offset ){
    printf("no such seg with seg_offset %u in container\n", seg_offset); 
    return -1; 
  }
  else {
    printf("get seg %u from container successfully\n", seg_offset); 
    seg_buf = container->blks[seg_offset];
    return 0;  
  }
    
} 







