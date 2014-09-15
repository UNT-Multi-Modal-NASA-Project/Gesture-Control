/*
* Moving sampling functions into a header
*/
//selected includes
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <string.h>
#include <memory.h>
#include <sys/socket.h>
#include <linux/wireless.h>
#include <tgmath.h>
#ifndef SAMPLING_H
#define SAMPLING_H

struct reading{
	double pwr;
	char prefix;
};

struct reading get_sample(char * iwname, int max_samples, int avg);
void put_sample(struct reading *);
#endif
