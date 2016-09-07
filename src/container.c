#include "container.h"

void container_test() {
    logger_debug("TESTING container operations ...");
    container_t *container = container_alloc();

    //init container, set to defaults.
    *container->header->type = 98;
    *container->header->data_blk_offset = 1;
    *container->header->seg_tbl->offset = CONTAINER_HEADER_DEFAULT_SEG_TBL_OFFSET;

    segment_t *s = seg_alloc(4096);
    const char * text = "i am segment, hahaha";
    memcpy(s->data, text, strlen(text));
    // container_write_seg(container, s, 1);
    container_add_seg(container, s);

    container_print(container);
    container_write(container);

    fp_t fp0;
    //fp_compute(text, strlen(text), fp0);
    seg_compute_fingerprint(s, &fp0);
    char fp_readable[FINGERPRINT_READABLE_HEX_STR_LEN];
    fp_to_readable_hex(&fp0, fp_readable);
    logger_debug("FP=>%s", fp_readable);
    segment_t *seg = container_get_seg_by_fp(container, &fp0);
    assert(seg);

    // segment_t *s2 = container_get_seg(container, 0);

    /*container_buf_print(container->buffer);
    container_write(container);
    container_buf_print(container->buffer);
    // container_free(container);
    //*/
    container_t *c2 = container_alloc();
    container_read(0, c2);
    container_print(c2);
    segment_t *seg2 = container_get_seg(c2, 0);
    logger_debug("seg_size=>%d", seg2->size);
    // int ret = container_read(1, c2);
    //int cid = container_write(c2);
    // container_buf_print(c2->buffer);
    /*container_t *c3 = container_alloc();
    container_read(0, c3);
    // container_write(c2);
    container_buf_print(c2->buffer);
    segment_t *s2 = container_read_seg(c3, 4096, 4096);
    logger_debug("Segment Data: %s", s2->data);*/
    // container_free(c2);
}

//alloc memory for container. size is container_size in bytes
//return pointer to container.
container_t *container_alloc() {
    //get filesystem defined container size from stat
    lfs_stat_t *stat = get_lfs_stat();

    //TODO: error handling if fails.
    //assert if cannot get filesystem stat
    assert(stat);

    //allocate memeory for struct container_t,
    container_t *container = (container_t*) malloc (sizeof(container_t));
    memset(container, 0, sizeof(container_t));

    int buffer_size = stat->container_size;
    //malloc a blank buffer for container.
    container->buffer = (uint8_t*) malloc (buffer_size);
    memset (container->buffer, 0, buffer_size);

    //NOTE: value is not set here.
    container->header = new_container_header (container->buffer);
    assert(container->header->id);
    //no data pointer assigned at this time. (set to NULL);
    container->data = NULL;
    return container;
}

void container_free(container_t *container) {
    assert(container);
    free(container->buffer);
    free(container->header);
    free(container);
}

//write a container into disk, returns container_id (unique identifier)
int container_write (container_t *container) {
    lfs_stat_t *stat = get_lfs_stat();
    assert(stat);
    // get current container_id for stat.
    int cid = stat->cur_cid;

    assert(container->header->data_blk_offset);
    assert(container->header->id);

    // update container id in buffer.
    *container->header->id = cid;

    // calculate absolute offset.
    int offset = calculate_container_offset(cid, stat);
    // write to disk.
    int ret = pwrite(stat->fd, container->buffer, stat->container_size, offset);
    if (ret < 0) {
        logger_error("Failed to write to disk.");
        return -1;
    }
    if (ret != stat->container_size) {
        logger_error("Wrote %d bytes to disk, but container_size is %d. Failed",
                        stat->container_size, ret);
        return -1;
    }

    int from_blk = offset / stat->blk_size;
    int to_blk = from_blk + stat->blks_per_container;
    logger_debug("Write conatainer#%d (blk#%d - blk#%d) to disk",
                    cid, from_blk, to_blk);

    //update cid in stat.
    stat->cur_cid += 1;

    return cid;
}

//read container from disk.
int container_read (uint32_t container_id, container_t *container) {
    assert(container);
    lfs_stat_t *stat = get_lfs_stat();
    assert(stat);

    //get contaienr offset (in bytes).
    int offset = calculate_container_offset (container_id, stat);
    int ret = pread(stat->fd, container->buffer, stat->container_size, offset);
    if (ret < 0) {
        logger_error("Failed to read container#%d from disk.", container_id);
        return -1;
    }

    //NOTE: header has been filled when pread() above;
    int cid = *container->header->id;
    if (cid != container_id) {
        logger_error("Container#%d. id in buffer is %d", container_id, cid);
        //TODO: error handling.
    }

    //TODO: header check();
    //get header info. assign pointer to container data here.
    int data_offset = *container->header->data_blk_offset * stat->blk_size;
    container->data = container->buffer + data_offset;
    logger_debug("Read container#%d from disk.", container_id);
    return ret;
}

