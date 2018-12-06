#ifndef DSTRING_H
#define DSTRING_H

#include <stddef.h>

typedef struct {
    char *data;
    size_t capacity;
} dstring_t;

int ds_init(dstring_t *str);
void ds_free(dstring_t *str);
char *ds_string(dstring_t *str);
int ds_strncat(dstring_t *str, const char *src, size_t n);
int ds_strcatf(dstring_t *str, const char *fmt, ...);
size_t ds_strlen(dstring_t *str);

#endif /* DSTRING_H */
