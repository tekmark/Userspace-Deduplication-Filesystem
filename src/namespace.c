#include "namespace.h"

void namespace_test() {
    printf("%s\n", "namespace test cases : ");
    //alloc buffer for testing.
    char *buffer = (char*)malloc(sizeof(char) * 4096);
    memset(buffer, 0, sizeof(char) * 4096);

    namespace_t namespace;
    namespace.size = (uint32_t*)buffer;
    namespace.records = (namespace_r_t*) (buffer + sizeof(uint32_t));

    namespace_r_t r0, r1, r2, r3;
    //record 0
    strcpy(r0.fp, "fingerprint_0");
    r0.container_id = 0;
    r0.live = 1;
    //add to table.
    //update size;
    int size = *namespace.size;
    int offset = size * sizeof(namespace_r_t);
    memcpy(namespace.records + offset, &r0, sizeof(r0));
    *namespace.size = 1;
    //record 1
    strcpy(r1.fp, "fingerprint_1");
    r1.container_id = 1;
    r1.live = 1;

    size = *namespace.size;
    offset = size * sizeof(namespace_r_t);
    memcpy(namespace.records + offset, &r1, sizeof(r1));
    *namespace.size = 2;

    //read buffer. use r3;
    int id = 1;
    offset = id * sizeof(r3);
    memcpy(&r3, namespace.records + offset, sizeof(r3));

    printf("Fingerprint: %s, container id: %d\n", r3.fp, r3.container_id);
}
