#ifndef COMPILE_H
#define COMPILE_H

#include "common.h"

/**
 * @brief compile bf code in src and return malloc'd compiled code
 *
 * @param src source code
 * @param opt options structure
 *
 * @return pointer to compiled code or NULL on failure
 */
char *compile(const char *src, struct options *opt);

#endif /* COMPILE_H */
