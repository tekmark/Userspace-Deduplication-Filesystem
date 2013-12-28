#include"container.h"
#include"global.h"

const uint32_t c_blk_size = 4096;                 //block size is 4K
const uint32_t c_seg_size = 4096;           //segment equals block size
const uint32_t c_container_blk_num = 8;
const uint32_t c_container_seg_num = 8;
const uint32_t c_container_size = 4096 * 8;
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
  assert(container); 
  uint32_t container_id = container->header->container_id;
  uint32_t addr = container_id * c_container_size + c_blk_size; 
  int32_t ret = 0; 
  ret = pwrite( lfs_info->fd, container->buf, c_container_size, addr);
  printf("container_write: write container_size = %x container id = %u to addr: %x\n", 
          c_container_size, container_id, addr); 
  printf("container_write: write %u bytes to disk\n", ret); 
  return container_id;  
}

//read a container from disk 
uint32_t container_read(container_t *container, uint32_t container_id) { 
  //calculate target container address
  uint32_t addr = container_id * c_container_size + c_blk_size;
  printf("container_read: read container id %u, addr: %x\n", container_id, addr ); 
  pread(lfs_info->fd, container->buf, c_container_size, addr);
  printf("container_read: get container %u from disk\n", (uint32_t)container->header->container_id);
  container_print_header( container ); 
  container->seg_offset = c_container_blk_num;  //container is full
  return 0; 
}

void container_clean( uint32_t container_id, uint32_t *live_seg, 
                      uint32_t size) { 
}

uint32_t container_add_seg( container_t *container, char *seg_buf) {
  //uint32_t seg_num = container->offset/SEG_SIZE;  //+1 because next seg
  printf("container_add_seg: container_offset %u\n",
           container->seg_offset); 
  if ( container->seg_offset >= c_container_blk_num ) {
    printf("cannot add seg to container because no available seg\n");
    return -1;                           
  } else {
    printf("seg is added to container with seg_no %u\n", container->seg_offset);
    memcpy( container->buf + container->seg_offset * c_seg_size, 
            seg_buf, c_seg_size); 
    container->seg_offset += 1;
    if( container->seg_offset == c_container_seg_num ) {
      printf("container_add_seg: container is full, write to disk\n");
      container_print_header(container); 
      lfs_info->next_container_id = container_write( container, NULL) + 1;
      printf("container_add_seg: next container id = %u\n", lfs_info->next_container_id); 
      //memset(container, 0, sizeof(container_t)); 
      memset(container->buf, 0, c_container_size);
      container->header->container_id = lfs_info->next_container_id;
      container->seg_offset = 1; 
    }
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

uint32_t container_header_add_fingerprint( container_t *container, 
                    fingerprint_seg_record_t *record){
  uint32_t index = 0; 
  for( index = 0; index != 8; index++) {
    if(container->header->records[index].seg == 0){
      container->header->records[index].seg = record->seg;
      memcpy( container->header->records[index].fp.fingerprint, 
                  record->fp.fingerprint, FINGERPRINT_SIZE);
      printf("container_header_add_fingerprint: fingerprint is added to entry#%u\n", 
              index);
    break;
    }
  }
  return 0; 
}

//return 
uint32_t container_header_find_fingerprint( container_t *container,
                    fingerprint_t *fp) {
  uint32_t ret = 0;
  uint32_t i, j;
  for ( i = 0; i != 8; i++) {
    for (j = 0; j != FINGERPRINT_SIZE; j++) {
      if ( container->header->records[i].fp.fingerprint[j] != fp->fingerprint[j]){
        break;   
      }
      ret = container->header->records[i].seg;
      if( ret == 0) {
        printf("find useless fingerprint\n"); 
      } 
      printf("container_header_find_fingerprint: find fingerprint record in header, seg: %u\n",               
                   container->header->records[i].seg);
      return ret;  
    }
  }
  return ret; 
} 

void container_print_header(container_t * container) {
  uint32_t i;
  printf("container_print_header*********************************\n"); 
  for( i = 0; i != 8; i++ ){
    fingerprint_print( &container->header->records[i].fp ); 
    printf(" %u \n", container->header->records[i].seg); 
  }
}
