
/*
* Program to learn the idiocincracie of the fftw3 and nfft3 libraries
*/

#include <rfftw.h> // for fft action
#include <stdio.h>

//These required for the signal generator
#include <stdlib.h>//for the rand function set
#include <math.h>
#include <time.h>

double sig_gen(double freq, double t, int nflag);
double get_rand();//for noise
void load_data(char *, int, double*);
double *ufft(fftw_real *in, int N);
double *scale(double *data, int len);
double *magnitude(fftw_real*, int);
double parse_freq(char *);
main(int argc, char **argv)
{
        extern char *optarg;
        extern int optind;
        char *path=NULL;
        int opt, i;
        int ncount = 1024;
	int noise=0;
        double *spectrum, freq=5.0*1e3;
	fftw_real *dp;
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
//		case 'F':
//                        freq=parse_freq(optarg);
                default:
                        fprintf(stderr, "usage: %s -r -n count -f [input file]",
                            argv[0]);
//                        fprintf(stderr, "frequency is in the form; 5Hz, 50MHz");
                        return (-1);
                }
        }
        if (ncount <= 0) {
                fprintf(stderr, "%s: data count must be > 0\n", argv[0]);
                return (-1);
        }

        dp = (fftw_real *)malloc(sizeof (fftw_real) * ncount);
		for(i=0;i<ncount;i++)
			dp[i]=(fftw_real)sig_gen(freq, (double)i,noise);

	spectrum=ufft(dp,ncount);

	for(i=0;i<ncount/2;i++)
		printf("%e\t%e\n",(double)dp[i],spectrum[i]);
	for(;i<ncount;i++)
		printf("%e\n",(double) dp[i]);
}


double * ufft(fftw_real *in,int N){//does the actual transform
	rfftw_plan plan;
	int i;
	fftw_real out[N];

	plan=rfftw_create_plan(N, FFTW_REAL_TO_COMPLEX, FFTW_ESTIMATE);
	rfftw_one(plan, in, out);
	rfftw_destroy_plan(plan);

	return magnitude(out, N);

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

double *magnitude(fftw_real *out, int N){
	int k;
	double power_spectrum[N/2+1];

	power_spectrum[0]=out[0]*out[0];
	for (k=1; k<N;++k)
		power_spectrum[k]=out[k]*out[k]+out[N-k]*out[N-k];
	if(N%2==0)
		power_spectrum[N/2]=out[N/2]*out[N/2];

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
