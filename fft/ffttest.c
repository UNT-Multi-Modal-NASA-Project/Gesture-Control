/*
* Program to learn the idiocincracie of the fftw3 library
*/

#include <rfftw.h>
#include <stdio.h>
#include <stdlib.h>//for the rand function set
#include <time.h>

double get_rand();

#define SAMPLESIZE 32
void main(){
	fftw_real in[SAMPLESIZE];
	fftw_real out[SAMPLESIZE];
	fftw_real power_spectrum[SAMPLESIZE/2+1];
	rfftw_plan p;
	int i;

	srand(time(NULL));

	for(i=0;i<SAMPLESIZE;i++)
		in[i]=get_rand();

	printf("input:");
	for(i=0;i<SAMPLESIZE;i++)
		printf("%.3f, ",in[i]);
	printf("\n");
	printf("\n");

	p=rfftw_create_plan(SAMPLESIZE, FFTW_REAL_TO_COMPLEX, FFTW_ESTIMATE);

	rfftw_one(p,in,out);
	power_spectrum[0]=out[0]*out[0];
	for(i=1;i<(SAMPLESIZE+1)/2;++i)
		power_spectrum[i]=out[0]*out[0]+out[SAMPLESIZE/2]*out[SAMPLESIZE/2];
	if(SAMPLESIZE%2==0)
		power_spectrum[SAMPLESIZE/2]=out[SAMPLESIZE/2]*out[SAMPLESIZE/2];

	printf("output:");
	for(i=0;i<SAMPLESIZE;i++)
		printf("%.3f, ",out[i]);
	printf("\n");
	printf("\n");

	printf("power spectrum:");
	for(i=0;i<(SAMPLESIZE+1)/2;i++)
		printf("%.3f, ",power_spectrum[i]);
	printf("\n");
	printf("\n");

	rfftw_destroy_plan(p);
}

double get_rand(){
	return rand()/1e6;
}
