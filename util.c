#include"util.h"

uint8_t* compute_fingerprint(fingerprint_t *fp, uint8_t *buf, 
                         uint32_t size) {
  return SHA1(buf, size, fp->fingerprint);
}

//get the file name from the path
char* get_filename(const char *path) {
        char *p;
        p = path;
        while(*p != '\0') {
                p++;
        }
        while(*p != '/')
                p--;
        p++;
        return p;
}
