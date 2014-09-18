/*
* Moving the working functions from pwrsamptest0.c into a header file
*/
#include "sampling.h"

int u8_to_dbm(const int power);
double dbm2mw(const double in);
//char scale(double*); // repeated in pktdetection.h
double ewma(double, double, double);
static void iw_getstat(struct iw_statistics *, char *);//actually reads the value

//sampler functions
struct reading get_sample(char* ifname,int samples,int flag)
{
	struct iw_statistics reading;
	int level, dbm;
	double power, decay=10,avg_pwr=0, mavg=0;
	char unit;
	struct reading sample;

//	printf("Reading levels:\n");

	iw_getstat(&reading,ifname);
	level=reading.qual.level;
	dbm=u8_to_dbm(level);
	power=dbm2mw(dbm);
//	unit=scale(&power);
//	printf("%d\t%d(dbm)\t%f(%cW)\t%f\t\t %s\n", level,dbm,power,unit,mavg,(reading.qual.updated & IW_QUAL_LEVEL_UPDATED ? "updated":""));

	sample.pwr=power;
	sample.prefix=unit;

	return sample;
}
void put_sample(struct reading *rd)
{
	printf("Reading recieved: %f  %cW\n", rd->pwr,rd->prefix);
}
static void iw_getstat(struct iw_statistics *stats, char* iwname){
	int sockfd;
	struct iwreq req;

	req.u.data.pointer=(caddr_t)stats;
	req.u.data.length=sizeof (*stats);
	req.u.data.flags=1;
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

double ewma(double avg, double sample, double weight)
{
	if( avg==0)
		return sample;
	else
		return (weight * avg + (1.0-weight)*sample);
}
