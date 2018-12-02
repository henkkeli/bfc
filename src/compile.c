#include "common.h"
#include "compile.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/syscall.h>

struct loopstack {
    int n;
    struct loopstack *prev;
};

struct instr {
    char cmd;
    int param;
    struct instr *next;
};

static struct loopstack *stack_top = NULL;

static void instr_list_add(struct instr **list_ptr, char cmd, int param)
{
    struct instr *newnode = (struct instr *) malloc(sizeof(struct instr));
    newnode->cmd = cmd;
    newnode->param = param;
    newnode->next = NULL;

    (*list_ptr)->next = newnode;
    *list_ptr = newnode;
}

static void instr_list_free(struct instr *ptr)
{
    struct instr *tmp = ptr;
    while (ptr != NULL)
    {
        tmp = ptr->next;
        free(ptr);
        ptr = tmp;
    }
}

static int begin_loop(void)
{
    static int count = 0;
    struct loopstack *loop = (struct loopstack *) malloc(sizeof(struct loopstack));
    loop->n = count;
    loop->prev = stack_top;
    stack_top = loop;
    return count++;
}

static int end_loop(void)
{
    if (stack_top == NULL)
        return -1;

    int res = stack_top->n;
    struct loopstack *tmp = stack_top;
    stack_top = stack_top->prev;
    free(tmp);
    return res;
}

static struct instr *parse(FILE *in_stream)
{
    struct instr *prg_start = (struct instr *) malloc(sizeof(struct instr));
    prg_start->cmd = 0;
    struct instr *prg_ptr = prg_start;

    int count = 0;
    char c;
    int loop;
    while ((c = getc(in_stream)) != EOF)
    {
        switch (c)
        {
        case '.':
        case ',':
            instr_list_add(&prg_ptr, c, 1);
            break;
        case '[':
            instr_list_add(&prg_ptr, c, begin_loop());
            break;
        case ']':
            loop = end_loop();
            if (loop == -1)
            {
                instr_list_free(prg_start);
                return NULL;
            }
            instr_list_add(&prg_ptr, c, loop);
            break;
        case '+':
        case '-':
            ungetc(c, in_stream);
            count = 0;

            while ((c = getc(in_stream)) != EOF)
            {
                if (c == '+')
                    ++count;
                else if (c == '-')
                    --count;
                else
                    break;
            }

            ungetc(c, in_stream);

            if (count == 0)
                break;
            else if (count > 0)
                c = '+';
            else
            {
                c = '-';
                count = -count;
            }

            instr_list_add(&prg_ptr, c, count);
            break;
        case '>':
        case '<':
            ungetc(c, in_stream);
            count = 0;

            while ((c = getc(in_stream)) != EOF)
            {
                if (c == '>')
                    ++count;
                else if (c == '<')
                    --count;
                else
                    break;
            }

            ungetc(c, in_stream);

            if (count == 0)
                break;
            else if (count > 0)
                c = '>';
            else
            {
                c = '<';
                count = -count;
            }

            instr_list_add(&prg_ptr, c, count);
            break;
        }
    }

    return prg_start;
}

int compile(FILE *in_stream, FILE *out_stream, struct options *opt)
{
    struct instr *prg_start = parse(in_stream);
    if (prg_start == NULL)
        return 0;

    fprintf(out_stream,
            "\t.globl\t%1$s\n"
            "\t.lcomm\tbuf, %2$d\n"
            "\t.text\n"
            "%1$s:\n"
            "\tpushq\t%%rbp\n"
            "\tmovq\t%%rsp, %%rbp\n"
            "\tleaq\tbuf(%%rip), %%rbx\n", /* init cell pointer */
            opt->symbol, opt->memsize);

    struct instr *prg_ptr = prg_start->next;
    while (prg_ptr != NULL)
    {
        switch (prg_ptr->cmd)
        {
        case '<':
            fprintf(out_stream,
                    "\tsubq\t$%d, %%rbx\n",
                    prg_ptr->param);
            break;

        case '>':
            fprintf(out_stream,
                    "\taddq\t$%d, %%rbx\n",
                    prg_ptr->param);
            break;

        case '+':
            fprintf(out_stream,
                    "\taddb\t$%d, (%%rbx)\n",
                    prg_ptr->param);
            break;

        case '-':
            fprintf(out_stream,
                    "\tsubb\t$%d, (%%rbx)\n",
                    prg_ptr->param);
            break;

        case '[':
            fprintf(out_stream,
                    "LB%1$d:\n"
                    "\tcmpb\t$0, (%%rbx)\n"
                    "\tje\tLE%1$d\n",
                    prg_ptr->param);
            break;

        case ']':
            fprintf(out_stream,
                    "\tjmp\tLB%1$d\n"
                    "LE%1$d:\n",
                    prg_ptr->param);
            break;

        case '.':
            fprintf(out_stream,
                    "\tcall\twrite\n");
            break;

        case ',':
            fprintf(out_stream,
                    "\tcall\tread\n");
            break;
        }

        prg_ptr = prg_ptr->next;
    }

    /* cleanup code, write/read calls */
    fprintf(out_stream,
            "\tmovl\t$0, %%eax\n"         /* return value */
            "\tpopq\t%%rbp\n"
            "\tret\n");

    fprintf(out_stream,
            "write:\n"
            "\tmovq\t$%d, %%rax\n"        /* syscall */
            "\tmovq\t$%d, %%rdi\n"        /* fd */
            "\tmovq\t%%rbx, %%rsi\n"      /* buf */
            "\tmovq\t$1, %%rdx\n"         /* count */
            "\tsyscall\n"
            "\tret\n",
            SYS_write, STDOUT_FILENO);

    fprintf(out_stream,
            "read:\n"
            "\tmovq\t$%d, %%rax\n"        /* syscall */
            "\tmovq\t$%d, %%rdi\n"        /* fd */
            "\tmovq\t%%rbx, %%rsi\n"      /* buf */
            "\tmovq\t$1, %%rdx\n"         /* count */
            "\tsyscall\n"
            "\tret\n",
            SYS_read, STDIN_FILENO);

    instr_list_free(prg_start);
    return 1;
}
