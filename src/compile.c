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
        prg_add_instr(subprg, '<', -cur, 0);
    else if (cur > 0)
        prg_add_instr(subprg, '>', cur, 0);

    /* generate +/- instructions relative to alredy moved pointer */
    for (int i = 0; i < mem_diff_size; ++i)
    {
        int diff = mem_diff[i];
        if (diff == 0)
            continue;

        char ptr_cmd;

        if (diff < 0)
        {
            ptr_cmd = '-';
            diff = -diff;
        }
        else
        {
            ptr_cmd = '+';
        }

        prg_add_instr(subprg, ptr_cmd, diff, i + low - cur);
    }

    free(mem_diff);
}

static int parse(const char *src, struct program *prg, struct options *opt)
{
    struct loopstack *stack_top = NULL;
    int loop_count = 0;

    for (size_t i = 0; i < strlen(src); ++i)
    {
        char c = src[i];
        int param = 1;
        int offset = 0;
        switch (c)
        {
        case '.':
        case ',':
            break;
        case '[':
            param = begin_loop(&stack_top, &loop_count);
            break;
        case ']':
            param = end_loop(&stack_top);
            if (param == -1)
            {
                prg_clear(prg);
                return 0;
            }
            break;
        case '+':
        case '-':
        case '>':
        case '<':
            if (!opt->optimize)
                break;

            struct program subprg = {NULL, NULL};
            int count = strcspn(src + i, ",.[]");
            char *cmpd = strndup(src + i, count);
            compound_instr(cmpd, &subprg);
            free(cmpd);
            prg_cat(prg, &subprg);
            i += (count-1);

            continue;

        default:
            continue;
        }

        prg_add_instr(prg, c, param, offset);
    }

    return 1;
}

char *compile(const char *src, struct options *opt)
{
    struct program prg = {NULL, NULL};
    if (src == NULL || opt == NULL || !parse(src, &prg, opt))
        return NULL;

    struct formats fmts;
    (*opt->set_fmts)(&fmts);

    struct instr *prg_ptr = prg.begin;
    char *out = NULL;

    asprintfa(&out, fmts.init_fmt, opt->symbol, opt->memsize);

    while (prg_ptr != NULL)
    {
        switch (prg_ptr->cmd)
        {
        case '<':
            asprintfa(&out, fmts.lt_fmt, prg_ptr->param);
            break;

        case '>':
            asprintfa(&out, fmts.gt_fmt, prg_ptr->param);
            break;

        case '+':
            if (prg_ptr->offset)
                asprintfa(&out, fmts.plus_off_fmt, prg_ptr->param,
                                                    prg_ptr->offset);
            else
                asprintfa(&out, fmts.plus_fmt, prg_ptr->param);
            break;

        case '-':
            if (prg_ptr->offset)
                asprintfa(&out, fmts.minus_off_fmt, prg_ptr->param,
                                                     prg_ptr->offset);
            else
                asprintfa(&out, fmts.minus_fmt, prg_ptr->param);
            break;

        case '[':
            asprintfa(&out, fmts.lb_fmt, prg_ptr->param);
            break;

        case ']':
            asprintfa(&out, fmts.rb_fmt, prg_ptr->param);
            break;

        case '.':
            asprintfa(&out, fmts.dot_fmt);
            break;

        case ',':
            asprintfa(&out, fmts.comma_fmt);
            break;
        }

        prg_ptr = prg_ptr->next;
    }

    /* cleanup code, write/read calls */
    asprintfa(&out, fmts.exit_fmt);
    asprintfa(&out, fmts.write_fmt, STDOUT_FILENO);
    asprintfa(&out, fmts.read_fmt, STDIN_FILENO);

    prg_clear(&prg);
    return out;
}
