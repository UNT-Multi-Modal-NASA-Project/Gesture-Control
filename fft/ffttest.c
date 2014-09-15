/*
* Program to learn the idiocincracie of the fftw3 library
*/

#include <rfftw.h>
#include <stdio.h>
#include <stdlib.h>//for the rand function set
#include <time.h>

double get_rand();
double **fft(double*);
void printfft(double**);

#define SAMPLESIZE 16

void main(){
	double in[SAMPLESIZE];
	double sampleid[SAMPLESIZE];
	int i;

	srand(time(NULL));

	for(i=0;i<SAMPLESIZE;i++)
		in[i]=get_rand();

	printf("input :\t");
	for(i=0;i<SAMPLESIZE;i++)
		printf("%.1f\t",in[i]);
	printf("\n");
	printf("sample:\t");
	for(i=0;i<SAMPLESIZE;i++)
		printf("%d\t", i);
	printf("\n");

	p=rfftw_create_plan(SAMPLESIZE, FFTW_REAL_TO_COMPLEX, FFTW_ESTIMATE);

	rfftw_one(p,in,out);
	power_spectrum[0]=out[0]*out[0];
	for(i=1;i<(SAMPLESIZE+1)/2;++i)
		power_spectrum[i]=out[0]*out[0]+out[SAMPLESIZE/2]*out[SAMPLESIZE/2];
	if(SAMPLESIZE%2==0)
		power_spectrum[SAMPLESIZE/2]=out[SAMPLESIZE/2]*out[SAMPLESIZE/2];
	printf("\n");
	printf("output:\t");
	for(i=0;i<SAMPLESIZE;i++)
		printf("%.1f\t",out[i]);
	printf("\n");
	printf("freq?:\t");
	for(i=0;i<SAMPLESIZE;i++)
		printf("%.1f\t",(double)i/(double)(SAMPLESIZE));
	printf("\n");

	printf("\n");
	printf("power spectrum:\t");
	for(i=0;i<(SAMPLESIZE+1)/2;i++)
		printf("%.1f\t",power_spectrum[i]);
	printf("\n");
	printf("frequency?:\t");
	for(i=0;i<(SAMPLESIZE+1)/2;i++)
		printf("%.4f\t",(double)i/(double)(SAMPLESIZE));
	printf("\n");

	rfftw_destroy_plan(p);
}

double get_rand(){
	return (double)(rand()%10000000)/1e6;
}
double **fft(double *in){
	fftw_real out[SAMPLESIZE];
	fftw_real power_spectrum[SAMPLESIZE/2+1];
	rfftw_plan p;

