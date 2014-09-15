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
        if (*in < 0.00000001)
	{
                (*in)*=1e9;
                return 'p';
        }
	 else if (*in < 0.00001)
	{
                (*in)*=1e6;
                return 'n';
        }
	else if (*in < 0.01)
	{
                (*in)*=1e3;
                return 'u';
        }
	else {
                return 'm';
        }
}
double dtct_pcket(char* if_nm){
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
