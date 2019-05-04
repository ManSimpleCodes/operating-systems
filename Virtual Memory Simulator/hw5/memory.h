#ifndef MEMORY_H
#define MEMORY_H

#include "queues.h"
#include "utils.h"
#include "mmu.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

extern int pageSize;
extern int KB;
extern int pageSize;  //in bytes

extern int physicalPages ;
extern int virtualPages ;

struct virtualPage {
	 int pageFrame;
};

struct virtualPage pageTable[64]; 


void initialize();
void initVirtualMemory();
void initPhysicalMemory();


#endif