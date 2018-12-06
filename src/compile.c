#include "common.h"
#include "compile.h"
#include "dstring.h"
#include <stdlib.h>
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

static struct instr *parse(const char *src)
{
    struct instr *prg_start = (struct instr *) malloc(sizeof(struct instr));
    prg_start->cmd = 0;
    struct instr *prg_ptr = prg_start;

    int count = 0;
    size_t i = 0;
    int loop;
    while (i < strlen(src))
    {
        char c = src[i++];
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

            instr_list_add(&prg_ptr, c, count);
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

            instr_list_add(&prg_ptr, c, count);
            break;
        }
    }

    return prg_start;
}

int compile(const char *src, dstring_t *out, struct options *opt)
{
    struct instr *prg_start = parse(src);
    if (prg_start == NULL)
        return 0;

    ds_strcatf(out,
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
            ds_strcatf(out,
                    "\tsubq\t$%d, %%rbx\n",
                    prg_ptr->param);
            break;

        case '>':
            ds_strcatf(out,
                    "\taddq\t$%d, %%rbx\n",
                    prg_ptr->param);
            break;

        case '+':
            ds_strcatf(out,
                    "\taddb\t$%d, (%%rbx)\n",
                    prg_ptr->param);
            break;

        case '-':
            ds_strcatf(out,
                    "\tsubb\t$%d, (%%rbx)\n",
                    prg_ptr->param);
            break;

        case '[':
            ds_strcatf(out,
                    "LB%1$d:\n"
                    "\tcmpb\t$0, (%%rbx)\n"
                    "\tje\tLE%1$d\n",
                    prg_ptr->param);
            break;

        case ']':
            ds_strcatf(out,
                    "\tjmp\tLB%1$d\n"
                    "LE%1$d:\n",
                    prg_ptr->param);
            break;

        case '.':
            ds_strcatf(out,
                    "\tcall\twrite\n");
            break;

        case ',':
            ds_strcatf(out,
                    "\tcall\tread\n");
            break;
        }

        prg_ptr = prg_ptr->next;
    }

    /* cleanup code, write/read calls */
    ds_strcatf(out,
            "\tmovl\t$0, %%eax\n"         /* return value */
            "\tpopq\t%%rbp\n"
            "\tret\n");

    ds_strcatf(out,
            "write:\n"
            "\tmovq\t$%d, %%rax\n"        /* syscall */
            "\tmovq\t$%d, %%rdi\n"        /* fd */
            "\tmovq\t%%rbx, %%rsi\n"      /* buf */
            "\tmovq\t$1, %%rdx\n"         /* count */
            "\tsyscall\n"
            "\tret\n",
            SYS_write, STDOUT_FILENO);

    ds_strcatf(out,
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
