/*
* Second attempt at an RX power sampling program. This one joins two other
* tools: the pwrsamptest0 and the pktdetect programs.
*
* Should be complied with the -lm flag
*
* The point of this program is to reduce the number of identical samples
* reported.
*/
//selected includes
#include <stdio.h>
#include <stdlib.h>
#include "sampling.h"
#include "pktdetection.h"

void main(int argc, char*argv[]){
	char *ifname={"wlan0"};
	char sec;
	int i, avg_flag=0;
	long samples=16;
	struct reading rdg;
	double mavg=0,decay=0.1;

	for(i=1;i<argc;i++){
		if(strcmp(argv[i],"-s")==0)
			samples=strtol(argv[++i],NULL,0);
		else if (strcmp(argv[i], "-a")==0)
			avg_flag=1;
		else if (argc>0 && strcmp(argv[i],"-h")==0){
			printf("Usage: pwrsamp [-s <# of samples to take>  -a\n");
			exit(EXIT_SUCCESS);
		}
	}

	i=0;
	while(i<samples)
	{
		rpt_pcket(dtct_pcket(ifname));
		rdg=get_sample(ifname,samples, avg_flag);
		mavg=ewma(mavg,rdg.pwr,decay);
		rdg.prefix=scale(&(rdg.pwr));
		put_sample(&rdg);
		i++;
	};
	if(avg_flag)
	{
		sec=scale(&mavg);
		printf("Samples: %d, Avg power: %.3f %cW\n",i,mavg,sec);
	}
}
