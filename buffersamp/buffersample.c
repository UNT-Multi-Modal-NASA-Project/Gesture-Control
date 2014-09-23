/*
* Experimental program for testing the RX power reading from the wireless
* adapter. Uses the general wireless interface defined in wireless.h and
* ioctl. Should be compliled with the -lpthread amd -lm flags.
*/
//selected includes
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <time.h>
//custom headers
#include "sampling.h"
#include "pktdetection.h"

//buffer read state definitions
#define READ 0
#define UNREAD 1

//thread functions
void *make_samples(void* buffer);
void *put_samples(void* buffer);

double get_randval();

pthread_mutex_t bufmut; // buffer's mutex

struct sample_buffer{
	char* iwname;
	struct reading* rdgs;//might do better to use a stack
	int max_samples;
	int last_sample;
	int last_read_sample;
	int read_state;
};

void main(int argc, char*argv[]){

	int i;
	pthread_t sampler, reader;
	pthread_attr_t attr;
	void * thread_status;
	struct sample_buffer buffer;

	//buffer's inhabitants
	int samples=16;
	//so with this configuratin, there should be 16 samples, taken over
	// 16000 usec, or 1.6 msec, or .0016 sec
	for(i=1;i<argc;i++){
		if(strcmp(argv[i],"-s")==0)
                	samples=strtol(argv[++i],NULL,0);
                else if (argc>0){
                        printf("Usage: pwrsamp [-s <# of samples to take>]\n");
                        pthread_exit(NULL);
                }
        }

	//initialize buffer
	buffer.iwname="wlan0";
	buffer.rdgs=malloc(sizeof(struct reading)*(samples));
	buffer.max_samples=samples;
	buffer.last_sample=0;
	buffer.last_read_sample=0;
	buffer.read_state=READ;

	//initialize threads
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
	pthread_mutex_init(&bufmut, NULL);

	//start the timer and the threads
	pthread_create(&sampler, &attr, make_samples, (void *)&buffer);
	pthread_create(&reader, &attr, put_samples, (void *)&buffer);

	//close up threads and associates
	pthread_attr_destroy(&attr);
	pthread_join(sampler,&thread_status);
	pthread_join(reader,&thread_status);

	// printf("Attempting to release the buffer now\n");
	free(buffer.rdgs);
	pthread_mutex_destroy(&bufmut);
	pthread_exit(NULL);
}
//buffer reader function
void *put_samples(void* args)
{
	struct sample_buffer *buffer=(struct sample_buffer *)args;


	while (buffer->last_sample< buffer->max_samples){
		pthread_mutex_lock(&bufmut);
//		printf("Reader locking mutex\n");
		if(buffer->read_state==UNREAD){
			for(;buffer->last_read_sample< buffer->last_sample;buffer->last_read_sample++)
				printf("Read value: %.4f %cW from position %d\n",buffer->rdgs[buffer->last_read_sample].pwr, buffer->rdgs[buffer->last_read_sample].prefix, buffer->last_read_sample);
			buffer->read_state=READ;
		}
//		printf("Reader unlocking mutex\n");
		pthread_mutex_unlock(&bufmut);
	}
	printf("\n");
	pthread_exit(NULL);
}
//buffer writer function
void *make_samples(void* args)
{
	struct sample_buffer *buffer=(struct sample_buffer *)args;
	double pkt_time;

	srand(time(NULL));

	while(buffer->last_sample < buffer->max_samples)
	{
			pkt_time=dtct_pcket(buffer->iwname);
			pthread_mutex_lock(&bufmut);
//			printf("Writer locking mutex\n");
			buffer->read_state=UNREAD;
			buffer->rdgs[buffer->last_sample]=get_sample(buffer->iwname);
			buffer->rdgs[buffer->last_sample].prefix=scale(&(buffer->rdgs[buffer->last_sample].pwr));
			printf("Wrote value %f %cW to position %d with interval %fs\n",buffer->rdgs[buffer->last_sample].pwr, buffer->rdgs[buffer->last_sample].prefix,buffer->last_sample,pkt_time);
			buffer->last_sample++;
//			printf("Writer unlocking mutex\n");
			pthread_mutex_unlock(&bufmut);
	}
	pthread_exit(NULL);
}
