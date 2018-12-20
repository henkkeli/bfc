#include "arch/amd64.h"
#include "common.h"

const char *amd64_init_fmt = 
    "\t.globl\t%1$s\n"
    "\t.lcomm\tbuf, %2$d\n"
    "\t.text\n"
    "%1$s:\n"
    "\tpushq\t%%rbp\n"
    "\tmovq\t%%rsp, %%rbp\n"
    "\tpushq\t%%rbx\n"
    "\tleaq\tbuf(%%rip), %%rbx\n"; /* init cell pointer */

const char *amd64_exit_fmt =
    "\tmovl\t$0, %%eax\n"          /* return value */
    "\tpopq\t%%rbx\n"
    "\tpopq\t%%rbp\n"
    "\tret\n";

const char *amd64_read_fmt =
    ".read:\n"
    "\tmovq\t$0, %%rax\n"          /* syscall read */
    "\tmovq\t$%d, %%rdi\n"         /* fd */
    "\tmovq\t%%rbx, %%rsi\n"       /* buf */
    "\tmovq\t$1, %%rdx\n"          /* count */
    "\tsyscall\n"
    "\tret\n";

const char *amd64_write_fmt =
    ".write:\n"
    "\tmovq\t$1, %%rax\n"          /* syscall write */
    "\tmovq\t$%d, %%rdi\n"         /* fd */
    "\tmovq\t%%rbx, %%rsi\n"       /* buf */
    "\tmovq\t$1, %%rdx\n"          /* count */
    "\tsyscall\n"
    "\tret\n";

const char *amd64_gt_fmt =
    "\taddq\t$%d, %%rbx\n";

const char *amd64_lt_fmt =
    "\tsubq\t$%d, %%rbx\n";

const char *amd64_plus_fmt =
    "\taddb\t$%d, (%%rbx)\n";

const char *amd64_plus_off_fmt =
    "\taddb\t$%d, %d(%%rbx)\n";

const char *amd64_minus_fmt =
    "\tsubb\t$%d, (%%rbx)\n";

const char *amd64_minus_off_fmt =
    "\tsubb\t$%d, %d(%%rbx)\n";

const char *amd64_lb_fmt =
    ".LB%1$d:\n"
    "\tcmpb\t$0, (%%rbx)\n"
    "\tje\t.LE%1$d\n";

const char *amd64_rb_fmt =
    "\tjmp\t.LB%1$d\n"
    ".LE%1$d:\n";

const char *amd64_comma_fmt =
    "\tcall\t.read\n";

const char *amd64_dot_fmt =
    "\tcall\t.write\n";

void amd64_set_fmts(struct formats *fmts)
{
    fmts->init_fmt      = amd64_init_fmt;
    fmts->exit_fmt      = amd64_exit_fmt;
    fmts->read_fmt      = amd64_read_fmt;
    fmts->write_fmt     = amd64_write_fmt;
    fmts->gt_fmt        = amd64_gt_fmt;
    fmts->lt_fmt        = amd64_lt_fmt;
    fmts->plus_fmt      = amd64_plus_fmt;
    fmts->plus_off_fmt  = amd64_plus_off_fmt;
    fmts->minus_fmt     = amd64_minus_fmt;
    fmts->minus_off_fmt = amd64_minus_off_fmt;
    fmts->lb_fmt        = amd64_lb_fmt;
    fmts->rb_fmt        = amd64_rb_fmt;
    fmts->comma_fmt     = amd64_comma_fmt;
    fmts->dot_fmt       = amd64_dot_fmt;
}
