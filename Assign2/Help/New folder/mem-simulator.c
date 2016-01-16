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
\* ******************* */


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <standards.h>
#include <os.h>
#include "malloc-monitor.h"

static int freeprob;
static int numthreads;
static int numallocs;
static int maxallocsize;
static int finished;

PROCESS FirstFitProc(void* arg){
  
  int i, r, s, retval, argn;
  void * ptrs[numallocs];
  retval = 1;
  argn = (int)arg;
  srand(argn);
  FFInit();
  
  for(i=0; i<numallocs; i++){
    /*printf("%d: Allocating: %d\n", MyPid(), i);*/
    /*printf("Hello World!\n");*/
    s = (rand() % maxallocsize)+1;
    ptrs[i] = MyMalloc(s, FF);
    r = (rand() % 99)+1;
    if(r <= freeprob){
      while(retval != 0){
	r = rand() % (i+1);
	/*printf("%d: Attempting to Free: %d\n", MyPid(), r);*/
	MyFree(ptrs[r], &retval, FF);
	if(retval == 1){
	  /*printf("%d: %d was not being used\n", MyPid(), r);*/
	} else {
	  /*printf("%d: %d has been Freed\n", MyPid(), r);*/
	}
      }
    }
    retval = 1;
    /*printf("%d: Freeing: %d\n", MyPid(), i);
      MyFree(ptrs[i], FF);*/
    /*printf("%d: Going to sleep\n", MyPid());*/
    Sleep(10);
  }

  printf("Thread %d Finished\n", MyPid());
  finished++;

  if(numthreads <= finished){
    MyMemStats(FF, HOLES, stats);
    MyMemStats(FF, SEARCHED, stats);
    MyMemStats(FF, FREEBLKS, stats);
  }
}

PROCESS NextFitProc(void* arg){

}

int mainp(int argc,char* argv[]){
  
  int ffproc[numthreads], i, input = 0;
  MonInit(1);
  finished = 0;

  if(argc != 5){
    printf("Usage: mem-simulator-test numthreads numallocs freeprob maxallocsize\n");
    return 1;
  }

  numthreads = atoi(argv[1]);
  numallocs = atoi(argv[2]);
  freeprob = atoi(argv[3]);
  maxallocsize = atoi(argv[4]);

  for(i=0; i<numthreads; i++){
    ffproc[i] = Create((void(*)()) FirstFitProc, 1600000, "FirstFit", (void *)(i+1), NORM, USR);
  }

  /*
  else{
    memManage = Create((void(*)()) BestFit, 16000, "BestFit", void(*), NORM,
		       USER);
  }
  */
  return 0;
}
