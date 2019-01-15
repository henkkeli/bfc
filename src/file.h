#ifndef FILE_H
#define FILE_H

#include "common.h"

/**
 * @brief generate filename based on name of input file
 *
 * figures out proper filename for output file, which may be *.s, *.o or a.out.
 * possible .b/.bf extension is removed.
 *
 * @param opts struct with at least assemble/link properly filled and
 * infile/outfile either filled or set to NULL.
 *
 * @return pointer to malloc'd string containing new filename
 */
char *gen_fname(struct options *opt);

/**
 * @brief read entire file
 *
 * @param path path to file
 *
 * @return pointer to malloc'd string of file contents
 */
char *read_file(const char *path);

#endif /* FILE_H */
