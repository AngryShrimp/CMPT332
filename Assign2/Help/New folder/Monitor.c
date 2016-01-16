
/*              *\
 * Wade Jensen  *
 * wbj297       *
 * 1107558      *
 * Group5       *
\*              */

/* ******************* *\
** Christopher Klein   **
** chk019              **
** 11105376            **
\* ******************* */


#include "Monitor.h"


static int enterSem;
static int urgentSem;
static int * condSems;

/* List Library Borrowed from group22 */
static LIST * enterQueue;
static LIST * urgentQueue;
static LIST ** condLists;

static int isLocked = 0;


/* Enter Monitor Status
 * If it is locked add to queue and add
 * to the enter semaphore's sleep counter */
void MonEnter(){
  if(isLocked){
    ListAdd(enterQueue, MyPid());
    P(enterSem);
  }else{
    isLocked = 1;
  }
}

/* Exit Monitor Status
 * Calls schedule which will finish the process */
void MonLeave(){
  schedule();
}

/* Monitor Wait
 * Added to condition variable list and call
 * schedule then add to the condtion variable
 * semaphore status */
void MonWait(int cv){
  
  ListAdd(condLists[cv], MyPid());
  schedule();
  P(condSems[cv]);
}

/* Monitor Signal
 * If we have something in the condition variable
 * list then remove it from that list and add it to 
 * the urguentQueue list and we release the condition
 * variable semaphore and add to the urguentQueue's semaphore */
void MonSignal(int cv){

  if(ListCount(condLists[cv]) > 0){
    ListRemove(condLists[cv]);
    ListAdd(urgentQueue, MyPid());
    V(condSems[cv]);
    P(urgentSem);
  }
}

/* Scheduler function
 * First check the urgentQueue, if there is something
 * in it then we remove it from the list and release the
 * semaphore for it.
 * If first check doesnt go through we do the exact same check
 * for the enterQueue list & semaphore
 * else we release the lock so if anything enters it goes right
 * ahead. */
void schedule(){
  if(ListCount(urgentQueue) > 0){
    ListRemove(urgentQueue);
    V(urgentSem);
  }
  else if(ListCount(enterQueue) > 0){
    ListRemove(enterQueue);
    V(enterSem);
  }else{
    isLocked = 0;
  }
}

/* Initilize the Monitor
 * we create an index, then the enterSem and urgentSem with
 * values 1, 0 respectivly.  Then the condition variable semaphore
 * and List are created as array sizes K which are givent to the
 * initilizing function.  The semaphore for condition variable
 * is given 0 for every spot in the array.
 * Then the enterQueue and urgentQueue are created */
void MonInit(int k){
  int i;
  enterSem = NewSem(1);
  urgentSem = NewSem(0);
  condSems = (int*)malloc(sizeof(int)*k);
  condLists = (LIST**)malloc(sizeof(LIST *)*k);
  for(i=0; i<k; i++){
    condSems[i] = NewSem(0);
    condLists[i] = ListCreate();
  }
  enterQueue = ListCreate();
  urgentQueue = ListCreate();
}

