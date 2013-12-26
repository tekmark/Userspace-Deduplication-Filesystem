#include"container.h"
#include"global.h"

const uint32_t c_blk_size = 4096;                 //block size is 4K
const uint32_t c_seg_size = 4096;           //segment equals block size
const uint32_t c_container_blk_num = 8;
const uint32_t c_container_seg_num = 8;
const uint32_t c_container_size = 4096 * 1024;
const uint32_t c_max_container_num = 1024;



container_t* container_init(){
  //allocate memory for a new container
  container_t *container = (container_t*)malloc( sizeof(container_t) );
  memset( container, 0, sizeof(container_t) ); 
  //allocate memory for buf in container
  container->buf = (char*) malloc( c_container_size * sizeof(char) );
  memset( container->buf , 0 , c_container_size);
  //set the first seg as header of the container
  container->header = (container_header_t*)container->buf;
  container->seg_offset = 1 ;      //seg#0 is header, data starts from seg#1
  return container; 
} 

void container_free(container_t *container){
  if( container != NULL ) {
     free(container->buf); 
     free(container); 
  } 
}

//container copy operation
uint32_t container_copy(container_t *dst_container, container_t *src_container){
  dst_container->seg_offset = src_container->seg_offset;    
  memcpy( dst_container->buf, src_container->buf, c_container_size);
  return 0;   
}; 

//write container into disk 
uint32_t container_write( container_t *container, uint32_t *new_id) {
  uint32_t container_id = container->header->container_id;
  uint32_t addr = container_id * c_container_size + c_blk_size;  
  pwrite( lfs_info->fd, container->buf, c_container_size, addr);
  return container_id;  
}

//read a container from disk 
uint32_t container_read(container_t *container, uint32_t container_id) { 
  //calculate target container address
  uint32_t addr = container_id * c_container_size + c_blk_size; 
  pread(lfs_info->fd, container->buf, c_container_size, addr);
  container->seg_offset = c_container_blk_num;  //container is full
  return 0; 
}

void container_clean( uint32_t container_id, uint32_t *live_seg, 
                      uint32_t size) { 
}

uint32_t container_add_seg( container_t *container, char *seg_buf) {
  //uint32_t seg_num = container->offset/SEG_SIZE;  //+1 because next seg
  printf("container_add_seg: container_offset %u",
           container->seg_offset); 
  if ( container->seg_offset >= c_container_blk_num ) {
    printf("cannot add seg to container because no available seg\n");
    return -1;                           
  } else {
    printf("seg is added to container with seg_no %u\n", container->seg_offset);
    memcpy( container->buf + container->seg_offset * c_seg_size, 
            seg_buf, c_seg_size); 
    container->seg_offset += 1;
    return  0; 
  }
}

uint32_t container_get_seg( container_t *container, uint32_t offset, 
                       char *seg_buf) {
  if ( offset * c_seg_size > container->seg_offset ){
    printf("no such seg with seg_offset %u in container\n", container->seg_offset); 
    return -1; 
  }
  else {
    printf("get seg %u from container successfully\n", container->seg_offset);
    memcpy(seg_buf, container->buf + offset*c_seg_size, c_seg_size); 
    return 0;  
  }
    
} 

