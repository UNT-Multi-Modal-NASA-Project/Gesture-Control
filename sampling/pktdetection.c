//packet detection functions

#include "pktdetection.h"

unsigned long long if_getrxpkt(char *ifname)
{
        char line[0x100];
        unsigned long long d,ret;
        char *lp;
        size_t l = strlen(ifname);
        const char path[] = "/proc/net/dev";
        FILE *fp = fopen(path, "r");

        if (fp == NULL)
        	perror("can not open /proc/net/dev");
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
	if(*in>1e12)
	{
		*in/=1e12;
		return 'T';
	}
	else if(*in>1e9)
	{
		*in/=1e9;
		return 'G';
	}
	else if (*in >1e6)
	{
		*in/=1e6;
		return 'M';
	}
	else if (*in >1e3)
	{
		*in/=1e3;
		return 'k';
	}
	else if (*in >.001)
	{
		*in*=1e3;
		return 'm';
	}
	else if (*in >.000001)
	{
		*in*=1e6;
		return 'u';
	}
	else if (*in >000000001)
	{
		*in*=1e9;
		return 'n';
	}
	else if (*in >000000000001)
	{
		*in/=1e12;
		return 'p';
	}
	else
		return;
}
double dtct_pcket(char* if_nm){
	unsigned long long old=if_getrxpkt(if_nm);
	clock_t start, stop;
	start=clock();
	while(if_getrxpkt(if_nm)<=old);
	stop=clock();
	return ((double)(stop-start))/CLOCKS_PER_SEC;//seconds
}
void rpt_pcket(double recieved){
	char prefix=scale(&recieved);
	printf("Packet recieved in %f %cs\n",recieved, prefix);
}
