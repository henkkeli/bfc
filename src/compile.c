#define _GNU_SOURCE

#include "common.h"
#include "compile.h"
#include "str.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <string.h>

struct loopstack {
    int n;
    struct loopstack *prev;
};

struct instr {
    char cmd;
    int param;
    struct instr *next;
};

struct program {
    struct instr *begin;
    struct instr *end;
};

static void prg_add_instr(struct program *prg, char cmd, int param)
{
    struct instr *newnode = malloc(sizeof(struct instr));
    newnode->cmd = cmd;
    newnode->param = param;
    newnode->next = NULL;

    if (prg->end != NULL)
        prg->end->next = newnode;
    else
        prg->begin = newnode;

    prg->end = newnode;
}

static void prg_clear(struct program *prg)
{
    while (prg->begin != NULL)
    {
        struct instr *tmp = prg->begin;
        prg->begin = prg->begin->next;
        free(tmp);
    }
    prg->end = NULL;
}

static int begin_loop(struct loopstack **top)
{
    static int count = 0;
    struct loopstack *loop = (struct loopstack *) malloc(sizeof(struct loopstack));
    loop->n = count;
    loop->prev = *top;
    *top = loop;
    return count++;
}

static int end_loop(struct loopstack **top)
{
    if (*top == NULL)
        return -1;

    int res = (*top)->n;
    struct loopstack *tmp = *top;
    *top = (*top)->prev;
    free(tmp);
    return res;
}

static int parse(const char *src, struct program *prg)
{
    prg->begin = NULL;
    prg->end = NULL;

    int count = 0;
    size_t i = 0;
    struct loopstack *stack_top = NULL;


    while (i < strlen(src))
    {
        char c = src[i++];
        int param = 0;
        switch (c)
        {
        case '.':
        case ',':
            break;
        case '[':
            param = begin_loop(&stack_top);
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
            --i;
            count = 0;

            while (i < strlen(src))
            {
                c = src[i++];
                if (c == '+')
                    ++count;
                else if (c == '-')
                    --count;
                else
                    break;
            }

            --i;

            if (count == 0)
                break;
            else if (count > 0)
                c = '+';
            else
            {
                c = '-';
                count = -count;
            }

            param = count;
            break;
        case '>':
        case '<':
            --i;
            count = 0;

            while (i < strlen(src))
            {
                c = src[i++];
                if (c == '>')
                    ++count;
                else if (c == '<')
                    --count;
                else
                    break;
            }

            --i;

            if (count == 0)
                break;
            else if (count > 0)
                c = '>';
            else
            {
                c = '<';
                count = -count;
            }

            param = count;
            break;

        default:
            continue;
        }

        prg_add_instr(prg, c, param);
    }

    return 1;
}

char *compile(const char *src, struct options *opt)
{
    struct program prg;
    if (!parse(src, &prg))
        return NULL;

    struct instr *prg_ptr = prg.begin;
    char *out;

    asprintf(&out,
            "\t.globl\t%1$s\n"
            "\t.lcomm\tbuf, %2$d\n"
            "\t.text\n"
            "%1$s:\n"
            "\tpushq\t%%rbp\n"
            "\tmovq\t%%rsp, %%rbp\n"
            "\tleaq\tbuf(%%rip), %%rbx\n", /* init cell pointer */
            opt->symbol, opt->memsize);

    while (prg_ptr != NULL)
    {
        switch (prg_ptr->cmd)
        {
        case '<':
            asprintfa(&out,
                    "\tsubq\t$%d, %%rbx\n",
                    prg_ptr->param);
            break;

        case '>':
            asprintfa(&out,
                    "\taddq\t$%d, %%rbx\n",
                    prg_ptr->param);
            break;

        case '+':
            asprintfa(&out,
                    "\taddb\t$%d, (%%rbx)\n",
                    prg_ptr->param);
            break;

        case '-':
            asprintfa(&out,
                    "\tsubb\t$%d, (%%rbx)\n",
                    prg_ptr->param);
            break;

        case '[':
            asprintfa(&out,
                    "LB%1$d:\n"
                    "\tcmpb\t$0, (%%rbx)\n"
                    "\tje\tLE%1$d\n",
                    prg_ptr->param);
            break;

        case ']':
            asprintfa(&out,
                    "\tjmp\tLB%1$d\n"
                    "LE%1$d:\n",
                    prg_ptr->param);
            break;

        case '.':
            asprintfa(&out,
                    "\tcall\twrite\n");
            break;

        case ',':
            asprintfa(&out,
                    "\tcall\tread\n");
            break;
        }

        prg_ptr = prg_ptr->next;
    }

    /* cleanup code, write/read calls */
    asprintfa(&out,
            "\tmovl\t$0, %%eax\n"         /* return value */
            "\tpopq\t%%rbp\n"
            "\tret\n");

    asprintfa(&out,
            "write:\n"
            "\tmovq\t$%d, %%rax\n"        /* syscall */
            "\tmovq\t$%d, %%rdi\n"        /* fd */
            "\tmovq\t%%rbx, %%rsi\n"      /* buf */
            "\tmovq\t$1, %%rdx\n"         /* count */
            "\tsyscall\n"
            "\tret\n",
            SYS_write, STDOUT_FILENO);

    asprintfa(&out,
            "read:\n"
            "\tmovq\t$%d, %%rax\n"        /* syscall */
            "\tmovq\t$%d, %%rdi\n"        /* fd */
            "\tmovq\t%%rbx, %%rsi\n"      /* buf */
            "\tmovq\t$1, %%rdx\n"         /* count */
            "\tsyscall\n"
            "\tret\n",
            SYS_read, STDIN_FILENO);

    prg_clear(&prg);
    return out;
}
