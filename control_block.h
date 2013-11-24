//
//  control_block.h
//  dispatcher
//
//  
//  Copyright (c) 2011 __MyCompanyName__. All rights reserved.
//

#ifndef dispatcher_control_block_h
#define dispatcher_control_block_h

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <signal.h>
#include <sys/wait.h>
#include "memory_block.h"

#define MAXARGS 10

struct pcb {
    pid_t pid;
    char *args[MAXARGS];
    int arrivaltime;
    int remainingcputime;
    int priority;
    int mem_alloc;
    int printer;
    int scanner;
    int modem;
    int cd_drive;
    int status;
    struct pcb *next;
    struct mab *mab;
    
};
typedef struct pcb Pcb;
typedef Pcb *PcbPtr;
/***********************************
 status
 0 - Terminated
 1 - Ready
 2 - Running
 3 - Suspended
 4 - Initialized
 5 - NULL
 ***********************************/


PcbPtr startPcb(PcbPtr process);
PcbPtr terminatePcb(PcbPtr process);
PcbPtr suspendPcb(PcbPtr process);
PcbPtr createnullPcb(void);
PcbPtr enqPcb (PcbPtr headofQ, PcbPtr process);
PcbPtr deqPcb (PcbPtr *headofQ);





#endif
