#ifndef COMPILE_H
#define COMPILE_H

#include "common.h"
#include "dstring.h"

/**
 * @brief compile bf code read from in_stream and write result to out_stream
 *
 * @param in_stream stream where caller writes bf code
 * @param out_stream stream where caller reads compiled code
 * @param opt options structure
 *
 * @return 1 on success, 0 on parse failure
 */
int compile(const char *src, dstring_t *out, struct options *opt);

#endif /* COMPILE_H */
