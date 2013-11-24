//
//  control_block.c
//  dispatcher
//
//  
//  Copyright (c) 2011 __MyCompanyName__. All rights reserved.
//

#include "control_block.h"

PcbPtr startPcb(PcbPtr process){
    
    if ((process->pid = fork())== 0) {
        process->pid = getpid();
        process->status = 2;
    }
    else {
        kill(process->pid, SIGCONT);
    }

    return process;
    
}



PcbPtr terminatePcb(PcbPtr process){
    
    int temp;
    
    if (kill(process->pid, SIGINT)) {
        fprintf(stderr, "couldn't terminate process" );
    }
        
    waitpid(process->pid, &temp, WUNTRACED);
    
    return process;
        
}


PcbPtr suspendPcb(PcbPtr process){
    
    int temp;
    
    if (kill(process->pid, SIGTSTP)) {
        fprintf(stderr, "couldn't suspend process" );
    }
    
    waitpid(process->pid, &temp, WUNTRACED);
    
    return process;
    
}

PcbPtr createnullPcb(void){
    
    PcbPtr process;
    
    if ((process = malloc(sizeof(Pcb))))
    {
        process->args[0] = "process";
        process->args[1] = NULL;
        process->arrivaltime = 0;
        process->remainingcputime = 0;
        process->priority = 0;
        process->mem_alloc = 0;
        process->printer = 0;
        process->scanner = 0;
        process->modem = 0;
        process->cd_drive = 0;
        process->mab = NULL;
        process->next = NULL;
        
        return process;
    }
    else
    {
        
        fprintf(stderr, "malloc failed creating null pcb");
        return NULL;
          
    }
}


PcbPtr enqPcb (PcbPtr headofQ, PcbPtr process){
    
    PcbPtr temp;
    
    if (!headofQ) { 
        headofQ = process;
    }
    else {
        temp = headofQ;
        while (temp->next)
            temp = temp->next;
        temp->next = process;
    }
    
    return headofQ;
}


PcbPtr deqPcb (PcbPtr *headofQ){
    PcbPtr temp;
    fprintf(stdout, "dequeued\n");
    if (headofQ) {
        temp = *headofQ;
        *headofQ = (*headofQ)->next;
        temp->next = NULL;
        return temp;
        
    }
    else {
        return NULL;   
    }
}

/* PcbPtr deqPcb (PcbPtr *headofQ){
    PcbPtr temp;
    
    if (headofQ) {
        temp = *headofQ;
        *headofQ = (*headofQ)->next;
        temp->next = NULL;
        return temp;
    }
    else {
        return NULL;   
    }
} */

