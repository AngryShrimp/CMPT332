
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

#ifndef MONITOR_H
#define MONITOR_H

/*  Includes  */
#include <stdio.h>
#include <stdlib.h>
#include <standards.h>
#include <os.h>
#include <unistd.h>
#include <assert.h>
#include "list.h"

/*  Function Headers  */
void MonEnter();
void MonLeave();
void MonWait();
void MonSignal();
void MonInit();
void schedule();

#endif
