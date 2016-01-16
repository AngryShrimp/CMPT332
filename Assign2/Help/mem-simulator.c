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

#include <math.h>
#include "mem-simulator.h"

/*
 * Param:
 *      arg - the maximum possible memory allocations made
 */
PROCESS user(ARGS *arg)
{
	int i; int j; int count;
	LIST *used_list;
	BLOCK *free_ptr; BLOCK *mem_ptr;
	used_list = ListCreate();
	
	printf("My algorithm is: %d\n", arg->algorithm);
	printf("My executions are: %d\n", arg->executions);
	
	for (i = 0; i < arg->executions; i++)
	{
		free_ptr = NULL;
		/* Checks free probability and if something can be freed*/
		if (arg->free_prob[i] > FREE_PROB)
		{
			count = ListCount(used_list);
			if (count)
			{
				free_ptr = ListFirst(used_list);
				for (j = 1; j < count; j++)
				{
					free_ptr = ListNext(used_list);
				}
			}
		}
		
		mem_ptr = MonitorAllocate(arg, i, free_ptr);
		ListAdd(used_list, mem_ptr);
		Sleep(arg->sleep_time[i]);
	}
	
}

/*******************************************************************
 * RANDOM NUMBER GENERATION
 *******************************************************************/
int exponential_rand()
{
	double u;
	u = (double)(rand()+1.0)/(RAND_MAX+1.0);
	
	return (-EXC_MEAN * log(u));
}

int normal_rand()
{
	double u1; double u2; double u3; double x; 
	int bool = 1;
	
	while (bool)
	{
		u1 = (double)(rand()+1.0)/(RAND_MAX+1.0);
		u2 = (double)(rand()+1.0)/(RAND_MAX+1.0);
	
		x = (-1) * log(u1);
		if (u2 < exp( (-1 * ( (x-1.0) * (x-1.0) ) ) / 2  ) )
		{ bool = 0; }
	}
	
	u3 = (double)(rand()+1.0)/(RAND_MAX+1.0);
	
	if (u3 > 0.5)
		return NORM_MEAN + STD_DEV * x;
	else
		return NORM_MEAN - STD_DEV * x;
}

/* 
 * Thread entry point.
 * 
 * Invocation: mem-sim numthreads executions seed
 * 	numthreads 	- the number of threads to run concurrently
 * 	executions	- how many times to allocate memory
 * 	seed		- the seed the random number generator uses
 */
int mainp(int argc, char **argv)
{
	int numthreads; int executions; int seed; int mod_exec;
	int *FF_t; int *BF_t; char *name;
	int i; int j;
	ARGS *user_args; ARGS *user_args2;
	int error = 0;
	
	/* Arg conversion and error checking. */
	if (argc < 4)
	{
		fprintf(stderr, "Invalid number of parameters (expected 3)");
		return -1;
	}
	
	numthreads = strtod(argv[1], NULL);
	executions = strtod(argv[2], NULL);
	seed = strtod(argv[3], NULL);
	
	if (numthreads < 1)
	{
		fprintf(stderr, "Argument 1 invalid: error or negative.\n");
		error = 1;
	}
	if (executions < 1)
	{
		fprintf(stderr, "Argument 3 invalid: error or negative.\n");
		error = 1;
	}
	if (seed < 1)
	{
		fprintf(stderr, "Argument 4 invalid: error or negative.\n");
		error = 1;
	}
	
	if (error == 1)
		return -1;
	
	/* End arg conversion. */
	
	/* User argument setup and random number generation. */
	
	srand(seed);
	
	FF_t = malloc(numthreads * sizeof(int));
	BF_t = malloc(numthreads * sizeof(int));
	name = malloc(8 * sizeof(char));
	
	FF_mem = Create_Memory();
	BF_mem = Create_Memory();
	data_s = Create_Data();
	
	/* Monitor Initialization */
	MonInit(1);
	
	for (i = 0; i < numthreads; i++)
	{
		user_args = malloc(sizeof(ARGS));
		user_args->mem_size = malloc(executions * sizeof(int));
		user_args->sleep_time = malloc(executions * sizeof(int));
		user_args->free_prob = malloc(executions * sizeof(int));
		user_args->free_ptr = malloc(executions * sizeof(int));
		user_args->executions = executions;
		
		for (j = 0; j < executions; j++)
		{
			user_args->mem_size[j] = normal_rand();
			user_args->sleep_time[j] = exponential_rand();
			user_args->free_prob[j] = normal_rand();
			user_args->free_ptr[j] = rand();
			
			/* Debug line to check random distros
			printf("mem_size: %d, sleep_time: %d\n", 
				user_args->mem_size[j], 
				user_args->sleep_time[j]
			);*/
		}
		
		/* Make the number of allocations semi-random, with a lower 
		 * bound of arg->executions - arg->executions/EXEC_LIMIT */
		mod_exec = rand() % user_args->executions/EXEC_LIMIT;
		user_args->executions = user_args->executions - mod_exec;
		
		error = sprintf(name, "User %d", i+1);
		user_args->name = malloc(8 * sizeof(char));
		strcpy(user_args->name, name);
		
		/* Thread creation and error checking using multiple
		 * fitting algorithms. */
		user_args->algorithm = 1;
		FF_t[i] = Create( (void(*)()) user, 16000, name, 
				  (void*) user_args, NORM, USR );
		if (FF_t[i] == PNUL) 
		{
			perror("Create userthread error");
			return -1;
		}
		user_args2 = malloc(sizeof(ARGS));
		user_args2->mem_size = user_args->mem_size;
		user_args2->sleep_time = user_args->sleep_time;
		user_args2->free_prob = user_args->free_prob;
		user_args2->free_ptr = user_args->free_ptr;
		user_args2->executions = user_args->executions;
		user_args2->algorithm = 2;
		
		BF_t[i] = Create( (void(*)()) user, 16000, name, 
				  (void*) user_args2, NORM, USR );
		if (BF_t[i] == PNUL) 
		{
			perror("Create userthread error");
			return -1;
		}
	}
	
	return 0;
}
