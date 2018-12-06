#define _GNU_SOURCE

#include "dstring.h"
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>

#define SUCCESS 1
#define FAILURE 0

int ds_init(dstring_t *str)
{
    str->capacity = 1;
    str->data = malloc(1);
    str->data[0] = 0;

    if (str->data == NULL)
        return FAILURE;

    return SUCCESS;
}

void ds_free(dstring_t *str)
{
    if (str == NULL)
        return;

    free(str->data);
}

char *ds_string(dstring_t *str)
{
    return str->data;
}

int ds_strncat(dstring_t *str, const char *src, size_t n)
{
    if (str == NULL)
        return FAILURE;

    while (str->capacity <= ds_strlen(str) + n)
        str->capacity *= 2;

    if ((str->data = realloc(str->data, str->capacity)) == NULL)
        return FAILURE;

    strncat(str->data, src, n);

    return SUCCESS;
}

int ds_strcatf(dstring_t *str, const char *fmt, ...)
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

    ds_strncat(str, buf, n);
    free(buf);

    va_end(ap);

    return n;
}

size_t ds_strlen(dstring_t *str)
{
    return strlen(str->data);
}
