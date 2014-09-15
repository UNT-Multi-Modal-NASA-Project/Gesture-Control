/*
* taking a break from banging my head against the whole rx level problem,
* and thought about taking a reading whenever a packet is recieved rather
* than when the timer runs out.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include "error.h"//testing this too

unsigned long long if_getrxpkt(char *ifname);
double dtct_pcket(int, char*);
void rpt_pcket(double);
char scale(double*);

void main(int argc, char*argv[])
{
	long sample_interval=0, samples=16;
	char *ifname={"wlan0"};
	int i,pkt;

	 for(i=1;i<argc;i++){
                if(strcmp(argv[i],"-s")==0){
                        samples=strtol(argv[++i],NULL,0);
        /*      else*/ if (strcmp(argv[i], "-i")==0)
       		        sample_interval=strtoul(argv[++i], NULL,0);
//              else if (strcmp(argv[i], "-a")==0)
//                        avg_flag=1;
//              else if (argc>0){
                        printf("Usage: pktdetect [-s <# of samples to take> -i <usec between samples> -a]\n");
                        exit(EXIT_SUCCESS);
                }
        }
	for(i=0;i<samples;i++){
		pkt=dtct_pcket(sample_interval,ifname);
		rpt_pcket(pkt);
	}
}
double dtct_pcket(int interval, char* if_nm){
	unsigned long long old=if_getrxpkt(if_nm);
	time_t start, stop;
	time(&start);
	while(if_getrxpkt(if_nm)<=old);
	time(&stop);
	return difftime(start,stop);
}
void rpt_pcket(double recieved){
	char prefix=scale(&recieved);
	printf("Packet recieved in %f %cs\n",recieved, prefix);
}
unsigned long long if_getrxpkt(char *ifname)
{
        char line[0x100];
        unsigned long long d,ret;
        char *lp;
        size_t l = strlen(ifname);
        const char path[] = "/proc/net/dev";
        FILE *fp = fopen(path, "r");

        if (fp == NULL)
        	perror("can not open %s");
        /*
         * Inter-|   Receive                                               $
         *  face |bytes    packets errs drop fifo frame compressed multicas$
         */
        while (fgets(line, sizeof(line), fp)) {
                lp = line + strspn(line, " ");
                if (!strncmp(lp, ifname, l) && lp[l] == ':') {
                        lp += l + 1;
                        lp += strspn(lp, " ");

                        sscanf(lp, "%llu %llu %llu %llu %llu %llu %llu %llu %llu %llu",
                                &d, &ret, &d, &d, &d, &d, &d, &d, &d, &d);
                }
        }
        fclose(fp);
	return ret;
}

char scale(double* in)
{
        if (*in < 0.00000001) {
                (*in)*=1e9;
                return 'p';
        } else if (*in < 0.00001) {
                (*in)*=1e6;
                return 'n';
        } else if (*in < 0.01) {
                (*in)*=1e3;
                return 'u';
        } else {
                return 'm';
        }
}

