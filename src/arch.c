#include "common.h"
#include "arch/amd64.h"
#include "arch/armv6l.h"
#include <stdlib.h>
#include <string.h>
#include <sys/utsname.h>

int set_arch(struct options *opts)
{
    struct utsname buf;
    uname(&buf);

    if (opts->arch == NULL)
        opts->arch = strdup(buf.machine);

    if (strcmp(opts->arch, buf.machine) == 0)
        opts->native = 1;
    
    if (strcmp(opts->arch, "x86_64") == 0)
    {
        opts->set_fmts = &amd64_set_fmts;
    }
    else if (strcmp(opts->arch, "armv6l") == 0)
    {
        opts->set_fmts = &armv6l_set_fmts;
    }
    else
    {
        /* architecture not supported */
        return -1;
    }

    return 0;
}
