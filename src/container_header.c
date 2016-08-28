#include "container_header.h"

const uint32_t c_c_header_size = sizeof(container_header_t);

//test hearder operations.
void c_header_test() {
    printf("header size: %d\n", c_c_header_size);
    container_header_t header;
    header.c_id = 1;
    header.c_type = CONTAINER_TYPE_DATA;
    c_header_print(&header);
    char buffer[c_c_header_size];

    c_header_write(buffer, &header);

    container_header_t header2;
    c_header_read(buffer, &header2);

    c_header_print(&header2);
}

//TODO: return type change
int c_header_read (char *buf, container_header_t *header) {
    header->c_id = ((container_header_t*) buf)->c_id;
    header->c_type = ((container_header_t*) buf)->c_type;
    //c_header_print(header);
    return 0;
}

//write to buffer
int c_header_write(char *dist_buf, container_header_t *header) {
    memcpy(dist_buf, (void*)header, c_c_header_size);
    return 0;
}

uint32_t c_header_get_id(container_header_t *header) {
    return header->c_id;
}

uint32_t c_header_get_type(container_header_t *header) {
    return header->c_type;
}

void c_header_print(container_header_t *header) {
    printf("id: %d | type: %d\n", header->c_id, header->c_type);
}
