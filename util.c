#include"util.h"

uint8_t* compute_fingerprint(fingerprint_t *fp, uint8_t *buf, 
                         uint32_t size) {
  return SHA1(buf, size, fp->fingerprint);
}

void fingerprint_print( fingerprint_t *fp ) {
  uint32_t i = 0; 
  for( i = 0; i != FINGERPRINT_SIZE; i++) {
    printf("%x ", fp->fingerprint[i]);
  }
  printf("\n"); 
}

//get the file name from the path
const char* get_filename(const char *path) {
        const char *p =  path;
        while(*p != '\0') {
                p++;
        }
        while(*p != '/')
                p--;
        p++;
        return p;
}


uint32_t get_division_result (uint32_t dividend, uint32_t divisor) {
    uint32_t result = dividend / divisor;
    if (dividend % divisor != 0) {
        result++;
    }
    return result;
}
