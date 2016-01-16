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

#include "mem-simulator.h"

int coalesce(LIST *free_list, DATA *d)
{
	BLOCK *b1; BLOCK *b2;
	
	/* Cursor is after free block just freed */
	b2 = ListCurr(free_list);
	b1 = ListPrev(free_list);
	
	/* Cursor is on free block just freed */
	
	/* Fringe case, free memory is at front of free list.
	 * Make b2 the coalesed node and delete b1.
	 */
	if (b1 == NULL)
	{
		b1 = b2;
		b2 = ListNext(free_list);
		if (b1->size + b1->address == b2->address)
		{
			b2->size = b2->size + b1->size;
			b2->address = b2->address - b1->size;
			ListPrev(free_list);
			ListRemove(free_list);
			d->operations_performed++;
			return 1;
		}
		return 0;
	}
	else
	{
		/* Nodes are never inserted at the end of the list,
		 * so the only case is if the new free node has two
		 * adjacent nodes.
		 */
		while (b1 != NULL) {
			if ((b1->address + b1->size == b2->address))
			{
				b2->address = b1->address;
				b2->size = b1->size + b2->size;
			
				/* Next node has all block data, cur node is
				junk */
				ListRemove(free_list);
				d->operations_performed++;
			}
			b2 = ListCurr(free_list);
			b1 = ListPrev(free_list);
		}
		
		return 1;
	}
	return -1;
}

LIST *Create_Memory()
{
	LIST *newList; BLOCK *firstBlock;
	
	newList = ListCreate();
	
	firstBlock = malloc(sizeof(BLOCK));
	firstBlock->size = MAXMEM;
	firstBlock->address = 0;
	ListInsert(newList, firstBlock);
	
	return newList;
}

SIM_DATA *Create_Data()
{
	SIM_DATA *main_d; DATA *ff_d; DATA *bf_d;
	
	ff_d = malloc(sizeof(DATA)); bf_d = malloc(sizeof(DATA));
	main_d = malloc(sizeof(SIM_DATA));
	
	ff_d->nodes_searched = 0;
	ff_d->mallocs_made = 0;
	ff_d->operations_performed = 0;
	ff_d->frees_made = 0;
	ff_d->memory_holes = 0;
	
	bf_d->nodes_searched = 0;
	bf_d->mallocs_made = 0;
	bf_d->operations_performed = 0;
	bf_d->frees_made = 0;
	bf_d->memory_holes = 0;
	
	main_d->FF_data = ff_d;
	main_d->BF_data = bf_d;
	
	
	return main_d;
}

void printFreeList(LIST *free_list)
{
	BLOCK *cur_block;
	cur_block = ListFirst(free_list);
	
	while (cur_block != NULL)
	{
		printf("Free block start: %ld end: %ld\n",
		       cur_block->address, 
			cur_block->address + cur_block->size);
		cur_block = ListNext(free_list);
	}
	printf("\n");
}