#include "program.h"
#include <stdlib.h>

void prg_add_instr_0(struct program *prg, char cmd)
{
    prg_add_instr_2(prg, cmd, 0, 0);
}

void prg_add_instr_1(struct program *prg, char cmd, int prm1)
{
    prg_add_instr_2(prg, cmd, prm1, 0);
}

void prg_add_instr_2(struct program *prg, char cmd, int prm1, int prm2)
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

