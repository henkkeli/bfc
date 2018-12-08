#ifndef ARCH_H
#define ARCH_H

#include "common.h"

/**
 * @brief sets target architecture and some architecture specific options
 *
 * defaults to native. in case of native arch, also sets native flag.
 *
 * @param opts options structure
 *
 * @return 0 on success, -1 if architecture not supported
 */
int set_arch(struct options *opts);

#endif /* ARCH_H */
