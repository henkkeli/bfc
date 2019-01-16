#include "loop.h"
#include <stdlib.h>

int begin_loop(struct loopstack **top, int *count)
{
    struct loopstack *loop = malloc(sizeof(struct loopstack));
    loop->n = *count;
    loop->prev = *top;
    *top = loop;
    return (*count)++;
}

int end_loop(struct loopstack **top)
{
    if (loopstack_empty(*top))
        return -1;

    int res = (*top)->n;
    struct loopstack *tmp = *top;
    *top = (*top)->prev;
    free(tmp);
    return res;
}

int loopstack_empty(struct loopstack *top)
{
    return top == NULL;
}