//
// container_t* container_init(){
//   //allocate memory for a new container
//   container_t *container = (container_t*)malloc( sizeof(container_t) );
//   memset( container, 0, sizeof(container_t) );
//   //allocate memory for buf in container
//   container->buf = (char*) malloc( c_container_size * sizeof(char) );
//   memset( container->buf , 0 , c_container_size);
//   //set the first seg as header of the container
//   container->header = (container_header_t*)container->buf;
//   container->seg_offset = 1 ;      //seg#0 is header, data starts from seg#1
//   return container;
// }
//
// container copy operation
// uint32_t container_copy(container_t *dst_container, container_t *src_container){
//   dst_container->seg_offset = src_container->seg_offset;
//   memcpy( dst_container->buf, src_container->buf, c_container_size);
//   return 0;
// };
//
// //write container into disk
// uint32_t container_write( container_t *container, uint32_t *new_id) {
//   assert(container);
//   uint32_t container_id = container->header->container_id;
//   uint32_t addr = container_id * c_container_size + c_blk_size;
//   int32_t ret = 0;
//   ret = pwrite( lfs_info->fd, container->buf, c_container_size, addr);
//   printf("container_write: write container_size = %x container id = %u to addr: %x\n",
//           c_container_size, container_id, addr);
//   printf("container_write: write %u bytes to disk\n", ret);
//   return container_id;
// }
//
// //read a container from disk
// uint32_t container_read(container_t *container, uint32_t container_id) {
//   //calculate target container address
//   uint32_t addr = container_id * c_container_size + c_blk_size;
//   printf("container_read: read container id %u, addr: %x\n", container_id, addr );
//   pread(lfs_info->fd, container->buf, c_container_size, addr);
//   printf("container_read: get container %u from disk\n", (uint32_t)container->header->container_id);
//   container_print_header( container );
//   container->seg_offset = c_container_blk_num;  //container is full
//   return 0;
// }
//
// void container_clean( uint32_t container_id, uint32_t *live_seg,
//                       uint32_t size) {
// }
//

//seg_buf - segment buffer.
//count   - size of segment buffer, in bytes.
// uint32_t container_add_seg (container_t *container, char *seg_buf, size_t count) {
  //uint32_t seg_num = container->offset/SEG_SIZE;  //+1 because next seg
  // printf("container_add_seg: container_offset %u\n",
        //    container->seg_offset);



//get specified block from contaienr
int container_read_blk(container_t *container, int blk_num, uint8_t* blk) {
    lfs_stat_t *stat = get_lfs_stat();
    assert(stat);

    int offset = blk_num * stat->blk_size;
    memcpy(blk, container->buffer + offset, stat->blk_size);
    return 0;
}

int container_write_blk(container_t *container, uint8_t * blk, int blk_num) {
    lfs_stat_t *stat = get_lfs_stat();
    assert(stat);

    int offset = blk_num * stat->blk_size;
    memcpy(container->buffer + offset, blk, stat->blk_size);
    return 0;
}


int container_write_seg(container_t *container, segment_t *seg, int blk_offset) {
    lfs_stat_t *stat = get_lfs_stat();
    assert(stat);

    //check if blk offset is valid.

    //TODO: calculate seg_size in bytes.
    int seg_size = stat->blk_size; //hardcoded

    int offset = blk_offset * stat->blk_size;
    memcpy(container->buffer + offset, seg->data, seg_size);

    logger_debug("Add segment to container, size : %d, blk_offset : %d",
                    seg_size, blk_offset);
    return 0;
}

//offset - block offset of segment
//size - # of blocks
segment_t * container_read_seg(container_t *container, int blk_offset, int count) {
    //get segment sizes
    segment_t *seg = seg_alloc(4096);

    memcpy(seg->data, container->buffer + blk_offset, count);
    logger_debug("Get segment at offset: %d, size: %d", blk_offset, count);
    return seg;
}

//container operations.
int container_add_seg(container_t *container, segment_t *seg) {
    lfs_stat_t *stat = get_lfs_stat();
    assert(stat);

    int blks = seg->blks;
    int size = seg->size;

    seg_tbl_r_t seg_info;

    //get fingerprint of segment.
    seg_compute_fingerprint(seg, &seg_info.fp);
    char fp_hex[FINGERPRINT_READABLE_HEX_STR_LEN];
    fp_to_readable_hex(&seg_info.fp, fp_hex);

    int blk_offset = *container->header->data_blk_offset;
    seg_info.blk_offset = blk_offset;
    seg_info.seg_size = size;

    int byte_offset = blk_offset * stat->blk_size;

    memcpy(container->buffer + byte_offset, seg->data, seg->size);
    assert(container->header->id);

    //add metadata of seg to header.
    c_header_add_seg_info(container->header, &seg_info);

    logger_debug("blk_offset->%d, byte_offset->%d, fp->%s",
                    blk_offset, byte_offset, fp_hex);
    assert(container->header->id);
    return 0;

}

