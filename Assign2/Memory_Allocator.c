/***
Keenan Johnstone 	- 11119412 	- kbj182
Kyle Sheasby		- 11112400 	- kms123

CMPT 332 	Assignment 2	October 22, 2015
***/

#include <stdio.h>
#include <sys/mman.h>

#include "Memory_Allocator.h"

/***
helper function

Find the next fit in the free list

Inputs: int size 	- The Size of the space to be allocated from the free list

Output: node_t 		- The node that matches the size
***/

node_t* next_fit(int size)
{
	node_t *tmp;
	tmp = free_list;
	/*If the first node in the free list is big enough use it*/
	if (tmp->size > size)
		return tmp;
	/*Else iterate through through the free_list looking for the first node that is big enough*/
	else
	{
		while (tmp->next != NULL)
		{
			tmp = tmp->next;
			if (tmp->size > size)
				return tmp;
		}
	}
	/*If the next fit IS the free_list and the only entry is the free*/
	if(tmp == free_list  && free_list->next == NULL)
	{
		/*Make sure that there is enough room in the free list for the allocation*/
		if(tmp->size - size < sizeof(node_t))
		{
			fprintf(stderr, "ERROR: No Space\n");
			tmp = NULL;
		}
	}

	return tmp;
}



/***
helper function

Coalesces the free_list into a single node of the same size of all of the lists comnbined


***/
void coalesce(){

	node_t *tmp = free_list;

	if(tmp==NULL)
	{
		return;
	}
	else
	{
		/*While there is another node*/
		while(tmp->next != NULL)
		{
			tmp = tmp->next;

			/*Check that the node's addresses are adjacent and if they are, coalesce them*/
			if((int*)tmp == ((int*)(tmp->prev) + ((tmp->prev)->size)))
			{
				/*Add the sizes*/
				(tmp->prev)->size = (tmp->prev)->size + tmp->size;

				/*If the next node is NULL (end of the list) remove the node*/
				if(tmp->next == NULL)
				{
					foot = tmp->prev;
					(tmp->prev)->next = NULL;
					return;

				}
				/*Else remove the current node and connec the two surrounding nodes together*/
				else
				{
					(tmp->next)->prev = tmp->prev;
					(tmp->prev)->next = tmp->next;
					tmp = tmp->prev;

				}
			}
		}
		return;
	}
}

/***
M_Init

initialize the memory allocator

Precondition: M_Init will not have already been called

Inputs: size 	- The space to be used from the OS in bytes

Output: int 	- is a 0 on sucess and -1 on failure

***/
int M_Init(int size)
{
	if(head != NULL || size <= 0) return -1;

	head = mmap(NULL, size, PROT_READ|PROT_WRITE, MAP_ANON|MAP_PRIVATE, -1, 0);

	if(head == MAP_FAILED)
	{
		fprintf(stderr, "ERROR: mmap Creation Failed\n");
		return -1;
	}

	foot = head;

	free_list = head;
	free_list->size = size - sizeof(node_t);
	free_list->next = NULL;
	free_list->prev = NULL;

	return 0;
}

/***
M_Alloc

Allocates a chunk of initilized memory (M_Init must be used first)

Precondition: There must be enough room 

Postcondition: The memory is allocated

Inputs:		size 	The space to be allocated for a chunk 

Outputs: 	void*	The pointer to the start of the allocated chunk
***/
void *M_Alloc(int size)
{
	/*If there is an invalid size, return NULL*/
	if(size <= 0) 
		return NULL;

	/*Align the size so it rounds up to the nearest M_BIT_SIZE*/
	if(size % M_BIT_SIZE != 0)
		size = size + M_BIT_SIZE - (size % M_BIT_SIZE);

	int total_size = size + sizeof(header_t);

	/*If the size requested is larger than the list AND there are no further nodes in the free list, return NULL*/
	if((total_size > free_list->size) && free_list->next == NULL)
	{
		fprintf(stderr, "ERROR: No Space\n");
		return NULL;
	}

	header_t *header_ptr, header_fill;
	node_t *tmp = NULL;

	
	header_fill.size = total_size;
	header_fill.magic = MAGIC;

	tmp = next_fit(total_size);

	if(tmp != NULL)
	{
		/*Free_list is the only node*/
		if(free_list->next == NULL && tmp == free_list)
		{
			free_list = (node_t*)((int*)tmp+total_size);
			free_list->prev = NULL;
			free_list->next = NULL;
			foot = free_list;

			/*If the total size is not the same free space remaining
			From before we've checked that it fits in free->size */
			if(total_size != free_list->size)
			{
				free_list->size = tmp->size -total_size;
			}
			/*The size needed is the same as the free space*/
			else
			{
				free_list->size = 0;
			}
		}
		/*Free_list is not the only node. This should only happen if we dont coalesce*/
		else
		{
			/*If tmp is as big as the size needed*/
			if(tmp->size == total_size)
			{
				/**/
				if(free_list->next != NULL && tmp == free_list)
					free_list = tmp->next;
				/*If temp isnt the foot make the previous node point to tmp and its connections*/
				else if(tmp != foot)
				{
					(tmp->prev)->next = tmp->next;
					(tmp->prev)->prev = tmp->prev;
				}
				else
				{
					foot = tmp->prev;
					(tmp->prev)->next = NULL;
				}
			}
			/*Else we need to make a new node*/
			else
			{
				node_t *tmp_node = (node_t*)((int*)tmp + total_size);
				tmp_node->size = tmp->size - total_size;

				if(tmp == free_list && free_list->next != NULL)
				{
					tmp_node->next = tmp->next;
					free_list = tmp_node;
				}
				else if(tmp != foot)
				{
					tmp_node->next = tmp->next;
					tmp_node->prev = tmp->prev;
					(tmp->prev)->next = tmp_node;
				}
				else
				{
					tmp_node->next = NULL;
					tmp_node->prev = tmp->prev;
					(tmp->prev)->next = tmp_node;
					foot = tmp_node;
				}
			}
		}

		tmp = (node_t*)((int*)tmp + sizeof(header_t));
		header_ptr = (header_t*)((int*)tmp - sizeof(header_t));
		*header_ptr = header_fill;

		return (void*) tmp;
	}
	/*No possible size in free_list*/
	else
		return NULL;
}

