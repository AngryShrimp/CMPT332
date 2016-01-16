/***
Keenan Johnstone 	- 11119412 	- kbj182

CMPT 332 	Assignment 4	December 7, 2015
***/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <pthread.h>

#define SEND_PORT "34322"
#define RECV_PORT "31131"

#define HOSTNAME_SIZE 1023

#define BACKLOG_MAX 5

#define CLIENT_MAX 20

#define BUFFER_MAX 2000

struct helper_t {
    int socketFD;
    char IP[BUFFER_MAX + INET_ADDRSTRLEN];
};

/*Global Variables!*/
int client_count;
char buffer[BUFFER_MAX + INET_ADDRSTRLEN];
pthread_mutex_t recv_lock, send_lock, buff_lock, remaining_lock, count_lock;
pthread_cond_t recv_cond, send_cond;
int recv_count = 0;
int recv_remaining = 0;

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
recv_connection_helper
precond:
	Used for creation from Receiver client threads
In:
	arg: The array of new sockets

	Out: Null
 */
void * recv_connection_helper(void * arg)
{
	int socket = *(int*)arg;
	int sent = 0;
	pthread_detach(pthread_self());
	
	while(1)
	{
		if(pthread_mutex_lock(&recv_lock) == -1) break;
		if(pthread_cond_wait(&recv_cond,&recv_lock) == -1) break;
		if(pthread_mutex_unlock(&recv_lock) == -1) break;
		
		
		if((sent = send(socket, buffer, BUFFER_MAX, MSG_NOSIGNAL)) < 0)
		{
			fprintf(stderr, "SERVER::RECVR: Connection Lost from socket %d\n", socket);
			break;
		}
		
		
		if(pthread_mutex_lock(&remaining_lock) == -1) break;
		recv_remaining--;
		if(recv_remaining <= 0)
		{
			if(pthread_mutex_lock(&send_lock) == -1) break;
			pthread_cond_broadcast(&send_cond);
			if(pthread_mutex_unlock(&send_lock) == -1) break;
		}
		if(pthread_mutex_unlock(&remaining_lock) == -1) break;
		
		
	}
	if(pthread_mutex_lock(&count_lock) == -1) return NULL;
	recv_count--;
	if(pthread_mutex_unlock(&count_lock) == -1) return NULL;
	close(socket);
	return NULL;
}

/*
send_connection_helper
precond:
	Used for creation from Sender client threads
In:
	arg: The array of new sockets

	Out: Null
 */
void * send_connection_helper(void * arg)
{
	struct helper_t *tmp_arg = (struct helper_t*) arg;
	int read;
	char local_buffer[BUFFER_MAX];
	char local_IP[BUFFER_MAX + INET_ADDRSTRLEN];
	char tmp_IP[BUFFER_MAX + INET_ADDRSTRLEN];
	char tmp_port[50];
	strcpy(local_IP, tmp_arg->IP);
    strcat(local_IP, ", ");
	strcat(local_IP, SEND_PORT);
	sprintf(tmp_port, "(%d):", tmp_arg->socketFD);
	strcat(local_IP, tmp_port);
	strcpy(tmp_IP, local_IP);
	pthread_detach(pthread_self());
	
	while(1)
	{
		
		read = recv(tmp_arg->socketFD, local_buffer, BUFFER_MAX-1, 0);
        strcat(local_IP, local_buffer);
		
		if(read == 0)
		{
			printf("SERVER::SENDER: Connection Lost from %d\n", tmp_arg->socketFD);
			break;
		}
		else
		{
			local_buffer[read] = '\0';
		}
		
		/*Lock global buffer*/
		if(pthread_mutex_lock(&buff_lock) == -1) break;
		strcpy(buffer, local_IP);
		
		/*UNCOMMENT for outputting to server term*/
		/*printf("%s\n", buffer);*/
		
		
		strcpy(local_IP, tmp_IP);
		
		
		/*Lock receiver counter to count how many Receivers have gotten the message*/
		if(pthread_mutex_lock(&count_lock) == -1) break;
		
		/*Lock, edit the unlock */
		if(pthread_mutex_lock(&remaining_lock) == -1) break;
		recv_remaining = recv_count;
		if(pthread_mutex_unlock(&remaining_lock) == -1) break;

		
		/*Signal all Receivers that there is a new message
		 And lock senders from sending*/
		/*if(pthread_mutex_lock(&send_lock) == -1) break;*/
		pthread_cond_broadcast(&recv_cond);
		
		/*if(pthread_cond_wait(&send_cond,&send_lock) == -1) break;*/
		/*if(pthread_mutex_unlock(&send_lock) == -1) break;*/
		if(pthread_mutex_unlock(&count_lock) == -1) break;
		
		/*Unlock buffer*/
		if(pthread_mutex_unlock(&buff_lock) == -1) break;
		
		/*More locks needed?*/
		
		
	}
	
	close(tmp_arg->socketFD);
	return NULL;
}

