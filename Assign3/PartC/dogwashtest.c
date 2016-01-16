#include <pthread.h>
#include <unistd.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "dogwashsynch.h"

/***
dogHelp

This function is for helping with 
***/
void *dogHelp(void * dog)
{
	clock_t begin, end;

	dogtype* d;
	d = (dogtype*) dog;
	
	newdog(*d);
	begin = clock();
	if(*d == DA) printf("%d: Washing Dog of type A.\n", (int)begin);	
	if(*d == DB) printf("%d: Washing Dog of type B.\n", (int)begin);
	if(*d == DO) printf("%d: Washing Dog of type O.\n", (int)begin);

	
	
	
	/*Sleep while washing*/
	sleep(2);

	dogdone(*d);

	end = clock();
	if(*d == DA) printf("%d: Done washing Dog of type A.\n", (int)end);
	if(*d == DB) printf("%d: Done washing Dog of type B.\n", (int)end);
	if(*d == DO) printf("%d: Done washing Dog of type O.\n", (int)end);

	return NULL;
}

/***
main

This function is for the test of the dogwash.
***/
int main()
{
	pthread_t thds[20];
	dogtype DogA, DogB, DogO;
	int i;

	DogA = DA;
	DogB = DB;
	DogO = DO;
	
	printf("\n\n======== Begin Assert Tests ========\n");
	
	
	

	/*Test invalid values*/
	printf("Testing Invalid inputs for dogwash_init:\n");
	assert(dogwash_init(0) == -1);
	assert(dogwash_init(-45) == -1);
	assert(dogwash_init(-1) == -1);
	
	printf("Testing Initilizing twice for dogwash_init:\n");
	dogwash_init(4);
	assert(dogwash_init(4) == -1);
	
	
	printf("======= Assert Tests Success =======\n");

	printf("\n\n\n");
	printf("====== Begin General Op Tests ======\n");
	printf("======== Testing Starvation ========\n");
	/*Create 20 thds for testing*/
	/*Test Starvation*/
	pthread_create(&thds[0],NULL,dogHelp,(void *) &DogA);
	pthread_create(&thds[1],NULL,dogHelp,(void *) &DogB);
	pthread_create(&thds[2],NULL,dogHelp,(void *) &DogA);
	pthread_create(&thds[3],NULL,dogHelp,(void *) &DogA);
	pthread_create(&thds[4],NULL,dogHelp,(void *) &DogA);
	pthread_create(&thds[5],NULL,dogHelp,(void *) &DogA);
	pthread_create(&thds[6],NULL,dogHelp,(void *) &DogA);
	pthread_create(&thds[7],NULL,dogHelp,(void *) &DogA);
	pthread_create(&thds[8],NULL,dogHelp,(void *) &DogO);
	pthread_create(&thds[9],NULL,dogHelp,(void *) &DogA);
	sleep(10);
	printf("======== General Operation =========\n");
	/*General Op*/
	pthread_create(&thds[10],NULL,dogHelp,(void *) &DogA);
	pthread_create(&thds[11],NULL,dogHelp,(void *) &DogB);
	pthread_create(&thds[12],NULL,dogHelp,(void *) &DogA);
	pthread_create(&thds[13],NULL,dogHelp,(void *) &DogA);
	pthread_create(&thds[14],NULL,dogHelp,(void *) &DogO);
	sleep(10);
	/*General Op*/
	pthread_create(&thds[15],NULL,dogHelp,(void *) &DogO);
	pthread_create(&thds[16],NULL,dogHelp,(void *) &DogA);
	pthread_create(&thds[17],NULL,dogHelp,(void *) &DogB);
	pthread_create(&thds[18],NULL,dogHelp,(void *) &DogB);
	pthread_create(&thds[19],NULL,dogHelp,(void *) &DogO);

	for(i = 0; i < 20; i++) {
		pthread_join(thds[i], NULL);
	}
	printf("============= End Test =============\n");
	dogwash_done();

	return 0;
}