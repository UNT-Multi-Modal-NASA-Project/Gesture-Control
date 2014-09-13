/*
* Experimental program for testing the RX power reading from the wireless
* adapter. Uses the general wireless interface defined in wireless.h and
* ioctl. Should be complied with the -lm flag
*/
//selected includes
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <string.h>
#include <memory.h>
#include <sys/socket.h>
#include <linux/wireless.h>
#include <tgmath.h>

#define READ 0
#define UNREAD 1

int u8_to_dbm(const int power);
double dbm2mw(const double in);

void get_samples(char * iwname, int max_samples);
static void iw_getstat(struct iw_statistics *, char *);//actually reads the value

void main(int argc, char*argv[]){
	char *ifname={"wlan0"};
	int i;
	long samples=16;

	for(i=1;i<argc;i++){
		if(strcmp(argv[i],"-s")==0)
			samples=strtol(argv[++i],NULL,0);
	//	else if (strcmp(argv[i], "-i")==0)
	//		sample_interval=strtoul(argv[++i], NULL,0);
		else if (argc>0){
			printf("Usage: pwrsamp [-s <# of samples to take> -i <usec between samples>\n");
			exit(EXIT_SUCCESS);
		}
	}

	//so with this configuration, there should be 16 samples, taken over
	//	16000 usec, or 1.6 msec, or .0016 sec
	get_samples(ifname,samples);
}

//sampler functions
void get_samples(char* ifname,int samples)
{
	struct iw_statistics reading;
	int level, dbm, noise_lvl, noise_dbm;
	double power,noise_pwr;
	int i=0;

	printf("Reading levels:\n");

	while(i<samples)
	{
		iw_getstat(&reading,ifname);
		level=reading.qual.level;
		dbm=u8_to_dbm(level);
		power=dbm2mw(dbm);
		noise_lvl=reading.qual.noise;
		noise_dbm=u8_to_dbm(noise_lvl);
		noise_pwr=dbm2mw(noise_dbm);
		printf("%d\t%d(dbm)\t%.2f(mW)\t%d\t%d(dbm)\t%.2f(mW)\n", level,dbm,power, noise_lvl, noise_dbm, noise_pwr);
		i++;
	}
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
		exit(EXIT_FAILURE);
	}

	/* Perform the ioctl */
	if(ioctl(sockfd, SIOCGIWSTATS, &req) == -1) {
		perror("Error performing SIOCGIWSTATS on wlan0");
		close(sockfd);
		exit(EXIT_FAILURE);
	}

	close(sockfd);
}

int u8_to_dbm(const int power)
{
        return power > 63 ? power - 0x100 : power;
}

/* Convert log dBm values to linear mW */
double dbm2mw(const double in)
{
        return pow(10.0, in / 10.0);
}

