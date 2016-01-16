/***
Keenan Johnstone 	- 11119412 	- kbj182
Kyle Sheasby		- 11112400 	- kms123

CMPT 332 	Assignment 3	November 16, 2015
***/
#include <stdio.h>
#include <pthread.h>
#include <time.h>
#include <assert.h>
#include <unistd.h>
#include <stdlib.h>

#ifndef __DOGWASHSYNCH__
#define __DOGWASHSYNCH__

typedef enum {DA, DB, DO} dogtype;



/***
dogwash_init

initialize the dog wash with the specified number of washing bays

Precondition: 	dogwash_init will not have already been called

Inputs: 		size 	- The number of dog wash bays to be created upon init

Output: 		int 	- is a 0 on sucess and -1 on failure
***/
int dogwash_init(int numBays);

/***
newdog

This function is called when a new dog arrives at the dog wash needing a wash.

Precondition: 	dogwash_init has already been successfully called
				
				the given dog is a of a valid dog_type

Inputs: 		dog 	- The dog that needs to be allocated into a dogbay

Output: 		int 	- is a 0 on sucess and -1 on failure
***/
int newdog(dogtype dog);

/***
dogdone

This function is called when a dog of type dogtype that had been 
allocated a wash bay is done being washed and the wash bay is now 
free.

Precondition: 	dogwash_init has already been successfully called
				
				the dog bay with the 
				
				the given dog is a of a valid dog_type

Inputs: 		dog 	- The dog that needs to be allocated into a dogbay

Output: 		int 	- is a 0 on sucess and -1 on failure
***/
int dogdone(dogtype dog);

/***
dogwash_done

This function is called to perform any required data cleanup actions;
after calling this function it should be possible to reinitialize

Precondition: 	dogwash_init has already been successfully called

Inputs: 		

Output: 		int 	- is a 0 on sucess and -1 on failure
***/
int dogwash_done();

#endif