#include"util.h"


void print_filerecipe(fr_t *filerecipe) {
    int size = filerecipe->size;
    logger_debug("---File Recipe (# of entries : %d)", size);
    int i = 0;
    for (; i < size; ++i) {
        fp_readable_t fp_hex;
        fp_to_readable_hex(&filerecipe->entries[i].seg_fp, fp_hex.hex);
        logger_debug("#%d[%s]", i, fp_hex.hex);
    }
}


/*
int calculate_blk_no(lfs_stat_t *stat, int cid, int blk_offset) {

}
*/
void convert_SHA1_hex_str(uint8_t *, char *);

/*
fingerprint_t * fp_cpy(fingerprint_t *dest, fingerprint_t *src) {
    memcpy(dest->fingerprint, src->fingerprint, sizeof(fingerprint_t));
    return dest;
}

void fp_to_readable_hex(fp_t *fp, uint8_t *readable) {
    int i;
    for (i = 0; i < FINGERPRINT_LEN; ++i) {
        sprintf(&readable[i * 2], "%02X", fp->data[i]);
    }
    readable[FINGERPRINT_READABLE_HEX_STR_LEN - 1] = 0;
}
*/

void util_test () {
    // The data to be hashed
    /*char data[] = "Hello, world!!!!!!!";
    size_t length = sizeof(data);

    fingerprint_t fp;
    compute_fingerprint(data, length, &fp);

    char hex[41];
    convert_SHA1_hex_str(fp.fingerprint, hex);
    printf("%s\n", hex);
    */
    //fingerprint_print(&fp);
}

//util function
void convert_SHA1_hex_str(uint8_t *fp, char *hash_str) {
    for(int i = 0; i < 20; ++i) {
        sprintf(&hash_str[i*2], "%02X", fp[i]);
    }
    hash_str[40]=0;
}

/*
uint8_t * compute_fingerprint(uint8_t *buf, size_t bytes, fingerprint_t *fp) {
    return SHA1(buf, bytes, fp->fingerprint);
}
*/

void fingerprint_print(fingerprint_t *fp ) {
  uint32_t i = 0;
  for (i = 0; i < 20; ++i) {
    printf("%02X", fp->fingerprint[i]);
  }
  printf("\n");
}

//get the file name from the path
// const char* get_filename(const char *path) {
//         const char *p =  path;
//         while(*p != '\0') {
//                 p++;
//         }
//         while(*p != '/')
//                 p--;
//         p++;
//         return p;
// }


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
