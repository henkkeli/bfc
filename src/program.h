#ifndef PROGRAM_H
#define PROGRAM_H

struct instr {
    char cmd;
    int param;
    int offset;
    struct instr *next;
};

struct program {
    struct instr *begin;
    struct instr *end;
};

void prg_add_instr(struct program *prg, char cmd, int param, int offset);
void prg_cat(struct program *prg, struct program *subprg);
void prg_clear(struct program *prg);

#endif /* PROGRAM_H */
