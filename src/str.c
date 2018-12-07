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

    if (*strp == NULL)
    {
        *strp = buf;
    }
    else
    {
        *strp = realloc(*strp, strlen(*strp) + strlen(buf) + 1);
        strcat(*strp, buf);
        free(buf);
    }

    va_end(ap);
    return n;
}
