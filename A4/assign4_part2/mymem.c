#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include "mymem.h"
#include <time.h>


/* The main structure for implementing memory allocation.
 * You may change this to fit your implementation.
 */

struct memoryList
{
  // doubly-linked list
  struct memoryList *last;
  struct memoryList *next;

  int size;            // How many bytes in this block?
  char alloc;          // 1 if this block is allocated,
                       // 0 if this block is free.
  void *ptr;           // location of block in memory pool.
};

strategies myStrategy = NotSet;    // Current strategy


size_t mySize;
void *myMemory = NULL;

static struct memoryList *head;
static struct memoryList *next;


/* initmem must be called prior to mymalloc and myfree.

   initmem may be called more than once in a given exeuction;
   when this occurs, all memory you previously malloc'ed  *must* be freed,
   including any existing bookkeeping data.

   strategy must be one of the following:
		- "best" (best-fit)
		- "worst" (worst-fit)
		- "first" (first-fit)
		- "next" (next-fit)
   sz specifies the number of bytes that will be available, in total, for all mymalloc requests.
*/

void merge(void) {
	//printf("in merge\n");
	struct memoryList* curBlock;
	curBlock = head;
	while(curBlock->next != NULL) {
		if(curBlock->alloc == 0 && curBlock->next->alloc == 0) {
			struct memoryList* temp1;
			temp1 = curBlock->next;
			curBlock->size += temp1->size;
			curBlock->next = temp1->next;
			if(temp1->next != NULL) {
				temp1->next->last = curBlock;
			}
			if(next == temp1) {
				next = curBlock;
			}
			free(temp1);
		}
		else {
			curBlock = curBlock->next;
		}
	}
	//printf("merge worked\n");
}

static void split(struct memoryList* block, int size) {
	//printf("in split\n");
	struct memoryList* newBlock;
	newBlock = malloc(sizeof(struct memoryList));
	newBlock->ptr = block->ptr + block->size;
	//printf("%p\n", block->ptr);
	//printf("%p\n", newBlock->ptr);
	newBlock->size = size;
	newBlock->alloc = 0;
	newBlock->next = block->next;
	newBlock->last = block;
	block->next = newBlock;
	//printf("%p\n", block->next->ptr);

	merge();
	//printf("split worked\n");
}

static void alloc(struct memoryList* block, int requested) {
	//printf("in alloc\n");
	int newBlockSize = block->size - requested;
	block->alloc = 1;
	block->size = requested;
	if(newBlockSize > 0) {
		split(block, newBlockSize);
	}
	//printf("alloc worked\n");
}

void initmem(strategies strategy, size_t sz)
{
	myStrategy = strategy;

	/* all implementations will need an actual block of memory to use */
	mySize = sz;

	if (myMemory != NULL) free(myMemory); /* in case this is not the first time initmem2 is called */

	struct memoryList* tempBlock;
	tempBlock = head;
	while(head != NULL) {
		head = head->next;
		free(tempBlock);
		tempBlock = head;
	}


	myMemory = malloc(sz);
	//printf("%p\n", myMemory);
	
	head = malloc(sizeof(struct memoryList));
	head->size = sz;
	head->alloc = 0;
	head->ptr = myMemory;
	next = head;
	head->next = NULL;
	head->last = NULL;
	//printf("worked1\n");
}

/* Allocate a block of memory with the requested size.
 *  If the requested block is not available, mymalloc returns NULL.
 *  Otherwise, it returns a pointer to the newly allocated block.
 *  Restriction: requested >= 1 
 */

