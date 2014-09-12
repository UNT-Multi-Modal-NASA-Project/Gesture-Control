/*
* Experimental program for testing the RX power reading from the wireless
* adapter. Uses the general wireless interface defined in wireless.h and
* ioctl. Should be compliled with the -lpthread flag.
*/
//for input/output
#include <stdio.h>
#include <stdlib.h>
#include <ioctl.h>

//taken directly from wavemon's iw_if.h, may need to trim down
//#include <netdb.h>//
#include <pthread.h>
//#include <arpa/inet.h>//
#include <netinet/in.h>
//#include <netinet/ether.h>//
//#include <net/if_arp.h>//
//#include <net/ethernet.h>//
#include <sys/socket.h>
#include <linux/if.h>
#include <linux/wireless.h>
#include <uapi/linux/sockios.h>
/*
* taken from example code that does similar stuff
* #include <linux/wireless.h>
* #include <sys/types.h>
* #include <sys/socket.h>
*/

//code segments taken from wavemon.h
#include "timer.h"

//thread functions
void *get_samples(void* buffer);
void *put_samples(void* buffer);

//wavemon utility functions
//device information
static int if_get_flags(int skfd, const char *ifname);
int if_is_up(const char*ifname);

static void iw_getstat(struct iw_statistics *, char *);//actually reads the value

pthread_mutex_t bufmut; // buffer's mutex

struct sample_buffer{
	char* iwname;
	int* readings;
	int max_samples;
	int last_sample;
	unsigned long time_between_samples;
	struct timer *alarm;
};

void main(int argc, char*argv[]){
	//parse arguments, usage will include options for limiting samples
	// 	and total duration, maybe sample interval too. . .

	//TODO: just for robustness, check for multiple wireless adapters
	//For now, just get wlan0
	char *ifname={"wlan0"};

	if(if_is_up(ifname)<0){
		perror("cannot poll, wlan0 interface is down");
		pthread_exit(NULL);
	} // if your gonna fail, fail early

	pthread_t sampler, reader;
	pthread_attr_t attr;
	void * thread_status;

	struct sample_buffer buffer;

	//buffer's inhabitants
	struct timer coordinator;
	int samples=16;
	unsigned long sample_interval=1000; // in usec
	//so with this configuratin, there should be 16 samples, taken over
	//	16000 usec, or 1.6 msec, or .0016 sec

	//initialize buffer
	buffer.iwname=ifname;
	buffer.readings=malloc(samples*sizeof(int));
	buffer.max_samples=samples;
	buffer.last_sample=-1;
	buffer.time_between_samples=sample_interval;
	buffer.alarm=&coordinator;

	//initialize threads
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
	pthread_mutex_init(&bufmut, NULL);

	//start the timer and the threads
	start_timer(&coordinator,sample_interval);
	pthread_create(&sampler, &attr, get_samples, (void *)&buffer);
	pthread_create(&reader, &attr, put_samples, (void *)&buffer);

	//close up threads and associates
	pthread_attr_destroy(&attr);

	pthread_join(sampler,&thread_status);
	pthread_join(reader,&thread_status);

	free(buffer.readings);
	pthread_mutex_destroy(&bufmut);
	pthread_exit(NULL);
}

void *put_samples(void* args)
{

	struct sample_buffer *buffer=(struct sample_buffer *)args;

	while(buffer->last_sample< buffer->max_samples)
	{
		if(mutex_trylock(&bufmut)==0 && !end_timer(buffer->alarm))
		{
			if(buffer->last_sample>-1)//nothing's been written to the buffer yet
				printf("%d\t",buffer->readings[buffer->last_sample]);
		}
	}
	printf("\n");
}

//sampler functions
void *get_samples(void* args)
{
	struct sample_buffer *buffer=(struct sample_buffer *)args;
	struct iw_statistics reading;

	while(buffer->last_sample < buffer->max_samples)
	{
		if(end_timer(buffer->alarm))
		{
			iw_getstat(&reading,buffer->iwname);
			mutex_lock(&bufmut);
			buffer->last_sample++;
			buffer->readings[buffer->last_sample]=reading.qual.level;
			start_timer(buffer->alarm,buffer->time_between_samples);
			mutex_unlock(&bufmut);
		}
	}
	pthread_exit(NULL);
}

static void iw_getstat(struct iw_statistics *stats, char* iwname){
	int sockfd;
	struct iwreq req = {
		.ifr_name = iwname,
		.u.data = {
			.pointer = stats,
#ifdef CLEAR_UPDATED
			.flags = 1
#endif
		}
	};

	/* Any old socket will do, and a datagram socket is pretty cheap */
	if((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
		perror("Could not create simple datagram socket");
		pthread_exit(NULL);
	}

	/* Perform the ioctl */
	if(ioctl(sockfd, SIOCGIWSTATS, &req) == -1) {
		perror("Error performing SIOCGIWSTATS on ");
		close(sockfd);
		pthread_exit(NULL);
	}

	close(sockfd);
}

static int if_get_flags(int skfd, const char *ifname){
	struct ifreq ifr;

	memset(&ifr,0, sizeof(struct ifreq));
	strncpy(ifr.ifr_name,ifname,sizeof(ifr.ifr_name)-1);

	if(ioctl(skfd, SIOCGIFFLAGS, &ifr)<0)
		err_sys("can not get interface flags for %s", ifname);
	return ifr.ifr_flags;
}

int if_is_up(const char*ifname)
{
	int skfd;
	return if_get_flags(skfd, ifname) & IFF_UP;
}
