#include "lfs_utils.h"

//get the file name from the path
const char* get_filename(const char *path) {
    const char *p =  path;
    while(*p != '\0') {
        p++;
    }
    while(*p != '/') {
        p--;
    }
    p++;
    return p;
}
