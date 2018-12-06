#ifndef FILE_H
#define FILE_H

/**
 * @brief generate filename based on name of input file
 *
 * if ext is not NULL, filename will have possible ".bf" extension removed
 * and replaces by ext. if ext is NULL, "a.out" will be returned.
 *
 * @param path path to input file
 * @param ext extension for generated filename or NULL for linked executable
 *
 * @return pointer to malloc'd string containing new filename
 */
char *gen_fname(const char* path, const char* ext);

char *read_file(const char* path);

#endif /* FILE_H */
