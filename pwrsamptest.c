/*
* Experimental program for testing the RX power reading from the wireless
* adapter. Uses the general wireless interface defined in wireless.h and
* ioctl. Should be compliled with the -lpthread flag.
*/
//for input/output
#include <stdio.h>
#include <stdlib.h>

//taken directly from wavemon's iw_if.h
#include <netdb.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/ether.h>
#include <net/if_arp.h>
#include <net/ethernet.h>
#include <sys/socket.h>
#include <linux/if.h>
#include <linux/wireless.h>

//code segments taken from wavemon.h
#include "timer.h"

//thread functions
void *get_samples(void* buffer);
void *put_samples(void* buffer);

//wavemon utility functions
//device information
static int if_get_flags(int skfd, const char *ifname);
bool if_is_up(int skfd, const char*ifname);

static void iw_getstat(struct iw_levelstat *);//actually reads the value

struct sample_buffer{
	int* buffer;
	unsigned long time_between_samples;
	struct timer *alarms;
};

void main(int argc, char*argv[]){
	//parse arguments, usage will include options for limiting samples
	// 	and total duration, maybe sample interval too. . .

	//TODO: just for robustness, check for multiple wireless adapters
	//For now, just get wlan0
	char *ifname={"wlan0"};
	if(if_is_up(ifname)<0)
		err_sys("%s: cannot poll if interface is down");
