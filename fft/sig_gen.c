#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>

double sig_gen(double freq, double t, int noiseflg);
double get_rand();

void main(int argc, char *argv[]){

	int now, max_time=1024,nflg=0;
	double frequency=5.0, *signal;
	srand(time(NULL));
	if(argc>4)
	{
		printf("Usage: sig_gen [max_time][noise][freq]\n");
		return;
	}
	else if(argc>=2)
		max_time=strtol(argv[1], NULL, 0);
	else if(argc>=3)
		nflg=strtol(argv[2], NULL, 0);
	else if (argc==4)
		frequency=strtod(argv[3],NULL)*1e9;
	signal=malloc(max_time*sizeof(double));

	for(now=0;now<max_time;now++)
		signal[now]=sig_gen(frequency,((double)now)/1e3,nflg);

	printf("signal generated: \n");
	for(now=0;now<max_time;now++)
		printf("%f\n",30*signal[now]);
	printf("\n");
}

double sig_gen(double freq, double t, int noiseflg)
{
	double noise=0;
        if(noiseflg) noise=get_rand();
        return sin(2*M_PI*freq*t)+noise;
}
double get_rand(){
	return(double)(rand()%10)*1e9;
}
