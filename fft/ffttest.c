
/*
* Program to learn the idiocincracie of the fftw3 library
*/

#include <rfftw.h> // for fft action
#include <stdio.h>

//These required for the signal generator
#include <stdlib.h>//for the rand function set
#include <math.h>
#include <time.h>

double sig_gen(double freq, double t, int nflag);
double get_rand();//for noise
//void load_data(char *, int, double*);

double *magnitude(fftw_real*, int);

main(int argc, char **argv)
{
        extern char *optarg;
        extern int optind;
        char *path=NULL;
        int opt, i;
        int ncount = 1024;
	int noise=0;
        double *dp, *spectrum;
        rfftw_plan plan;
        fftw_real *cp;

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
                        fprintf(stderr, "usage: %s -n count -f input_file\n",
                            argv[0]);
                        return (-1);
                }
        }

        if (ncount <= 0) {
                fprintf(stderr, "%s: data count must be > 0\n", argv[0]);
                return (-1);
        }

        dp = (double *)malloc(sizeof (double) * ncount);
        cp = (fftw_real *)fftw_malloc(sizeof (fftw_real) * ncount);
//      memset(dp, 0, sizeof (double) * ncount);
//      memset(cp, 0, sizeof (fftw_complex) * ncount);
        plan = rfftw_create_plan(ncount, FFTW_REAL_TO_COMPLEX, FFTW_ESTIMATE);
//	if(path==NULL){
		for(i=0;i<ncount;i++)
			dp[i]=sig_gen(5.0, (double)i,noise);
//	}
//	else
//	        load_data(path, ncount, dp);

	rfftw_one(plan, dp, cp);//actually doing the dft
	spectrum=magnitude(cp, ncount);

	for(i=0;i<ncount/2;i++)
		printf("%e\t%e\n",dp[i],spectrum[i]);
	for(;i<ncount;i++)
		printf("%e\n", dp[i]);

	rfftw_destroy_plan(plan);
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

double *magnitude(fftw_real* out, int N){
	int k;
	double power_spectrum[(N+1)/2];

	power_spectrum[0] = (double)out[0]*(double)out[0];  /* DC component */
	for (k = 1; k < (N+1)/2; ++k)  /* (k < N/2 rounded up) */
        	power_spectrum[k] = (double)out[k]*(double)out[k] +(double) out[N-k]*(double)out[N-k];
	if (N % 2 == 0) /* N is even */
        	power_spectrum[N/2] = (double)out[N/2]*(double)out[N/2];  /* Nyquist freq. */
	return power_spectrum;
}