segment_t * container_get_seg(container_t *container, int seg_no) {
    lfs_stat_t *stat = get_lfs_stat();
    assert(stat);

    //get metadata of segment
    seg_tbl_r_t r;
    c_header_get_seg_info(container->header, seg_no, &r);
    // r->blks;
    uint32_t seg_size = r.seg_size;
    // uint32_t blk_offset = r->blk_offset;
    uint32_t byte_offset = r.blk_offset * stat->blk_size;

    segment_t *seg = (segment_t*)malloc(sizeof(segment_t));
    //memory alloc
    seg->data = (uint8_t*)malloc(seg_size * sizeof(uint8_t));
    seg->size = seg_size;

    memcpy(seg->data, container->buffer + byte_offset, seg_size);

    return seg;
}

segment_t * container_get_seg_by_fp(container_t *container, fp_t *fp) {
    lfs_stat_t *stat = get_lfs_stat();
    assert(stat);

    uint8_t fp_readable[FINGERPRINT_READABLE_HEX_STR_LEN];
    fp_to_readable_hex(fp, fp_readable);
    seg_tbl_r_t r;
    int ret = c_header_find_seg_by_fp(container->header, fp, &r);
    if (ret < 0) {
        logger_debug("Failed to find fingerprint[%s] in header", fp_readable);
        return NULL;
    }
    logger_debug("seg_no: %d", ret);
    int offset = r.blk_offset * stat->blk_size;
    logger_debug("SEG SIZE: %d", r.seg_size);
    segment_t * seg = seg_alloc(r.seg_size);

    memcpy(seg->data, container->buffer + offset, r.seg_size);

    return seg;
}

void container_print(container_t *container) {
    // logger_debug("Container:");
    container_header_print(container->header);
}

// int conatainer_get_seg(container_t *container, int blk_offset)
/*
  if ( container->seg_offset >= c_container_blk_num ) {
    printf("cannot add seg to container because no available seg\n");
    return -1;
  } else {
    printf("seg is added to container with seg_no %u\n", container->seg_offset);
    memcpy( container->buf + container->seg_offset * c_seg_size,
            seg_buf, c_seg_size);
    container->seg_offset += 1;
    if ( container->seg_offset == c_container_seg_num ) {
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
}*/
//
// uint32_t container_get_seg( container_t *container, uint32_t offset,
//                        char *seg_buf) {
//   if ( offset * c_seg_size > container->seg_offset ){
//     printf("no such seg with seg_offset %u in container\n", container->seg_offset);
//     return -1;
//   }
//   else {
//     printf("get seg %u from container successfully\n", container->seg_offset);
//     memcpy(seg_buf, container->buf + offset*c_seg_size, c_seg_size);
//     return 0;
//   }
// }
//
// uint32_t container_header_add_fingerprint( container_t *container,
//                     fingerprint_seg_record_t *record){
//   uint32_t index = 0;
//   for( index = 0; index != 8; index++) {
//     if(container->header->records[index].seg == 0){
//       container->header->records[index].seg = record->seg;
//       memcpy( container->header->records[index].fp.fingerprint,
//                   record->fp.fingerprint, FINGERPRINT_SIZE);
//       printf("container_header_add_fingerprint: fingerprint is added to entry#%u\n",
//               index);
//     break;
//     }
//   }
//   return 0;
// }
//
// //return
// uint32_t container_header_find_fingerprint( container_t *container,
//                     fingerprint_t *fp) {
//   uint32_t ret = 0;
//   uint32_t i, j;
//   for ( i = 0; i != 8; i++) {
//     for (j = 0; j != FINGERPRINT_SIZE; j++) {
//       if ( container->header->records[i].fp.fingerprint[j] != fp->fingerprint[j]){
//         break;
//       }
//       ret = container->header->records[i].seg;
//       if( ret == 0) {
//         printf("find useless fingerprint\n");
//       }
//       printf("container_header_find_fingerprint: find fingerprint record in header, seg: %u\n",
//                    container->header->records[i].seg);
//       return ret;
//     }
//   }
//   return ret;
// }
//
// void container_print_header(container_t * container) {
//   uint32_t i;
//   printf("container_print_header*********************************\n");
//   for( i = 0; i != 8; i++ ){
//     fingerprint_print( &container->header->records[i].fp );
//     printf(" %u \n", container->header->records[i].seg);
//   }
// }