/*
The main function
*/
int main(void)
{
	/*Beej's Suggesgetd Variables*/
	struct addrinfo hints;
	struct addrinfo *p, *server_info;
	struct sockaddr_in *server;
	int rv;
	int yes = 1;
	void * addr;
	int send_socket, recv_socket;
	struct helper_t sender_info[CLIENT_MAX];
	
	/*array of connected clients*/
	int new_socket[CLIENT_MAX];
	
	/*set of socket descriptors*/
	fd_set client_fds;
	int max_sd;
	struct sockaddr_storage client_address;
	int address_size = sizeof(client_address);
	
	/*Hostname variables*/
	char hostname[HOSTNAME_SIZE + 1];
	char ip_name[INET_ADDRSTRLEN];
	
	/*pthread stuff*/
	pthread_t pid;
	if(pthread_mutex_init(&recv_lock, NULL) == -1) return 1;
	if(pthread_mutex_init(&send_lock, NULL) == -1) return 1;
	if(pthread_mutex_init(&buff_lock, NULL) == -1) return 1;
	if(pthread_mutex_init(&remaining_lock, NULL) == -1) return 1;
	if(pthread_mutex_init(&count_lock, NULL) == -1) return 1;
	if(pthread_cond_init(&recv_cond, NULL) == -1) return 1;
	if(pthread_cond_init(&send_cond, NULL) == -1) return 1;
	
	/*Set client count to zero*/
	client_count = 0;
	
	
	/*Get Server's IP Address*/
	if (gethostname(hostname, HOSTNAME_SIZE) < 0) {
        perror("ERROR::SERVER: Couldn't get hostname");
        return 0;
    }
    
    /*Fill in hints, from Beej*/
	memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC; 		/*Unspecified, so IPv4 or v6*/
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE; 		/*Use my IP*/
	
	if ((rv = getaddrinfo(hostname, NULL, &hints, &server_info)) != 0) {
        fprintf(stderr, "ERROR::SERVER: getaddrinfo failure: %s\n", gai_strerror(rv));
        return 0;
    }
    
    
    server = (struct sockaddr_in *)server_info->ai_addr;
	addr = &(server->sin_addr);
    inet_ntop(server_info->ai_family, addr, ip_name, sizeof ip_name);
	
	/*Print Ports and IP on server*/
	printf("Server IP:\t %s\n", ip_name);
    printf("Receiver PORT:\t %s\n", RECV_PORT);
    printf("Sender PORT:\t %s\n", SEND_PORT);
    
    freeaddrinfo(server_info);
	
	printf("SERVER: Listening for connections...\n");
	
	/*Create Receiver Socket!*/
	
	/*Fill in hints, from Beej*/
	memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC; 		/*Unspecified, so IPv4 or v6*/
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE; 		/*Use my IP*/
    
    if ((rv = getaddrinfo(NULL,RECV_PORT, &hints, &server_info)) != 0) {
        fprintf(stderr, "ERROR::SERVER: getaddrinfo failure for Receiver: %s\n", gai_strerror(rv));
        return 0;
    }
    /*Loop through all the results and connect where we can*/
	/*From Beej*/
    for(p = server_info; p != NULL; p = p->ai_next) 
	{
		if((recv_socket = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) < 0)
		{
			fprintf(stderr, "ERROR::SERVER: Failed to create Socket for Receiver.\n");
			continue;
		}
		/*From Beej*/
		setsockopt(recv_socket, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
		if (bind(recv_socket, p->ai_addr, p->ai_addrlen) < 0) 
		{
            close(recv_socket);
            continue;
        }
        break;
    }
    
    freeaddrinfo(server_info);
	if (p == NULL)  {
        fprintf(stderr, "ERROR::SERVER: Failed to bind for Receiver.\n");
        return 0;
    }
	
	/*Create Sender Socket!*/
	
	/*Fill in hints, from Beej*/
	memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC; 		/*Unspecified, so IPv4 or v6*/
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE; 		/*Use my IP*/
    
    if ((rv = getaddrinfo(NULL,SEND_PORT, &hints, &server_info)) != 0) {
        fprintf(stderr, "ERROR::SERVER: getaddrinfo failure for Sender: %s\n", gai_strerror(rv));
        return 0;
    }
    /*Loop through all the results and connect where we can*/
	/*From Beej*/
    for(p = server_info; p != NULL; p = p->ai_next) 
	{
		if((send_socket = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) < 0)
		{
			fprintf(stderr, "ERROR::SERVER: Failed to create Socket for Sender.\n");
			continue;
		}
		/*From Beej*/
		setsockopt(send_socket, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
		if (bind(send_socket, p->ai_addr, p->ai_addrlen) == -1) 
		{
            close(send_socket);
            continue;
        }
        break;
    }
    
    freeaddrinfo(server_info);
	if (p == NULL)  {
        fprintf(stderr, "ERROR::SERVER: Failed to bind for Sender.\n");
        return 0;
    }
	
	
	/*Listen for both ports now!*/
	if(listen(recv_socket, BACKLOG_MAX) != 0)
	{
		fprintf(stderr, "ERROR::SERVER: Failed to listen for Receiver.\n");
        return 0;
	}
	if(listen(send_socket, BACKLOG_MAX) != 0)
	{
		fprintf(stderr, "ERROR::SERVER: Failed to listen for Sender.\n");
		return 0;
	}
	
	
	
	/*Accept loop*/
	/*We need to decide using select() which type of client is connecting*/
	while(1)
	{
		/*Clear Set*/
		FD_ZERO(&client_fds);
		
		/*Add the send and recv clients to the set*/
		FD_SET(recv_socket, &client_fds);
		FD_SET(send_socket, &client_fds);
		max_sd = send_socket;
		
		if(select(max_sd+1, &client_fds, NULL, NULL, NULL) == -1)
		{
			fprintf(stderr, "ERROR:SERVER: failed to use select()");
			return 0;
		}
		else
		{
			/*Receiver Client*/
			if(FD_ISSET(recv_socket, &client_fds))
			{
				/*Create new socket in our array of sokets*/
				new_socket[client_count] = accept(recv_socket, (struct sockaddr*)&client_address, (socklen_t*)&address_size);
				/*Failed to accept*/
				if(new_socket[client_count] == -1)
				{
					fprintf(stderr, "ERROR::SERVER: Accept failed on Receiver Socket!\n");
				}
				else
				{
					/*printf("SERVER::RECEIVER: Accepted Receiver client!\n");*/
					/*Accepted!*/
					inet_ntop(client_address.ss_family, get_in_addr((struct sockaddr *)&client_address), ip_name, sizeof ip_name);
					printf("SERVER::RECEIVER: Connection from Receiver %s:%s\n", ip_name, RECV_PORT);
					
					pthread_mutex_lock(&count_lock);
					recv_count++;
					pthread_mutex_unlock(&count_lock);
					
					/*Create pthread now!*/
					if(pthread_create(&pid, 0, recv_connection_helper, (void*)&new_socket[client_count]) == -1)
					{
						fprintf(stderr, "ERROR::SERVER: Couldn't create new pthread\n");
						return 0;
					}
					client_count++;
				}
			}
			/*Sender Client*/
			if(FD_ISSET(send_socket, &client_fds))
			{
				/*Create new socket in our array of sokets*/
				new_socket[client_count] = accept(send_socket, (struct sockaddr*)&client_address, (socklen_t*)&address_size);
				/*Failed to accept*/
				if(new_socket[client_count] == -1)
				{
					fprintf(stderr, "ERROR::SERVER: Accept failed on Sender Socket!\n");
					continue; 
				}
				else
				{
					/*printf("SERVER::SENDER: Accepted Sender client!\n");*/
					/*Accepted!*/
					inet_ntop(client_address.ss_family, get_in_addr((struct sockaddr *)&client_address), ip_name, sizeof ip_name);
				
					sender_info[client_count].socketFD = new_socket[client_count];
					strcpy(sender_info[client_count].IP, ip_name);
					printf("SERVER::SENDER: Connection from Sender %s:%s\n", ip_name, SEND_PORT);
				
					/*Create pthread now!*/
					if(pthread_create(&pid, 0, send_connection_helper, (void*)&sender_info[client_count]) == -1)
					{
						fprintf(stderr, "ERROR::SERVER: Couldn't create new pthread\n");
						return 0;
					}
					client_count++;
				}
			}
		}
		
		
	}
	printf("ByeBye!");
	close(send_socket);
	close(recv_socket);
	return EXIT_SUCCESS;
}
