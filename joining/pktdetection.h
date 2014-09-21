/*
* Moving the working functions from pktdetect.c into a header file
* These are the functions we want the user to see. The rest are declared
* in the .c file
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#ifndef PKTDETECTION_H
#define PKTDETECTION_H
double dtct_pcket(char* if_nm);
void rpt_pcket(double recieved);
#endif
