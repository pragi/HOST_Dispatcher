//
//  memory_block.c
//  dispatcher
//
// 
//  Copyright (c) 2011 __MyCompanyName__. All rights reserved.
//

#include "memory_block.h"

MabPtr memChk(MabPtr m, int size){

    int available;
    
    while (m) {
        
        available = m->size - m->allocated;
        
        if (available > size)
            return m;
        else
            m = m->next;
    }    
    
    return NULL;
}



MabPtr memAlloc(MabPtr m, int size){
    
    if (!m) {
        if ((m = malloc(sizeof(Mab)))){
            m->offset = 0;
            m->size = size;
            m->allocated = size;
            m->next = NULL;
            m->prev = NULL;
        }
        else
            fprintf(stderr, "Failed to malloc size for memory.\n");
    }
    else
        m->allocated = size;
    
    return m;
    
}



MabPtr memFree(MabPtr m){
    
    if (m->next || m->prev) {
        m->allocated = 0;
        m = memMerge(m);
    }
    else {
        free(m);
        m = NULL;
    }
    
    return m;    
}



MabPtr memMerge(MabPtr m){
    MabPtr temp;
    

    if (m->next && (m->next)->allocated == 0) {
        temp = m->next;
        m->size = m->size + temp->size;
        m->next = temp->next;
        
        if (m->next)
            (m->next)->prev = m;
        free(temp);
        temp = NULL;
    }
    
    if (m->prev && (m->prev)->allocated == 0) {
        temp = m;
        m = m->prev;
        
        m->size = m->size + temp->size;
        m->next = temp->next;
        
        if (m->next)
            (m->next)->prev = m;
        free(temp);
        temp = NULL;
    }
    
    
    
    return m;
    
}




MabPtr memSplit(MabPtr m, int size){
    
    MabPtr temp;
    
    if ((temp = malloc(sizeof(Mab)))) {
        temp->allocated = 0;
        temp->offset = size + m->offset;
        temp->size = m->size - size;
        
        temp->next = m->next;
        
        if (temp->next)
            (temp->next)->prev = temp;
        temp->prev = m;
        m->next = temp;
    }
    else
        fprintf(stderr, "Failed to malloc size for memory.\n");
    
    m->size = size;
    m->allocated = m->size;
    
    return m;
    
}


