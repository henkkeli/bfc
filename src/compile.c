#define _GNU_SOURCE

#include "common.h"
#include "compile.h"
#include "loop.h"
#include "program.h"
#include "str.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

static void compound_instr(const char *src, struct program *subprg)
{
    /* bounds of modified memory area and current memory pointer */
    int low = 0, high = 0, cur = 0;

    for (size_t i = 0; i < strlen(src); ++i)
    {
        if (src[i] == '<')
        {
            --cur;
            if (cur < low)
                low = cur;
        }
        else if (src[i] == '>')
        {
            ++cur;
            if (cur > high)
                high = cur;
        }
    }

    int mem_diff_size = high - low + 1;
    char *mem_diff = calloc(mem_diff_size, 1);
    char *ptr = mem_diff - low;

    for (size_t i = 0; i < strlen(src); ++i)
    {
        switch (src[i])
        {
        case '<':
            --ptr;
            break;

        case '>':
            ++ptr;
            break;

        case '+':
            ++(*ptr);
            break;

        case '-':
            --(*ptr);
            break;
        }
    }

    /* move pointer to final position */
    if (cur < 0)
        prg_add_instr_1(subprg, SYM_SUBP, -cur);
    else if (cur > 0)
        prg_add_instr_1(subprg, SYM_ADDP, cur);

    /* generate +/- instructions relative to alredy moved pointer */
    for (int i = 0; i < mem_diff_size; ++i)
    {
        int diff = mem_diff[i];
        if (diff == 0)
            continue;

        sym_t ptr_cmd;

        if (diff < 0)
        {
            ptr_cmd = SYM_SUB;
            diff = -diff;
        }
        else
        {
            ptr_cmd = SYM_ADD;
        }

        prg_add_instr_2(subprg, ptr_cmd, diff, i + low - cur);
    }

    free(mem_diff);
}

static int parse(const char *src, struct program *prg, struct options *opt)
{
    struct loopstack *stack_top = NULL;
    int loop_count = 0;
    int loop;

    for (size_t i = 0; i < strlen(src); ++i)
    {
        switch (src[i])
        {
        case '.':
        case ',':
            prg_add_instr_0(prg, src[i]);
            break;

        case '[':
            prg_add_instr_1(prg, SYM_LB, begin_loop(&stack_top, &loop_count));
            break;

        case ']':
            loop = end_loop(&stack_top);
            if (loop == -1)
            {
                prg_clear(prg);
                return -1;
            }

            prg_add_instr_1(prg, SYM_LE, loop);
            break;

        case '+':
        case '-':
        case '>':
        case '<':
            if (!opt->optimize)
            {
                prg_add_instr_1(prg, src[i], 1);
                break;
            }

            struct program subprg = {NULL, NULL};
            int count = strcspn(&src[i], ",.[]");
            char *cmpd = strndup(&src[i], count);
            compound_instr(cmpd, &subprg);
            free(cmpd);
            prg_cat(prg, &subprg);
            i += count-1;

            break;
        }
    }

    if (!loopstack_empty(stack_top))
    {
        prg_clear(prg);
        return -1;
    }

    return 0;
}

char *compile(const char *src, struct options *opt)
{
    struct program prg = {NULL, NULL};
    if (src == NULL || opt == NULL || parse(src, &prg, opt) == -1)
        return NULL;

    struct formats fmts;
    (*opt->set_fmts)(&fmts);

    struct instr *ins = prg.begin;
    char *out = NULL;

    asprintfa(&out, fmts.init_fmt, opt->symbol, opt->memsize);

    while (ins != NULL)
    {
        switch (ins->cmd)
        {
        case SYM_SUBP:
            asprintfa(&out, fmts.lt_fmt, ins->prm1);
            break;

        case SYM_ADDP:
            asprintfa(&out, fmts.gt_fmt, ins->prm1);
            break;

        case SYM_ADD:
            if (ins->prm2)
                asprintfa(&out, fmts.plus_off_fmt, ins->prm1, ins->prm2);
            else
                asprintfa(&out, fmts.plus_fmt, ins->prm1);
            break;

        case SYM_SUB:
            if (ins->prm2)
                asprintfa(&out, fmts.minus_off_fmt, ins->prm1, ins->prm2);
            else
                asprintfa(&out, fmts.minus_fmt, ins->prm1);
            break;

        case SYM_LB:
            asprintfa(&out, fmts.lb_fmt, ins->prm1);
            break;

        case SYM_LE:
            asprintfa(&out, fmts.rb_fmt, ins->prm1);
            break;

        case SYM_WR:
            asprintfa(&out, fmts.dot_fmt);
            break;

        case SYM_RD:
            asprintfa(&out, fmts.comma_fmt);
            break;

        default:
            break;
        }

        ins = ins->next;
    }

    /* cleanup code, write/read calls */
    asprintfa(&out, fmts.exit_fmt);
    asprintfa(&out, fmts.write_fmt, STDOUT_FILENO);
    asprintfa(&out, fmts.read_fmt, STDIN_FILENO);

    prg_clear(&prg);
    return out;
}
