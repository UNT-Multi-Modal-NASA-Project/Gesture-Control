/*
* program to create interface messages
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

struct msg{
	char* message;//actuation to report
	char* server;
	int port;
};
struct msg buildmsg(int);
void main(int argc, char *argv[]){
	int i,msgs=64;
	struct msg m;
	if (argc>1)
		msgs=atoi(argv[1]);

	srand(time(NULL));

	for(i=0;i<msgs;i++)
	{
		m=buildmsg(rand()%10);
		printf("%s sent to %s on port %d\n",m.message, m.server,m.port);
	}
}
struct msg buildmsg(int sel){
	struct msg ret;
	ret.server="Thor";
	ret.port=9001;

	switch(sel){
/*		case 0: ret.message="FILLING";
			break;*/
		case 1: ret.message="PUSH GESTURE";
			break;
		case 2: ret.message="PULL GESTURE";
			break;
		case 3: ret.message="NEAR SWIPE GESTURE";
			break;
		case 4: ret.message="FAR SWIPE GESTURE";
			break;
		//error codes
/*		case 7: ret.msg={""};
			break;

*/
		default: ret.message="NO GESTURE";
	}
	return ret;
}