/***
M_Free

Frees the chunk with address p

Precondition: p is a valid memory address

Inputs: ptr 	- is a memory addres that has been allocated

Output: int 	- is a 0 on sucess and -1 on failure

***/
int M_Free(void *ptr)
{
	if(ptr == NULL)
	{
		fprintf(stderr, "ERROR: NULL pointer given to M_Free\n");
		return -1;
	}

	header_t *tmp_head = (header_t*)((int*)ptr - sizeof(header_t));
	/**
	printf("ptr Size: %d\n", tmp_head->size);
	printf("ptr MAGIC: %d\n", tmp_head->magic);
	**/

	if(tmp_head->magic != MAGIC)
	{
		fprintf(stderr, "ERROR: Invalid pointer given to M_Free\n");
		return -1;
	}
	else
	{
		/*Create temp pointer for the header that is being dealloced*/
		node_t *tmp_ptr = (node_t*)tmp_head;
		tmp_ptr->size = tmp_head->size;

		/*Create a tmp node for iterating throught the free list*/
		node_t *tmp = free_list;

		/*Get the address of the ptr to be freed*/
		int *address = (int*)tmp_ptr;

		if(free_list != NULL)
		{
			/*If the address is before the list*/
			if(address < (int*)tmp)
			{
				tmp_ptr->prev = NULL;
				tmp_ptr->next = tmp;
				tmp->prev = tmp_ptr;
				free_list = tmp_ptr;
			}
			/*If the address is after the free list*/
			else if(address > (int*)foot)
			{
				tmp_ptr->prev = foot;
				tmp_ptr->next = NULL;
				foot->next = tmp_ptr;
				foot = tmp_ptr;
			}
			/*Else its after the start of the list but before the end 
			  So we need to search the list now*/
			else
			{
				while(tmp != NULL)
				{
					if(address < (int*)tmp)
					{
						(tmp->prev)->next = tmp_ptr;
						tmp_ptr->prev = tmp->prev;
						tmp_ptr->next = tmp;
						tmp->prev = tmp_ptr;
						break;
					}
					else
					{
						tmp = tmp->next;
					}
				}
			}

		}
		else
		{
			free_list = tmp_ptr;
		}
		coalesce();
		return 0;
	}



}

/***
M_Display

Takes all memory and prints their sizes and addresses

Precondition: None

***/
void M_Display()
{
	printf("====================\n");
	printf("Head Node:\n");
	printf("Size:\t\t%d\n", free_list->size);
	printf("Address: \t%p\n", (int*)free_list);
	printf("Next Ptr: \t%p\n", free_list->next);
	printf("Prev Ptr: \t%p\n", free_list->prev);

	node_t *tmp = free_list->next;
	while(tmp != NULL)
	{
		printf("--------------------\n");
		printf("Next Node:\n");
		printf("Size:\t\t%d\n", tmp->size);
		printf("Address: \t%p\n", (int*)tmp);
		printf("Next Ptr: \t%p\n", tmp->next);
		printf("Prev Ptr: \t%p\n", tmp->prev);

		tmp = tmp->next;
	}
}

/***
main

The Test program
***/

int main( int argc, const char* argv[] )
{
	void *ptr1, *ptr2, *ptr3;

	/*Assertt Testing, Test for failure cases*/
	printf("====================\nBegin Assert Testing...\nThis will crash the program if the test doesn't fail as intended.\n====================\n");
	/*Initialize the memory space for testing*/
	assert(M_Init(1024) != -1);

	/*Test to ensure that M_Init will not call twice
	  Will assert if this successfully returns*/
	assert(M_Init(4096) != 0);

	/*Test that M_Alloc will ot alocate if there isnt enough room*/
	assert(M_Alloc(10000) == NULL);

	/*Test that M_Free, given a faulty pointer will not free anything*/
	ptr1 = NULL;
	assert(M_Free(ptr1) != 0);

	printf("End Assert Testing...\n====================\n\n\n");
	
	printf("====================\nBegin Functional Testing...\nThree pointers are created in order and alocated and deallocated to prove Functionality\n====================\n\n");

	M_Display();
	printf("Allocating ptr1\n");
	ptr1 = M_Alloc(32);
	M_Display();

	printf("====================\n");
	printf("Freeing ptr1\n");
	M_Free(ptr1);
	M_Display();

	printf("====================\n");
	printf("Allocating ptr1\n");
	ptr1 = M_Alloc(32);
	M_Display();

	printf("====================\n");
	printf("Allocating ptr2\n");
	ptr2 = M_Alloc(32);
	M_Display();

	printf("====================\n");
	printf("Allocating ptr3\n");
	ptr3 = M_Alloc(32);
	M_Display();

	printf("====================\n");
	printf("Freeing ptr1 and ptr3\n");
	M_Free(ptr1);
	M_Free(ptr2);
	M_Display();

	printf("====================\n");
	printf("Freeing ptr2\n");
	M_Free(ptr3);
	M_Display();

	return 0;
}
