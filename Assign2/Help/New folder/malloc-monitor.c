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

#include <stdio.h>

#include "Monitor.h"
#include "mem-simulator.h"

#define FFMemAvail 0 /* Wait condition for out of memory error */


void *MonitorAllocate(ARGS *arg, int iter, void *ptr)
{
	void *ret = NULL;
	MonEnter();

	/* Ensures user thread attempts to malloc whenever memory
	 * becomes free.
	 */
	while (ret == NULL)
	{
		ret = MyMalloc(arg->mem_size[iter], arg->algorithm);
		if (ret == NULL)
		{
			/*MonWait(FFMemAvail);*/
		}
	}
	
	if (ptr != NULL)
	{
		MyFree(ptr, arg->algorithm);
		MyMemStats(arg->algorithm, 0, data_s);
	}

	MonSignal(FFMemAvail);

	MonLeave();
	
	return ret;
}

/*
 * Algorithm 1 = First Fit
 * Algorithm 2 = Best Fit
 */
void *MyMalloc(size_t size, int algorithm)
{
	BLOCK *new_block; BLOCK *cur_block;
	
	if (algorithm == 1)
	{
		cur_block = ListFirst(FF_mem);
		data_s->FF_data->nodes_searched++;
		while ( cur_block != NULL)
		{
			if (cur_block->size >= size)
				break;
			
			cur_block = ListNext(FF_mem);
			data_s->FF_data->nodes_searched++;
		}
		
		if (cur_block == NULL) return cur_block;
		
		new_block = malloc(sizeof(BLOCK));
		new_block->size = size;
		new_block->address = cur_block->address;
		cur_block->size = cur_block->size - size;
		cur_block->address = cur_block->address + size;
		data_s->FF_data->mallocs_made++;
		data_s->FF_data->operations_performed++;
		
		return new_block;
	}
	else if (algorithm == 2)
	{
		/* NOT BEST FIT NEEDS REPLACING */
		cur_block = ListFirst(BF_mem);
		data_s->BF_data->nodes_searched++;
		while ( cur_block != NULL)
		{
			if (cur_block->size >= size)
				break;
			
			cur_block = ListNext(BF_mem);
			data_s->BF_data->nodes_searched++;
		}
		
		if (cur_block == NULL) return cur_block;
		
		new_block = malloc(sizeof(BLOCK));
		new_block->size = size;
		new_block->address = cur_block->address;
		cur_block->size = cur_block->size - size;
		cur_block->address = cur_block->address + size;
		data_s->BF_data->mallocs_made++;
		data_s->FF_data->operations_performed++;
		
		return new_block;
	}
	else
	{
		printf("Incorrect algorithm specifier.\n");
		Pexit();
		return NULL;
	}
}

void MyFree(void *ptr, int algorithm)
{
	BLOCK *free_ptr; BLOCK *cur_block; BLOCK *prev_block;

	free_ptr = ptr;
	
	if (algorithm == 1)
	{
		prev_block = ListFirst(FF_mem);
		cur_block = ListNext(FF_mem);
		
		/* Base case, finding if free_ptr belongs between two blocks
		 */
		while (cur_block != NULL)
		{
			data_s->FF_data->nodes_searched++;

			/* Base condition for address sorted list */
			if (prev_block->address <= free_ptr->address &&
				cur_block->address > free_ptr->address)
			{
				ListInsert(FF_mem, free_ptr);
				data_s->FF_data->frees_made++;
				data_s->FF_data->operations_performed++;
				coalesce(FF_mem, data_s->FF_data);
				break;
			}
			
			prev_block = cur_block;
			cur_block = ListNext(FF_mem);
		}
		
		/* Fringe cases for freeing memory at beginning or
		 * end of address space.
		 */
		if (cur_block == NULL)
		{
			cur_block = ListFirst(FF_mem);
			data_s->FF_data->nodes_searched++;
			if (cur_block->address > free_ptr->address)
			{
				ListInsert(FF_mem, free_ptr);
				data_s->FF_data->frees_made++;
				data_s->FF_data->operations_performed++;
				coalesce(FF_mem, data_s->FF_data);
			}
			else
			{
				cur_block = ListLast(FF_mem);
				if (cur_block->address <= free_ptr->address)
				{
					ListAdd(FF_mem, free_ptr);
					data_s->FF_data->frees_made++;
					data_s->FF_data->operations_performed++;
					coalesce(FF_mem, data_s->FF_data);
				}
			}
		}
	}
	else if (algorithm == 2)
	{
		ListFirst(BF_mem);
		
		data_s->BF_data->frees_made++;
	}
	printFreeList(FF_mem);
}

void MyMemStats(int algorithm, int type, SIM_DATA *data_s)
{	
	DATA *curData;
	if (algorithm == 1)
	{
		curData = data_s->FF_data;
		curData->memory_holes = ListCount(FF_mem);
	}
	else if (algorithm == 2)
	{
		curData = data_s->BF_data;
		curData->memory_holes = ListCount(BF_mem);
		/* Until second algorithm is implemented don't print. */
		return;
	}
	
	if (type == 0)
	{
		printf("Nodes searched: %d\n", curData->nodes_searched);
		printf("Mallocs made: %d\n", curData->mallocs_made);
		printf("Frees made: %d\n", curData->frees_made);
		printf("Operations performed: %d\n", curData->operations_performed);
		printf("Memory holes: %d\n\n", curData->memory_holes);
	}
}