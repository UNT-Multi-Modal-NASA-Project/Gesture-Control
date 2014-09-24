
/*
* Program to learn the idiocincracie of the fftw3 library
*/

//#include <rfftw.h> // for fft action
#include <stdio.h>

//These required for the signal generator
#include <stdlib.h>//for the rand function set
#include <math.h>
#include <time.h>
//These required for nfft
//#ifdef HAVE_COMPLEX_H
#include <complex.h>
//#endif
#include "config.h"
#include "nfft3util.h"
#include "nfft3.h"
#include "infft.h"

double sig_gen(double freq, double t, int nflag);
double get_rand();//for noise
//void load_data(char *, int, double*);
//double *ufft(double *in, int N);
double *nufft(double *in, int N);
double *scale(double *data, int len);
//double *magnitude(double*, int);
double *magnitude(double complex*, int);

main(int argc, char **argv)
{
        extern char *optarg;
        extern int optind;
        char *path=NULL;
        int opt, i;
        int ncount = 1024;
	int noise=0;
        double *dp, *spectrum;
	srand(time(NULL));

        while ((opt = getopt(argc, argv, "in:")) != -1) {
                switch (opt) {
                case 'n':
                        ncount = atoi(optarg);
                        break;
		case 'f':
		        path =optarg;
			break;
		case 'r':
			noise=1;
			break;
                default:
                        fprintf(stderr, "usage: %s -r -n count -f input_file\n",
                            argv[0]);
                        return (-1);
                }
        }

        if (ncount <= 0) {
                fprintf(stderr, "%s: data count must be > 0\n", argv[0]);
                return (-1);
        }

        dp = (double *)malloc(sizeof (double) * ncount);
		for(i=0;i<ncount;i++)
			dp[i]=sig_gen(5.0*1e3, (double)i,noise);

	spectrum=nufft(dp,ncount);

	for(i=0;i<ncount/2;i++)
		printf("%e\t%e\n",dp[i],spectrum[i]);
	for(;i<ncount;i++)
		printf("%e\n", dp[i]);
}


double * nufft(double *in,int N){
	nfft_plan plan;
	int i;
	double *out;

	nfft_init_1d(&plan,N,N);

	for(i=0;i<N;i++)
		plan.x[i]=in[i];

	if(plan.nfft_flags & PRE_ONE_PSI) nfft_precompute_one_psi(&plan);

	nfft_trafo(&plan);
	out=magnitude(plan.f, N);
	nfft_finalize(&plan);

	return out;
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

double *magnitude(double complex *out, int N){
	int k;
	double power_spectrum[N];

	power_spectrum[0]=creal(out[0])*creal(out[0]);
	for (k=1; k<N;++k)
		power_spectrum[k]=creal(out[k])*creal(out[k])+cimag(out[k])*cimag(out[k]);
	if(N%2==0)
		power_spectrum[N/2]=creal(out[N/2])*creal(out[N/2]);

	return power_spectrum;
}

double* scale(double *data, int len){
	double min=1, max=0;
	int i;

	for(i=0;i<len;i++){
		if(data[i]>max) max=data[i];
		if(data[i]<min) min=data[i];
	}
	for(i=0;i<len;i++)
		data[i]=data[i]/(max-min);

	return data;
}
