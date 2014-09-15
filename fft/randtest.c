#include <stdio.h>
#include <stdlib.h>
#include <time.h>

double get_rand(){
	return (double)(rand()%10000000)/1e6;
}

void main()
{
	double r;
	int i;
	srand(time(NULL));

	for (i=0;i<16;i++)
		printf("%.4f \t", get_rand());
}

