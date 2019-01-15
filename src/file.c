#include "file.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

const char *valid_exts[] = {".b", ".bf", NULL};

char *gen_fname(struct options *opt)
{
    if (opt->outfile != NULL)
        return opt->outfile;

    if (opt->assemble && opt->link)
        return strdup("a.out");

    if (opt->infile == NULL || *opt->infile == 0)
        return NULL;

    const char *ext = !opt->assemble ? ".s" : ".o";

    const size_t ext_len = strlen(ext);

    const char *base = strrchr(opt->infile, '/');
    base = (base != NULL ? base+1 : opt->infile);
    const size_t base_len = strlen(base);

    /* reserve buffer big enough for added extension */
    char *fname = malloc(base_len + ext_len + 1);
    strcpy(fname, base);

    /* remove old extension if there is one */
    for (int i = 0; valid_exts[i] != NULL; ++i)
    {
        const size_t oldext_len = strlen(valid_exts[i]);

        if (base_len > oldext_len &&
            strcmp(fname + base_len - oldext_len, valid_exts[i]) == 0)
        {
            fname[base_len - oldext_len] = 0;
            break;
        }
    }

    return strcat(fname, ext);
}

char *read_file(const char *path)
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
