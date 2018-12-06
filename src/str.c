#define _GNU_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

int asprintfa(char **strp, const char *fmt, ...)
{
    int n;
    va_list ap;

    va_start(ap, fmt);

    char *buf;
    if ((n = vasprintf(&buf, fmt, ap)) == -1)
    {
        va_end(ap);
        return -1;
    }

    char *final = malloc(strlen(*strp) + strlen(buf) + 1);
    strcpy(final, *strp);
    strcat(final, buf);

    free(buf);
    free(*strp);

    *strp = final;

    va_end(ap);

    return n;
}