void *mymalloc(size_t requested)
{
	assert((int)myStrategy > 0);

	struct memoryList* curBlock;
	
	switch (myStrategy)
	  {
	  case NotSet: 
	            return NULL;
	  case First:
				//struct memoryList* curBlock; 
				curBlock = head;
				while(curBlock != NULL) {
					//printf("worked\n");
					if(curBlock->size >= requested && curBlock->alloc == 0) {
						alloc(curBlock, requested);
						break;
					}
					curBlock = curBlock->next;
				}
				//printf("T: %p\n", curBlock->ptr);
				if(next != NULL && curBlock != NULL) {
					next = curBlock;
					alloc(next, requested);
					return next->ptr;
				}
	            return NULL;
	  case Best:
				curBlock = head;
				struct memoryList* minBlock;
				minBlock = NULL;
				while(curBlock != NULL) {
					if(curBlock->size >= requested && curBlock->alloc == 0) {
						if(minBlock == NULL) {
							minBlock = curBlock;
						}
						else if(curBlock->size < minBlock->size) {
							minBlock = curBlock;
						}
					}
					curBlock = curBlock->next;
				}
				if(next != NULL && minBlock != NULL) {
					next = minBlock;
					alloc(next, requested);
					return next->ptr;
				}
	            return NULL;
	  case Worst: 
				curBlock = head;
				struct memoryList* maxBlock; 
				maxBlock = NULL;
				while(curBlock != NULL) {
					if(curBlock->size >= requested && curBlock->alloc == 0) {
						if(maxBlock == NULL) {
							maxBlock = curBlock;
						}
						else if(curBlock->size > maxBlock->size) {
							maxBlock = curBlock;
						}
					}
					curBlock = curBlock->next;
				}
				if(next != NULL && maxBlock != NULL) {
					next = maxBlock;
					alloc(next, requested);
					return next->ptr;
				}
	            return NULL;
	  case Next:
				//printf("in next\n");
				//printf("%p\n", next->ptr);
				if(next->alloc == 0 && next->size >= requested) {
					struct memoryList* tempBlock = next;
					alloc(tempBlock, requested);
					next = (tempBlock->next != NULL) ? tempBlock->next : head;
					return tempBlock->ptr;
				}
				else { 
					if(next->next != NULL) {
						curBlock = next->next;
					}
					else {
						curBlock = head;
					}
					while(curBlock != next) {
						if(curBlock->size >= requested && curBlock->alloc == 0) {
							alloc(curBlock, requested);
							next = (curBlock->next != NULL) ? curBlock->next : head;
							return curBlock->ptr;
						}
						if(curBlock->next != NULL) {
							curBlock = curBlock->next;
						}
						else {
							curBlock = head;
						}
					}
				}
	            return NULL;
	  }
	return NULL;
}


/* Frees a block of memory previously allocated by mymalloc. */
void myfree(void* block)
{
	struct memoryList* tempBlock;
	tempBlock = head;
	while(tempBlock != NULL) {
		if(tempBlock->ptr == block) {
			tempBlock->alloc = 0;
			break;
		}
		tempBlock = tempBlock->next;
	}
	//printf("%p freed\n", tempBlock->ptr);
	merge();
	return;
}

/****** Memory status/property functions ******
 * Implement these functions.
 * Note that when we refer to "memory" here, we mean the 
 * memory pool this module manages via initmem/mymalloc/myfree. 
 */

/* Get the number of contiguous areas of free space in memory. */
int mem_holes()
{
	int numHoles = 0;
	struct memoryList* tempBlock;
	tempBlock = head;
	while(tempBlock != NULL) {
		if(tempBlock->alloc == 0) {
			numHoles++;
		}
		tempBlock = tempBlock->next;
	}
	return numHoles;
}

/* Get the number of bytes allocated */
int mem_allocated()
{
	int numAllocated = 0;
	struct memoryList* tempBlock;
	tempBlock = head;
	while(tempBlock != NULL) {
		if(tempBlock->alloc == 1) {
			numAllocated++;
		}
		tempBlock = tempBlock->next;
	}
	return numAllocated;
}

/* Number of non-allocated bytes */
int mem_free()
{
	int numFree = 0;
	struct memoryList* tempBlock;
	tempBlock = head;
	while(tempBlock != NULL) {
		if(tempBlock->alloc == 0) {
			numFree += tempBlock->size;
		}
		tempBlock = tempBlock->next;
	}
	return numFree;
}

