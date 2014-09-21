#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "joining/pktdetection.h"

void main(){
	clock_t c;
	time_t t;

	c=clock();
	time(&t);

	printf("CLOCK_PER_SEC for this system: %l\n", CLOCKS_PER_SEC);
	printf("Detecting packet...\n");
	dtct_pcket("wlan0");
	c=clock()-c;
	printf("Packet received in %d clicks and %f seconds according to clocks\n",c,((float)c)/CLOCKS_PER_SEC);
	printf("Timer gives: %f seconds\n", difftime(t,time(NULL)));
}
