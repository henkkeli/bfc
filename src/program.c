#include "program.h"
#include <stdlib.h>

void prg_add_instr(struct program *prg, char cmd, int param, int offset)
{
    struct instr *newnode = malloc(sizeof(struct instr));
    newnode->cmd = cmd;
    newnode->param = param;
    newnode->offset = offset;
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

