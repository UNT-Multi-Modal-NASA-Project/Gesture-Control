/*
* Experimental program for testing the RX power reading from the wireless
* adapter. Uses the general wireless interface defined in wireless.h and
* ioctl. Should be complied with the -lm flag
*
* The point of this code is to figure out how to get level information.
*/
//selected includes
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sampling.h"
#include "pktdetection.h"

void main(int argc, char*argv[]){
	char *ifname={"wlan0"};
	int i, avg_flag,decay=10;
	long samples=16;
	struct reading rdg;
	double mavg;

	for(i=1;i<argc;i++){
		if(strcmp(argv[i],"-s")==0)
			samples=strtol(argv[++i],NULL,0);
	//	else if (strcmp(argv[i], "-i")==0)
	//		sample_interval=strtoul(argv[++i], NULL,0);
		else if (strcmp(argv[i], "-a")==0)
			avg_flag=1;
		else if (argc>0){
			printf("Usage: pwrsamp [-s <# of samples to take> -i <usec between samples> -a\n");
			exit(EXIT_SUCCESS);
		}
	}

	//so with this configuration, there should be 16 samples, taken over
	//	16000 usec, or 1.6 msec, or .0016 sec
	i=0;
	while(i<samples)
	{
		rdg=get_sample(ifname,samples, avg_flag);
		mavg=ewma(mavg,rdg.pwr,decay/100.0);
		put_sample(&rdg);
		i++;
	};
	printf("Samples: %d, Avg power: %f\n",i,mavg);
}

