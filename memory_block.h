//
//  memory_block.h
//  dispatcher
//
//  
//  Copyright (c) 2011 __MyCompanyName__. All rights reserved.
//

#ifndef dispatcher_memory_block_h
#define dispatcher_memory_block_h

#include <stdlib.h>
#include <stdio.h>

struct mab {
    int offset;
    int size;
    int allocated;
    struct mab *next;
    struct mab *prev;
};
typedef struct mab Mab;
typedef Mab *MabPtr;

MabPtr memChk(MabPtr m, int size);
MabPtr memAlloc(MabPtr m, int size);
MabPtr memFree(MabPtr m);
MabPtr memMerge(MabPtr m);
MabPtr memSplit(MabPtr m, int size);

#endif
