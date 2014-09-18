/*
* Program to learn the idiocincracie of the fftw3 library
*/

#include <rfftw.h>
#include <stdio.h>
#include <stdlib.h>//for the rand function set
#include <memory.h>
#include <time.h>

double get_rand();
void fft(fftw_real*, int, double **);
void printfft(double **, int);
int findpeaks(double **, int, double **);
void printpeaks(double**,int);
double gensig(double freq, int t,int noiseflg);
double pwr_spect(double transform, int len);
//double *windowHann(double* sig);
void main(){
	int i,N=5000,p;
	fftw_real in[N];
	double **power_spectrum, **peaks;

	srand(time(NULL));
	power_spectrum=malloc((N/2+1)*sizeof(double*));
	peaks=malloc((N/2+1)*sizeof(double*));

	for (i=0;i<(N/2+1);i++){
		power_spectrum[i]=malloc(2*sizeof(double));
		peaks[i]=malloc(2*sizeof(double));
		peaks[i][1]=-1;
	}

	for(i=0;i<N;i++)
		in[i]=gensig(get_rand(),0);
	fft(in,N, power_spectrum);
//	printfft(power_spectrum,N/2+1);
	p=findpeaks(power_spectrum,N/2+1,peaks);
	printpeaks(peaks,p);
}
double gensig(double freq, int t,int noiseflg)
{
	double noise=0;
	if(noiseflg) noise=get_rand();
	return sin(2*M_PI*freq*t)+noise;
}
double get_rand(){
	return (double)(rand()%10000000)/1e6;
}
void fft(fftw_real *in, int N, double **power_spectrum){
	fftw_real out[N];
	fftw_plan p;
	int n;

	p=fftw_plan_r2r_1d(N, in, out,FFTW_R2HC,FFTW_ESTIMATE);

	fftw_destroy_plan(p);
}
void printfft(double **spectrum, int N){
	int i;
	printf("power spectrum:\t");
	for(i=0;i<N;i++)
		printf("%.1f\t",spectrum[i][0]);
	printf("\n");
	printf("frequency?:\t");
	for(i=0;i<N;i++)
		printf("%.4f\t",spectrum[i][1]);
	printf("\n");
}
int findpeaks(double **spectrum, int N, double **peaks){
	int n=1;
	int i=0;
	while (n<N-1){
		if(spectrum[n][0]>spectrum[n-1][0] && spectrum[n][0]>spectrum[n+1][0]){
			peaks[i][0]=spectrum[n][0];
			peaks[i][1]=spectrum[n][1];
			i++;
		}
		n++;
	}
	return i;
}
void printpeaks(double **peaks,int p){
	int i=0;
	printf("peaks found:\t");
	while(i<p)
	{
		printf("%.1f\t",peaks[i][0]);
		i++;
	}
	printf("\n");
	i=0;
	printf("frequencies:\t");
	while(i<p)
	{
		printf("%.2f\t",peaks[i][1]);
		i++;
	}
	printf("\n");
}

double spectrum(double *transform, int N){

	double pwr_spec[N/2+1];
	int n;

	pwr_spec[0]=transform[0]*transform[0];
	for(n=1;n<N/2+1;i++)
		pwr_spec[n]=transform[n]*transform[n]+transform[N-n]*transform[N-n];


