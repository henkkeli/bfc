#ifndef STR_H
#define STR_H

/**
 * @brief asprintf but append string to *str
 *
 * @param strp pointer to existing malloc'd string or NULL
 * @param fmt see printf(3)
 * @param ... see printf(3)
 *
 * @return count of appended bytes
 */
int asprintfa(char **strp, const char *fmt, ...);

#endif /* STR_H */
