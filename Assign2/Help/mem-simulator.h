/*
 * Name: Jonas Tarnowetzki
 * NSID: jjt739
 * Student #: 11040237

 * Name: Chengshu Yu
 * NSID: chy600
 * Student #: 11104935

 * CMPT 332
 * Assignment 4
 * Part A
 */

#define MAXMEM 2097152

#define EXEC_LIMIT 	10
#define NORM_MEAN	1024
#define STD_DEV 	256
#define EXC_MEAN	4

#define FREE_PROB	800

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <standards.h>
#include <os.h>
#include "Monitor.h"
#include "list.h"

typedef struct _block
{
	long size;
	long address;
} BLOCK;

typedef struct _data
{
	int nodes_searched;
	int mallocs_made;
	int operations_performed;
	int frees_made;
	int memory_holes;
} DATA;

typedef struct _sim_data
{
	DATA *FF_data;
	DATA *BF_data;
} SIM_DATA;

typedef struct _args
{
	int *mem_size;
	int *sleep_time;
	int *free_prob;
	int *free_ptr;
	int executions;
	int algorithm;
	char *name;
} ARGS;

LIST *FF_mem; LIST *BF_mem; SIM_DATA *data_s;

/* 
 * Creates a simulated memory storage of size MAXMEM kilobytes
 */
LIST *Create_Memory();

/*
 * Creates statistic storage for a simulated memory.
 */
SIM_DATA *Create_Data();

/*
 * Monitor function.
 * Param:
 * 	iter - which allocation this user is on
 */
void *MonitorAllocate(ARGS *arg, int iter, void *ptr);

/*
 * Malloc simulation for SIM_MEM.  Returns a pointer to the allocated block.
 * Param:
 *      size_t size     - the amount of memory to allocate
 * 	int algorithm	- the type of fitting algorithm to use
 * Return: 
 * 	a pointer to the allocated memory block or NULL if no memory is
 * 	available
 */
void *MyMalloc(size_t size, int algorithm);

/*
 * Free simulation for SIM_MEM.  Also tests whether a block merge
 * needs to occur.
 * Param:
 *      void *ptr       - a pointer to a block of memory in the simulation
 * 	int algorithm	- the simulated memory that was previously 
 * 				alloacted to
 */
void MyFree(void *ptr, int algorithm);

/*
 * Prints the specified stats for the specified algorithm.
 */
void MyMemStats(int algorithm, int type, SIM_DATA *data_s);

/*
 * Coalesce all adjacent free memory blocks.
 * Precond:
 * 	current position in free list is immediately after position
 * 	freed block was inserted into; function is invoked immediately
 * 	after a block of memory is freed
 * Param:
 * 	free_list	- the list to coalesce
 * 	d		- the data associated with the list being freed
 */
int coalesce(LIST *free_list, DATA *d);

/*
 * Comparison function for ListSearch for first fit of memory.
 */
int compare(void *item1, void *item2);

/*
 * Creates an exponentially distributed psuedo random number.
 */
int exponential_rand();

/*
 * Creates a normally distributed pseudo random number.
 */
int normal_rand();