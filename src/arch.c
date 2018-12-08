#include "common.h"
#include "arch/amd64.h"
#include <stdlib.h>
#include <string.h>
#include <sys/utsname.h>

int set_arch(struct options *opts)
{
    if (opts->arch == NULL)
    {
        struct utsname buf;
        uname(&buf);
        opts->arch = buf.machine;
    }
    
    if (strcmp(opts->arch, "x86_64") == 0)
    {
        opts->set_fmts = &amd64_set_fmts;
    }
    else
    {
        /* architecture not supported */
        return -1;
    }

    return 0;
}
