/*
* Experimental program for testing the RX power reading from the wireless
* adapter. Uses the general wireless interface defined in wireless.h and
* ioctl. Should be compliled with the -lpthread flag.
*/
//selected includes
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <pthread.h>
#include <string.h>
#include <memory.h>
#include <sys/socket.h>
#include <linux/if.h>
#include <linux/wireless.h>
#include <math.h>
#include "timer.h" //code segments taken from wavemon.h

/*
* taken directly from wavemon's iw_if.h, may need to trim down
* #include <netdb.h>
* #include <arpa/inet.h>
* #include <netinet/in.h>
* #include <netinet/ether.h>
* #include <net/if_arp.h>
* #include <net/ethernet.h>

* #include <uapi/linux/sockio.h> //variant trying to get an ioctl cmd

* taken from example code that does similar stuff
* #include <linux/wireless.h>
* #include <sys/types.h>
* #include <sys/socket.h>
*/

#define READ 0
#define UNREAD 1

//thread functions
void *get_samples(void* buffer);
void *put_samples(void* buffer);

//value conversion functions
static inline int u8_to_dbm(const int power)
{
	return power > 63 ? power - 0x100 : power;
}

/* Convert log dBm values to linear mW */
static inline double dbm2mw(const double in)
{
	return pow(10.0, in / 10.0);
}
//wavemon utility functions
//device information
static int if_get_flags(int skfd, const char *ifname);
int if_is_up(const char*ifname);

static void iw_getstat(struct iw_statistics *, char *);//actually reads the value

pthread_mutex_t bufmut; // buffer's mutex

struct sample_buffer{
	char* iwname;
	double* readings;
	int max_samples;
	int last_sample;
	int read_state;
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
	buffer.readings=malloc(sizeof(double)*(samples));
	buffer.max_samples=samples;
	buffer.last_sample=0;
	buffer.read_state=READ;
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

//	printf("Attempting to release the buffer now\n");

	free(buffer.readings);
	pthread_mutex_destroy(&bufmut);
	pthread_exit(NULL);
}

void *put_samples(void* args)
{

	struct sample_buffer *buffer=(struct sample_buffer *)args;

	while(buffer->last_sample< buffer->max_samples)
	{
		pthread_mutex_trylock(&bufmut);
		if(!end_timer(buffer->alarm))//between readings
		{
			if(buffer->read_state==UNREAD)//don't re-read already read data
			{
				printf("%f\t",buffer->readings[buffer->last_sample]);
				buffer->read_state=READ;
			}
		}
		pthread_mutex_unlock(&bufmut);
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
//			printf("Attempting a reading from %s\n", buffer->iwname);
			pthread_mutex_lock(&bufmut);
			buffer->read_state=UNREAD;
			buffer->readings[buffer->last_sample]=u8_to_dbm(reading.qual.level);
			buffer->last_sample++;
			start_timer(buffer->alarm,buffer->time_between_samples);
			pthread_mutex_unlock(&bufmut);
		}
	}
	pthread_exit(NULL);
}

static void iw_getstat(struct iw_statistics *stats, char* iwname){
	int sockfd;
	struct iwreq req;

	req.u.data.pointer=(caddr_t)stats;
	req.u.data.length=sizeof (*stats);
	req.u.data.flags=0;
	strncpy(req.ifr_name,iwname,IFNAMSIZ);

	/* Any old socket will do, and a datagram socket is pretty cheap */
	if((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
		perror("Could not create simple datagram socket");
		pthread_exit(NULL);
	}

	/* Perform the ioctl */
	if(ioctl(sockfd, SIOCGIWSTATS, &req) == -1) {
		perror("Error performing SIOCGIWSTATS on wlan0");
		close(sockfd);
		pthread_exit(NULL);
	}

	close(sockfd);
}

static int if_get_flags(int skfd, const char *ifname){
	struct ifreq ifr;

	memset(&ifr,0, sizeof(struct ifreq));
	strcpy(ifr.ifr_name,ifname);
	if((skfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
		perror("Could not create simple datagram socket");
		return ifr.ifr_flags;
	}

	if(ioctl(skfd, SIOCGIFFLAGS, &ifr)<0)
		perror("can not get interface flags for wlan0");
	return ifr.ifr_flags;
}

int if_is_up(const char*ifname)
{
	int skfd;
	return if_get_flags(skfd, ifname) & IFF_UP;
}
