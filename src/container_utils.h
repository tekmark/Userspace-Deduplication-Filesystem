#ifndef CONTAINER_UTILS_H
#define CONTAINER_UTILS_H
#include <stdint.h>
#include <assert.h>

// #include "container.h"
#include "container_header.h"

#include "lfs_stat.h"
#include "logger.h"

//calculate container offset in bytes.
uint32_t calculate_container_offset (uint32_t c_id, lfs_stat_t *stat);
//calculate container header in bytes.

//get container id from buffer
// int container_buf_get_id(char *buf);
//update container id in buffer
// int container_buf_update_id(char *buf, uint32_t cid);

// int container_buf_get_type(char *buf);
// int container_buf_update_type(char *buf, int type);

// void container_buf_print(char *buf);

//TODO: update function argument
// void container_header_print(container_header_t *);
void c_utils_print_container();
void c_utils_print_container_header(c_header_t *header);

uint8_t* c_header_to_readable_str(uint8_t *buf, c_header_t *header);
// void container_print(container_t *container);
// void container_dump(container_t *container);
#endif
