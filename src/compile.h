#ifndef COMPILE_H
#define COMPILE_H

#include "common.h"
#include <stdio.h>

/**
 * @brief compile bf code read from in_stream and write result to out_stream
 *
 * @param in_stream stream where caller writes bf code
 * @param out_stream stream where caller reads compiled code
 * @param opt options structure
 *
 * @return 1 on success, 0 on parse failure
 */
int compile(FILE *in_stream, FILE *out_stream, struct options *opt);

#endif /* COMPILE_H */
