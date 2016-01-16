/***
Keenan Johnstone 	- 11119412 	- kbj182
Kyle Sheasby		- 11112400 	- kms123

CMPT 332 	Assignment 2	October 22, 2015
***/


#include <stdio.h>
#include <assert.h>
#include <sys/mman.h>

#ifndef __MLIST__
#define __MLIST__

#define M_BIT_SIZE 16
#define MAGIC 11119412

typedef struct node_t {
	int size;
	struct node_t *next;
	struct node_t *prev;
} node_t; 

typedef struct header_t {
	int size;
	int magic;
} header_t;

node_t *head;
node_t *foot;
node_t *free_list;

/***
M_Init

initialize the memory allocator

Precondition: M_Init will not have already been called

Inputs: size 	- The space to be used from the OS in bytes

Output: int 	- is a 0 on sucess and -1 on failure

***/
int M_Init(int);

/***
M_Alloc

Allocates a chunk of initilized memory (M_Init must be used first)

Precondition: There must be enough room 

Postcondition: The memory is allocated

Inputs:		size 	The space to be allocated for a chunk 

Outputs: 	void*	The pointer to the start of the allocated chunk
***/
void *M_Alloc(int);

/***
M_Free

Frees the chunk with address p

Precondition: p is a valid memory address

Inputs: p 	- is a memory addres that has been allocated

Output: int - is a 0 on sucess and -1 on failure

***/
int M_Free(void *);

/***
M_Display

Takes all memory and prints their sizes and addresses

Precondition: None

***/
void M_Display();


#endif