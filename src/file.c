#include "file.h"
#include <stdlib.h>
#include <string.h>

char *gen_fname(const char* path, const char* ext)
{
    if (ext == NULL)
        return strdup("a.out");

    const char *bf_ext = ".bf";
    size_t bf_ext_len = strlen(bf_ext);
    size_t ext_len = strlen(ext);

    const char *base = strrchr(path, '/');
    base = (base != NULL ? base+1 : path);
    size_t base_len = strlen(base);

    /* reserve buffer big enough for added extension */
    char *fname = (char *) malloc(base_len + ext_len);
    strcpy(fname, base);

    /* remove .bf extension if there is one */
    if (base_len > bf_ext_len &&
            strcmp(fname + base_len - bf_ext_len, bf_ext) == 0)
        fname[base_len - bf_ext_len] = 0;

    return strcat(fname, ext);
}
