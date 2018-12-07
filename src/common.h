#ifndef COMMON_H
#define COMMON_H

struct options {
    int assemble;
    int link;
    int memsize;
    char *symbol;
    char *outfile;
    char *infile;
    int optimize;
};

#endif /* COMMON_H */
