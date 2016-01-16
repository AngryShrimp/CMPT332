 /*
 * Wade Jensen
 * wbj297
 * 1107558
 * Group5
 */

/* ******************* *\
** Christopher Klein   **
** chk019              **
** 11105376            **
\* *********************/

#ifndef MEM_MONITOR_H
#define MEM_MONITOR_H

#include "Monitor.h"
#include "list.h"

#define MEM_AMOUNT 8388608
#define FF 0
#define NF 1
#define FREE 0
#define HOLES 0
#define SEARCHED 1
#define FREEBLKS 2

typedef struct block{
  int size;
  int num;
} block;

typedef struct FirstFit{
  LIST * free;
  LIST * used;
} FirstFit;

typedef struct Stats{
  int searched;
  int holes;
  int freeblocks;
} Stats;

static FirstFit * ff;
static int numBlocks = 0;
static Stats * stats;


void * MyMalloc(int, int);
void MyFree(void *, int *, int);
void CleanupMemory();
int CompareBlocks(void *, void *);

void FFInit();
block * FFGetBlock(int);

#endif
