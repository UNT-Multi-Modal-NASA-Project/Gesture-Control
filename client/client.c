#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#define BUFFERSIZE 100
#define SERVER_IP "129.120.151.98" // cse05
#define PORT 9734

int main ()
{
	int sockfd;
	int len;
	struct sockaddr_in address;
	int result;
	int bytesReceived = 0;
	char message[BUFFERSIZE];

	// create a socket
	sockfd = socket (AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0)
	{
		perror ("socket");
		exit (-1);
	}

	// create an address for the socket
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = inet_addr (SERVER_IP); // cse05
	address.sin_port = PORT;
	len = sizeof (address);

	result = connect (sockfd, (struct sockaddr *) &address, len);
	if (result < 0)
	{
		perror ("connect");
		exit (-2);
	}

	// use strlen() instead of sizeof since I am sending a string
	// the extra byte (+1) allows the null byte to be sent across the network
	while (1) {
		printf ("What would you like to send to server %s?\n", SERVER_IP);
		fgets (message, BUFFERSIZE, stdin);
		write (sockfd, message, strlen(message));
		if (strncmp(message, "exit", 4) == 0 ) 
			break;	
		
		bytesReceived = recv(sockfd, message, BUFFERSIZE, 0);
		message[bytesReceived] = '\0';
		printf ("received from %s:  %s\n", SERVER_IP, message);
	}
	close (sockfd);
}
