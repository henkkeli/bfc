#include "program.h"
#include <stdlib.h>

sym_t sym(char c)
{
    switch (c)
    {
    case '+': return SYM_ADD;
    case '-': return SYM_SUB;
    case '>': return SYM_ADDP;
    case '<': return SYM_SUBP;
    case '[': return SYM_LB;
    case ']': return SYM_LE;
    case ',': return SYM_RD;
    case '.': return SYM_WR;
    default:  return SYM_NOP;
    }
}

void prg_add_instr_0(struct program *prg, sym_t cmd)
{
    prg_add_instr_2(prg, cmd, 0, 0);
}

void prg_add_instr_1(struct program *prg, sym_t cmd, int prm1)
{
    prg_add_instr_2(prg, cmd, prm1, 0);
}

void prg_add_instr_2(struct program *prg, sym_t cmd, int prm1, int prm2)
{
    struct instr *newnode = malloc(sizeof(struct instr));
    newnode->cmd = cmd;
    newnode->prm1 = prm1;
    newnode->prm2 = prm2;
    newnode->next = NULL;

    if (prg->end != NULL)
        prg->end->next = newnode;
    else
        prg->begin = newnode;

    prg->end = newnode;
}

void prg_cat(struct program *prg, struct program *subprg)
{
    if (subprg->begin == NULL)
        return;

    if (prg->begin == NULL)
        prg->begin = subprg->begin;
    else
        prg->end->next = subprg->begin;

    prg->end = subprg->end;
}

void prg_clear(struct program *prg)
{
    while (prg->begin != NULL)
    {
        struct instr *tmp = prg->begin;
        prg->begin = prg->begin->next;
        free(tmp);
    }
    prg->end = NULL;
}

