/***
Keenan Johnstone 	- 11119412 	- kbj182

CMPT 332 	Assignment 4	December 7, 2015
***/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define MAXDATASIZE 2000

/*
get_in_addr

Taken from beej's guide to Network Programming

gets the socket address for IPv4 or IPv6

input:  sa	-Pointer to a sockaddr datatype

ouputs: The appropriate socket address type
*/
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

/*
The main function
*/
int main(int argc, char * argv[])
{
	int sockfd, rv, numbytes;
	char buffer[MAXDATASIZE];
	struct addrinfo hints, *servinfo, *p;
	char s[INET6_ADDRSTRLEN];
	
	if (argc != 3) {
        fprintf(stderr,"ERROR::RECEIVER: Usage: client  hostname/ip  port\n");
        return 1;
    }
    
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
	
	if ((rv = getaddrinfo(argv[1], argv[2], &hints, &servinfo)) != 0)
	{
        fprintf(stderr, "ERROR::RECEIVER: getaddrinfo failure: %s\n", gai_strerror(rv));
        return 1;
    }
    
    /*loop through all the results and connect to the first we can*/
    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
            fprintf(stderr, "ERROR::RECEIVER: on socket.\n");
            continue;
        }

        if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            fprintf(stderr, "ERROR::RECEIVER: on connect.\n");
            continue;
        }

        break;
    }
    
    if (p == NULL) {
        fprintf(stderr, "ERROR::RECEIVER: Failed to connect.\n");
        return 2;
    }
    
    inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr), s, sizeof s);
    printf("RECEIVER: Connecting to %s\n", s);

    freeaddrinfo(servinfo);
	
	while(1) 
	{
		numbytes = recv(sockfd, buffer, MAXDATASIZE, 0);
		if(numbytes <= -1)
		{
			fprintf(stderr, "RECEIVER: Error on recv()\n");
            exit(1);
		}
		
		if(numbytes == 0)
		{
			fprintf(stderr, "RECEIVER: Connection Lost\n");
            break;
		}
		printf("> %s\n", buffer);
    }
    close(sockfd);
    return EXIT_SUCCESS;
}































