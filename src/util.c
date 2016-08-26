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


int32_t get_division_result (uint32_t dividend, uint32_t divisor) {
    uint32_t result = dividend / divisor;
    if (dividend % divisor != 0) {
        result++;
    }
    return result;
}

char* get_parentpath(const char *path)
{
    if (!strcmp ((char *)path, "/"))
        return (char *)path;

    char *p_end = (char *)path;
    int length = 0;
    char *parent_path;

    while(*p_end != '\0') {
        length++;
        p_end++;
    }

    while(*p_end != '/') {
        length--;
        p_end--;
    }
    if (length == 0)
        return "/";
    //length--;
    printf ("length == %u\n", length);
    parent_path = (char *)malloc (length + 1);
    memset (parent_path, 0, length + 1);
    memcpy (parent_path, path, length);
    if (strcmp (parent_path, "") == 0)
        return "/";
    return parent_path;
}

//helper function: conver string to Uppercases
void string_toupper(char *str) {
    int len = strlen(str);
    for (int i = i; i < len; ++i) {
        str[i] = toupper(str[i]);
    }
    return;
}
