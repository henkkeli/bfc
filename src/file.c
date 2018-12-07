#include "file.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

char *gen_fname(const char* path, const char* ext)
{
    if (path == NULL || *path == 0)
        return NULL;

    if (ext == NULL || *ext == 0)
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

char *read_file(const char* path)
{
    FILE *stream = fopen(path, "r");
    if (stream == NULL)
        return NULL;

    fseek(stream, 0, SEEK_END);
    long n = ftell(stream);
    rewind(stream);

    char *buf = malloc(n + 1);

    fread(buf, 1, n, stream);
    fclose(stream);

    buf[n] = 0;

    return buf;
}
