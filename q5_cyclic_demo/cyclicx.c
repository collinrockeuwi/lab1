/*
 * Demo table-based cyclic task dispatcher for Linux with multiple task slots
 * Student: Collin Rocke (ID: XXXXX)  -- base code copied from Williams Ch4 §4.7
 */

#include <stdio.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/times.h>      /* build: gcc cyclicx.c -o cyclicx */

#define SLOTX   4
#define CYCLEX  5
#define SLOT_T  5000        /* 5 sec slot time (ms) */

int tps, cycle = 0, slot = 0;
clock_t now, then;
struct tms n;

/* task code */
void one(void)   { printf("task 1 running\n"); sleep(1); }
void two(void)   { printf("task 2 running\n"); sleep(2); }
void three(void) { printf("task 3 running\n"); sleep(3); }
void four(void)  { printf("task 4 running\n"); sleep(4); }
void five(void)  { printf("task 5 running\n"); sleep(5); }

void burn(void)
{
    clock_t bstart = times(&n);
    while ( ((now = times(&n)) - then) < SLOT_T * tps / 1000 ) {
        /* burn time here */
    }
    printf("burn time = %2.2dms\n\n", (int)((times(&n) - bstart) * 1000 / tps));
    then  = now;
    cycle = CYCLEX;
}

/* task table: rows = slots, columns = 5-sec cycles */
void (*ttable[SLOTX][CYCLEX])(void) = {
    { one,   two,   burn, burn, burn },
    { one,   three, burn, burn, burn },
    { one,   four,  burn, burn, burn },
    { burn,  burn,  burn, burn, burn }
};

int main(void)
{
    tps = sysconf(_SC_CLK_TCK);
    printf("clock ticks/sec = %d\n\n", tps);

    while (1) {
        for (slot = 0; slot < SLOTX;  ++slot)
            for (cycle = 0; cycle < CYCLEX; ++cycle)
                (*ttable[slot][cycle])();     /* dispatch next task from table */
    }
    return 0;
}
