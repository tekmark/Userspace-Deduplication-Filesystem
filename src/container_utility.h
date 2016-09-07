#ifndef CONTAINER_UTILITY_H
#define CONTAINER_UTILITY_H
#include <stdint.h>
#include <assert.h>

#include "container.h"
#include "container_header.h"

#include "lfs_stat.h"
#include "logger.h"

//calculate container offset in bytes.
uint32_t calculate_container_offset (uint32_t c_id, lfs_stat_t *stat);
//calculate container header in bytes.

//get container id from buffer
int container_buf_get_id(char *buf);
//update container id in buffer
int container_buf_update_id(char *buf, uint32_t cid);

int container_buf_get_type(char *buf);
int container_buf_update_type(char *buf, int type);

void container_buf_print(char *buf);

//TODO: update function argument
void container_header_print(container_header_t *);
// void container_print(container_t *container);
// void container_dump(container_t *container);
#endif
