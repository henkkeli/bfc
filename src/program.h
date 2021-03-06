#ifndef PROGRAM_H
#define PROGRAM_H

#define SYM_ADD '+'
#define SYM_SUB '-'
#define SYM_ADDP '>'
#define SYM_SUBP '<'
#define SYM_LB '['
#define SYM_LE ']'
#define SYM_RD ','
#define SYM_WR '.'

typedef char sym_t;

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

void prg_add_instr_0(struct program *prg, sym_t cmd);
void prg_add_instr_1(struct program *prg, sym_t cmd, int prm1);
void prg_add_instr_2(struct program *prg, sym_t cmd, int prm1, int prm2);
void prg_cat(struct program *prg, struct program *subprg);
void prg_clear(struct program *prg);

#endif /* PROGRAM_H */
