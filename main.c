//
//  main.c
//  dispatcher
//
// 
//  Copyright (c) 2011 __MyCompanyName__. All rights reserved.
//

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include "control_block.h"
#include "memory_block.h"

#define MEM_AVAILABLE 1024
void print_output(PcbPtr process); 


// Resources ------------------------

int printers;
int scanners;
int modems;
int cd_drives;


// Dispatcher queues ----------------

PcbPtr input_queue;
//PcbPtr rr_queue;
PcbPtr real_time_queue;
PcbPtr user_job_queue;
PcbPtr feedback_queue_1;
PcbPtr feedback_queue_2;
PcbPtr feedback_queue_3;

// Memory blocks ---------------------
MabPtr user_mem_block;
MabPtr real_time_mem_block;
    
int dispatch_timer;



int main (int argc, const char * argv[])
{
    PcbPtr process = NULL;
    PcbPtr temp = NULL;
    PcbPtr process2;
    FILE *input_file;
    char buffer[100];
    
    if (argc != 2)
    {
        fprintf(stderr, "Must have exactly one input parameter (disptch file list)\n");
        return -1;
    }

    
    
    if (!(input_file = freopen(argv[1], "r", stdin))){
        fprintf(stderr, "cant open file");
        return -1;
    }
    

    // 1. Dispatcher queue initialization
    
    input_queue = NULL;
    real_time_queue = NULL;
    user_job_queue = NULL;
    feedback_queue_1 = NULL;
    feedback_queue_2 = NULL;
    feedback_queue_3 = NULL;
    
    // 2. Memory and resource structure initialization
    
    printers = 2;
    scanners = 1;
    modems = 1;
    cd_drives = 2;
    
            
    
    if (!memChk(user_mem_block, MEM_AVAILABLE)) {
        user_mem_block = memAlloc(user_mem_block, MEM_AVAILABLE);
        user_mem_block = memSplit(user_mem_block, 64);
        
        real_time_mem_block = user_mem_block;
        
        user_mem_block = user_mem_block->next;
        real_time_mem_block->next = NULL;
        user_mem_block->prev = NULL;
    }
    else {
        fprintf(stderr, "memchck failed to find enough free memory.\n");
        return -1;
    }
    


    // 3. FIll input queue from dispatch list file
    
     while (!feof(input_file)) {
        if (fgets(buffer, 64, input_file)) {
            char *buf_ptr = buffer;
            temp = createnullPcb();
            
            temp->arrivaltime = atoi(strtok(buf_ptr," ,"));
            temp->priority = atoi(strtok(NULL," ,"));
            temp->remainingcputime = atoi(strtok(NULL," ,"));
            temp->mem_alloc = atoi(strtok(NULL," ,"));
            temp->printer = atoi(strtok(NULL," ,"));
            temp->scanner = atoi(strtok(NULL," ,"));
            temp->modem = atoi(strtok(NULL," ,"));
            temp->cd_drive = atoi(strtok(NULL," ,"));
            temp->status = 4;
            
            input_queue = enqPcb(input_queue, temp);

        }
    }
    
    
    // 4. Start dispatcher timer
    
    process = NULL;
    dispatch_timer = 0;


    
    // 5. While queues aren't emptry or there is a process running
    
    while (input_queue || real_time_queue || user_job_queue || feedback_queue_3 || feedback_queue_2 || feedback_queue_1 || process) {
        

        int mem_avai;

        PcbPtr input_pro, user_pro;

        MabPtr mem_block;
                
        
        
        // i. unload input_queue and enque on real-time or user-job
        
        while (input_queue && input_queue->arrivaltime <= dispatch_timer){
            
            input_pro = deqPcb(&input_queue);
            
            
            if (input_pro->priority == 0){
                
                input_pro->mab = memAlloc(real_time_mem_block, 64);
                real_time_queue = enqPcb(real_time_queue, input_pro);
            }
            else if (input_pro->priority <= 3) {
                user_job_queue = enqPcb(user_job_queue, input_pro);
            }
            else {
                exit (-1);
            }
            
        }
        
        // ii. unload user_job_queue and enque on one of the priority feedback queues
        
        while (user_job_queue && printers >= user_job_queue->printer && scanners >= user_job_queue->scanner && modems >= user_job_queue->modem && cd_drives >= user_job_queue->cd_drive && (mem_block = memAlloc(user_mem_block, user_job_queue->mem_alloc))) {
            
            //a. dequeue process
            user_pro = deqPcb(&user_job_queue);
            user_pro->status = 1; // status = ready
            
            // b. allocate memory
            mem_avai = mem_block->size - mem_block->allocated;
            
            if (user_pro->mem_alloc == mem_avai)
                user_pro->mab = memAlloc(mem_block, user_pro->mem_alloc);
            else
                user_pro->mab = memSplit(mem_block, user_pro->mem_alloc);
            
            // c. allocate resources
            printers = printers - user_pro->printer;
            scanners = scanners - user_pro->scanner;
            modems = modems - user_pro->modem;
            cd_drives = cd_drives - user_pro->cd_drive;
            
            // d. enqueue on queue
            
           if (user_pro->priority == 1) 
                feedback_queue_1 = enqPcb(feedback_queue_1, user_pro);
            else if (user_pro->priority == 2)
                feedback_queue_2 = enqPcb(feedback_queue_2, user_pro);
            else if (user_pro->priority == 3) 
                feedback_queue_3 = enqPcb(feedback_queue_3, user_pro);
            else
                exit (-1);
            
        }
        
        // iii. if a process if running
        
        if (process) {
            
            //a. decrememnt remaining cputime
            process->remainingcputime = process->remainingcputime - 1;
            
            //b. if times up send SIGINT (call terminatepcb) to terminate process,
            //   free memory and resources for user and process structure memory.
            if ((process->remainingcputime <= 0)) {
                process = terminatePcb(process);
                process->status = 0;
            
                if (process->priority == 0)
                    process->mab->allocated =0;
                else {
                    memFree(process->mab);
                    printers = printers + process->printer;
                    scanners = scanners + process->scanner;
                    modems = modems + process->modem;
                    cd_drives = cd_drives + process->cd_drive;
                }
                
                free(process);
                process = NULL;
                
            }
            else if ((real_time_queue || feedback_queue_3 || feedback_queue_2 || feedback_queue_1) && (process->priority > 0)) {
                
                process = suspendPcb(process);
                process->status = 3;
                
                
                if (process->priority < 3)
                    process->priority = process->priority + 1;
                
                if (process->priority == 1) 
                    exit(-1);
                else if (process->priority == 2)
                    feedback_queue_2 = enqPcb(feedback_queue_2, process);
                else if (process->priority == 3) 
                    feedback_queue_3 = enqPcb(feedback_queue_3, process);
                else
                    exit (-1);
                
                
                process = NULL;
            }
        }
        
        // iv. if no current process + real time and feedback not empty
        
        if ((!process) && (feedback_queue_3 || feedback_queue_2 || feedback_queue_1 || real_time_queue)) {
            
            //dequeue from the highest available priority.
            if (real_time_queue)
                process2 = deqPcb(&real_time_queue);
            else if (feedback_queue_1)
                process2 = deqPcb(&feedback_queue_1);
            else if (feedback_queue_2)
                process2 = deqPcb(&feedback_queue_2);
            else
                process2 = deqPcb(&feedback_queue_3);
            
            // b. if suspended then restart otherwise start it
            if (process2->status == 3){
                if (kill(process2->pid, SIGCONT))
                    process2 = NULL;
                else 
                    process2->status = 2;

            }
            else {
                
                process2 = startPcb(process2);
                //process2->status = 2;
                print_output(process2);
                //execvp(process2->args[0], process2->args);
            }
            
            // c. set it as currently running process
            process = process2;
        }
        
        // v. sleep for one second
        
        sleep(1);
        
        // vi. increment timer
        
        dispatch_timer = dispatch_timer + 1;
                    
    }
 
    memFree(real_time_mem_block);
    real_time_mem_block = NULL;
    
    memFree(user_mem_block);
    user_mem_block = NULL;
 
 
    return 0;
}

void print_output(PcbPtr process)
{
    char *status_str;
    
    if(process->status == 0)
        status_str = "Terminated";
    else if(process->status == 1)
        status_str = "Ready";
    else if(process->status == 2)
        status_str = "Running";
    else if(process->status == 3)
        status_str = "Suspended";
    else if(process->status == 4)
        status_str = "Initialized";
    else
        status_str = "NULL";
    
    
    fprintf (stdout, "pid\t arrive\t prior\t cpu\t offset\t Mbytes\t prn\t scn\t modem\t cd\t status\n");
    fprintf(stdout, "%d\t %d\t %d\t %d\t %d\t %d\t %d\t %d\t %d\t %d\t %s\t\n",
            process->pid, process->arrivaltime, process->priority, process->remainingcputime, process->mab->offset, process->mem_alloc, process->printer, process->scanner, process->modem,process->cd_drive,status_str);
     execvp(process->args[0], process->args);
}


