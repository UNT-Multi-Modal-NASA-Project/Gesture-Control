/*
* Program to learn the idiocincracie of the fftw3 library
*/

#include <rfftw.h> // for fft action
#include <stdio.h>
//These required for the signal generator
#include <stdlib.h>//for the rand function set
#include <math.h>
#include <time.h>

double sig_gen(double freq, int t, int nflag);
double get_rand();//for noise

