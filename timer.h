/*
* This is an impromptu header to enabe use of wavemon's timer functions,
* since the struct and functions are declared in wavemon.h. Basically
* just pulling them out and dropping them here.
*/
 #include <time.h>
#ifndef TIMER_H
#define TIMER_H
struct timer{
	unsigned long long 	stime;
	unsigned long		duration;
};

void start_timer(struct timer *t, unsigned long d);
bool end_timer(struct timer *t);
static unsigned long get_usecs();
#endif
