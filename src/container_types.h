#ifndef CONTAINER_TYPES_H
#define CONTAINER_TYPES_H

struct container {
    uint32_t c_id;
    char *buffer;
    //size_t buf_size;
    //container_header_t *header;
};
typedef struct container container_t;

#endif