/* Number of bytes in the largest contiguous area of unallocated memory */
int mem_largest_free()
{
	int numMax = 0;
	struct memoryList* maxBlock;
	struct memoryList* tempBlock;
	tempBlock = head;
	maxBlock = NULL;
	while(tempBlock != NULL) {
		if(tempBlock->alloc == 0) {
			if(maxBlock == NULL) {
				maxBlock = tempBlock;
			}
			else if(tempBlock->size > maxBlock->size){
				maxBlock = tempBlock;
			}
		}
		tempBlock = tempBlock->next;
	}
	if(maxBlock == NULL) {
		numMax = 0;
	}
	else {
		numMax = maxBlock->size;
	}
	return numMax;
}

/* Number of free blocks smaller than "size" bytes. */
int mem_small_free(int size)
{
	int numSmallFree = 0;
	struct memoryList* tempBlock;
	tempBlock = head;
	while(tempBlock != NULL) {
		if(tempBlock->alloc == 0) {
			if(tempBlock->size <= size){
				numSmallFree++;
			}
		}
		tempBlock = tempBlock->next;
	}
	return numSmallFree;
}       

char mem_is_alloc(void *ptr)
{
    char byteAlloc = 0;
	struct memoryList* tempBlock;
	tempBlock = head;
	while(tempBlock != NULL) {
		if(tempBlock->ptr == ptr){
			if(tempBlock->alloc == 1) {
				byteAlloc = 1;
			}
			else{
				byteAlloc = 0;
			}
			break;
		}
		tempBlock = tempBlock->next;
	}
	return byteAlloc;
}

/* 
 * Feel free to use these functions, but do not modify them.  
 * The test code uses them, but you may ind them useful.
 */


//Returns a pointer to the memory pool.
void *mem_pool()
{
	return myMemory;
}

// Returns the total number of bytes in the memory pool. */
int mem_total()
{
	return mySize;
}


// Get string name for a strategy. 
char *strategy_name(strategies strategy)
{
	switch (strategy)
	{
		case Best:
			return "best";
		case Worst:
			return "worst";
		case First:
			return "first";
		case Next:
			return "next";
		default:
			return "unknown";
	}
}

// Get strategy from name.
strategies strategyFromString(char * strategy)
{
	if (!strcmp(strategy,"best"))
	{
		return Best;
	}
	else if (!strcmp(strategy,"worst"))
	{
		return Worst;
	}
	else if (!strcmp(strategy,"first"))
	{
		return First;
	}
	else if (!strcmp(strategy,"next"))
	{
		return Next;
	}
	else
	{
		return 0;
	}
}


/* 
 * These functions are for you to modify however you see fit.  These will not
 * be used in tests, but you may find them useful for debugging.
 */

/* Use this function to print out the current contents of memory. */
void print_memory()
{
	struct memoryList* tempBlock;
	tempBlock = head;
	while(tempBlock != NULL) {
		printf("S: %p\n", tempBlock->ptr);
		tempBlock = tempBlock->next;
	}
	return;
}

/* Use this function to track memory allocation performance.  
 * This function does not depend on your implementation, 
 * but on the functions you wrote above.
 */ 
void print_memory_status()
{
	printf("%d out of %d bytes allocated.\n",mem_allocated(),mem_total());
	printf("%d bytes are free in %d holes; maximum allocatable block is %d bytes.\n",mem_free(),mem_holes(),mem_largest_free());
	printf("Average hole size is %f.\n\n",((float)mem_free())/mem_holes());
}

/* Use this function to see what happens when your malloc and free
 * implementations are called.  Run "mem -try <args>" to call this function.
 * We have given you a simple example to start.
 */
void try_mymem(int argc, char **argv) {
        strategies strat;
	void *a, *b, *c, *d, *e;
	if(argc > 1)
	  strat = strategyFromString(argv[1]);
	else
	  strat = First;
	
	
	/* A simple example.  
	   Each algorithm should produce a different layout. */
	
	initmem(strat,500);
	
	a = mymalloc(100);
	b = mymalloc(100);
	c = mymalloc(100);
	myfree(b);
	d = mymalloc(50);
	myfree(a);
	e = mymalloc(25);
	
	print_memory();
	print_memory_status();
	
}
