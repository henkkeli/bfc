#ifndef COMMON_H
#define COMMON_H

struct formats {
    const char *init_fmt;
    const char *exit_fmt;
    const char *read_fmt;
    const char *write_fmt;
    const char *gt_fmt;
    const char *lt_fmt;
    const char *plus_fmt;
    const char *plus_off_fmt;
    const char *minus_fmt;
    const char *minus_off_fmt;
    const char *lb_fmt;
    const char *rb_fmt;
    const char *comma_fmt;
    const char *dot_fmt;
};

struct options {
    int assemble;
    int link;
    int memsize;
    char *symbol;
    char *outfile;
    char *infile;
    int optimize;
    char *arch;
    void (*set_fmts)(struct formats*);
};

#endif /* COMMON_H */
