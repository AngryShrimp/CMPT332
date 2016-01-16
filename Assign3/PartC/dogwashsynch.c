/***
Keenan Johnstone 	- 11119412 	- kbj182
Kyle Sheasby		- 11112400 	- kms123

CMPT 332 	Assignment 3	November 16, 2015
***/
#include "dogwashsynch.h"

sem_t DALock, DBLock, bayLock, typeLock, dogLock;

int numOfDA, numOfDB;
int initialized = 0;

/***
dogwash_init

initialize the dog wash with the specified number of washing bays

Precondition: 	dogwash_init will not have already been called

Inputs: 		size 	- The number of dog wash bays to be created upon init

Output: 		int 	- is a 0 on sucess and -1 on failure
***/
int dogwash_init(int numBays)
{
	/*Create a new array of dog bays based on size of num_bays*/
	/*Create mutex'*/
	if(initialized == 1)
	{
		fprintf(stderr, "Error: DogWash has already been initialized.\n");
		return -1;
	}
	if(numBays < 1)
	{
		fprintf(stderr, "Error: Invalid size for dogwash_init.\n");
		return -1;
	}

	if(sem_init(&DALock, 0, 1) == -1) return -1;
	if(sem_init(&DBLock, 0, 1) == -1) return -1;
	if(sem_init(&bayLock, 0, numBays) == -1) return -1;
	if(sem_init(&typeLock, 0, 1) == -1) return -1;
	if(sem_init(&dogLock, 0, 1) == -1) return -1;

	numOfDA = 0;
	numOfDB = 0;

	initialized = 1;

	return 0;
}

/***
newdog

This function is called when a new dog arrives at the dog wash needing a wash.

Precondition: 	dogwash_init has already been successfully called
				
				the given dog is a of a valid dog_type

Inputs: 		dog 	- The dog that needs to be allocated into a dogbay

Output: 		int 	- is a 0 on sucess and -1 on failure
***/
int newdog(dogtype dog)
{
	if(initialized != 1)
	{
		fprintf(stderr, "Error: DogWash was not initialized before calling newdog.\n");
		return -1;
	}
	/*DA*/
	if(dog == DA)
	{
		/*Lock Counters and Lock from other dogs enterig the system while editig values*/
		if(sem_wait(&dogLock) == -1) return -1;
		if(sem_wait(&DALock) == -1) return -1;

		/*If there are no DA's try to grab the typelock
		This will wait if there are any DB's*/
		if(numOfDA == 0)
		{
			if(sem_wait(&typeLock) == -1) return -1;
		}
		numOfDA++;
		
		if(sem_post(&DALock) == -1) return -1;
		if(sem_post(&dogLock) == -1) return -1;
		

		/*Now lock bays*/
		if(sem_wait(&bayLock) == -1) return -1;
	}
	/*DB*/
	else if(dog == DB)
	{
		/*Lock Counters and Lock from other dogs enterig the system while editig values*/
		if(sem_wait(&dogLock) == -1) return -1;
		if(sem_wait(&DBLock) == -1) return -1;

		/*If there are no DA's try to grab the typelock
		This will wait if there are any DB's*/
		if(numOfDB == 0)
		{
			if(sem_wait(&typeLock) == -1) return -1;
		}
		numOfDB++;
		
		if(sem_post(&DBLock) == -1) return -1;
		if(sem_post(&dogLock) == -1) return -1;
		

		/*Now lock bays*/
		if(sem_wait(&bayLock) == -1) return -1;
	}
	/*DO*/
	else if(dog == DO)
	{
		/*Sleep for fairness, so it doesnt hog*/
		sleep(5);

		/*Now lock bays*/
		if(sem_wait(&bayLock) == -1) return -1;
	}
	/*Invalid*/
	else
	{
		fprintf(stderr, "Error: Invalid Dog Type in newdog.\n");
		return -1;
	}
	return 0;
}

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
int dogdone(dogtype dog)
{
	if(initialized != 1)
	{
		fprintf(stderr, "Error: DogWash was not initialized before calling dogdone.\n");
		return -1;
	}
	/*DA*/
	if(dog == DA)
	{
		/*Lock Bays for modification*/
		if(sem_post(&bayLock) == -1) return -1;

		/*Lock Da Counter for modification*/
		if(sem_wait(&DALock) == -1) return -1;
		numOfDA--;

		/*If there are no more DAs in the system release type lock*/
		if(numOfDA <= 0)
		{
			if(sem_post(&typeLock) == -1) return -1;
		}
		if(sem_post(&DALock) == -1) return -1;
	}
	/*DB*/
	else if(dog == DB)
	{
		/*Lock Bays for modification*/
		if(sem_post(&bayLock) == -1) return -1;

		/*Lock DB Counter for modification*/
		if(sem_wait(&DBLock) == -1) return -1;
		numOfDB--;

		/*If there are no more DBs in the system release type lock*/
		if(numOfDB <= 0)
		{
			if(sem_post(&typeLock) == -1) return -1;
		}
		if(sem_post(&DBLock) == -1) return -1;
	}
	/*DO*/
	else if(dog == DO)
	{
		/*Lock Bays for modification*/
		if(sem_post(&bayLock) == -1) return -1;
	}
	/*Invalid*/
	else
	{
		fprintf(stderr, "Error: Invalid Dog Type in dogdone.\n");
		return -1;
	}
	return 0;
}

/***
dogwash_done

This function is called to perform any required data cleanup actions;
after calling this function it should be possible to reinitialize

Precondition: 	dogwash_init has already been successfully called

Inputs: 		

Output: 		int 	- is a 0 on sucess and -1 on failure
***/
int dogwash_done()
{
	if(initialized != 1)
	{
		fprintf(stderr, "Error: DogWash was not initialized.\n");
		return -1;
	}
	if(sem_destroy(&DALock) == -1) return -1;
	if(sem_destroy(&DBLock) == -1) return -1;
	if(sem_destroy(&bayLock) == -1) return -1;
	if(sem_destroy(&typeLock) == -1) return -1;
	if(sem_destroy(&dogLock) == -1) return -1;

	numOfDA = 0;
	numOfDB = 0;

	initialized = 0;

	return 0;
}

