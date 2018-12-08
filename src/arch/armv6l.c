#include "arch/armv6l.h"
#include "common.h"
#include <stdlib.h>

const char *armv6l_init_fmt = 
    "\t.globl\t%1$s\n"
    ".balign 4\n"
    "\t.lcomm\tbuf, %2$d\n"
    ".rbuf:\n"
    "\t.word\tbuf-(.rload+8)\n"    /* PC-relative offset */
    "\t.text\n"
    ".balign 4\n"
    "%1$s:\n"
    "\tpush\t{fp, lr}\n"
    "\tadd\tfp, sp, #4\n"
    "\tldr\tr3, .rbuf\n"           /* init cell pointer */
    ".rload:\n"
    "\tadd\tr3, pc, r3\n";         /* fix offset */

const char *armv6l_exit_fmt =
    "\tmov\tr0, #0\n"              /* return value */
    "\tsub\tsp, fp, #4\n"
    "\tpop\t{fp, pc}\n";           /* exit by popping PC */

const char *armv6l_read_fmt =
    ".read:\n"
    "\tmov\tr7, #3\n"              /* syscall read */
    "\tmov\tr0, #%d\n"             /* fd */
    "\tmov\tr1, r3\n"              /* buf */
    "\tmov\tr2, #1\n"              /* count */
    "\tswi\t0\n"
    "\tbx\tlr\n";

const char *armv6l_write_fmt =
    ".write:\n"
    "\tmov\tr7, #4\n"              /* syscall write */
    "\tmov\tr0, #%d\n"             /* fd */
    "\tmov\tr1, r3\n"              /* buf */
    "\tmov\tr2, #1\n"              /* count */
    "\tswi\t0\n"
    "\tbx\tlr\n";

const char *armv6l_gt_fmt =
    "\tadd\tr3, r3, #%d\n";

const char *armv6l_lt_fmt =
    "\tsub\tr3, r3, #%d\n";

const char *armv6l_plus_fmt =
    "\tldrb\tr2, [r3]\n"
    "\tadd\tr2, r2, #%d\n"
    "\tstrb\tr2, [r3]\n";

const char *armv6l_minus_fmt =
    "\tldrb\tr2, [r3]\n"
    "\tsub\tr2, r2, #%d\n"
    "\tstrb\tr2, [r3]\n";

const char *armv6l_lb_fmt =
    ".LB%1$d:\n"
    "\tldrb\tr2, [r3]\n"
    "\tcmp\tr2, #0\n"
    "\tbeq\t.LE%1$d\n";

const char *armv6l_rb_fmt =
    "\tb\t.LB%1$d\n"
    ".LE%1$d:\n";

const char *armv6l_comma_fmt =
    "\tbl\t.read\n";

const char *armv6l_dot_fmt =
    "\tbl\t.write\n";

void armv6l_set_fmts(struct formats *fmts)
{
    fmts->init_fmt      = armv6l_init_fmt;
    fmts->exit_fmt      = armv6l_exit_fmt;
    fmts->read_fmt      = armv6l_read_fmt;
    fmts->write_fmt     = armv6l_write_fmt;
    fmts->gt_fmt        = armv6l_gt_fmt;
    fmts->lt_fmt        = armv6l_lt_fmt;
    fmts->plus_fmt      = armv6l_plus_fmt;
    fmts->plus_off_fmt  = NULL;
    fmts->minus_fmt     = armv6l_minus_fmt;
    fmts->minus_off_fmt = NULL;
    fmts->lb_fmt        = armv6l_lb_fmt;
    fmts->rb_fmt        = armv6l_rb_fmt;
    fmts->comma_fmt     = armv6l_comma_fmt;
    fmts->dot_fmt       = armv6l_dot_fmt;
}
