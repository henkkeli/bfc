#include "loop.h"
#include <stdlib.h>

int begin_loop(struct loopstack **top, int *count)
{
    struct loopstack *loop = (struct loopstack *) malloc(sizeof(struct loopstack));
    loop->n = *count;
    loop->prev = *top;
    *top = loop;
    return (*count)++;
}

int end_loop(struct loopstack **top)
{
    if (*top == NULL)
        return -1;

    int res = (*top)->n;
    struct loopstack *tmp = *top;
    *top = (*top)->prev;
    free(tmp);
    return res;
}
