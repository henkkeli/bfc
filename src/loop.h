#ifndef LOOP_H
#define LOOP_H

struct loopstack {
    int n;
    struct loopstack *prev;
};

int begin_loop(struct loopstack **top, int *count);
int end_loop(struct loopstack **top);

#endif /* LOOP_H */
