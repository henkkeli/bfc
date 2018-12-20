#ifndef PROGRAM_H
#define PROGRAM_H

typedef enum {
    SYM_NOP,
    SYM_ADD,
    SYM_SUB,
    SYM_ADDP,
    SYM_SUBP,
    SYM_LB,
    SYM_LE,
    SYM_RD,
    SYM_WR
} sym_t;

struct instr {
    sym_t cmd;
    int prm1;
    int prm2;
    struct instr *next;
};

struct program {
    struct instr *begin;
    struct instr *end;
};

sym_t sym(char c);
void prg_add_instr_0(struct program *prg, sym_t cmd);
void prg_add_instr_1(struct program *prg, sym_t cmd, int prm1);
void prg_add_instr_2(struct program *prg, sym_t cmd, int prm1, int prm2);
void prg_cat(struct program *prg, struct program *subprg);
void prg_clear(struct program *prg);

#endif /* PROGRAM_H */
