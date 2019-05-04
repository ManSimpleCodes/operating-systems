#include "memory.h"


int KB = 1024;
int pageSize = 4096;  //in bytes

int physicalPages = 8;
int virtualPages = 64;

void initialize()
{
	initVirtualMemory();
	initPhysicalMemory();
}

//Sizes are in KB;
void initVirtualMemory()
{
	int i;
	for(i = 0; i < virtualPages; i++)
	{
		struct virtualPage vp;
		vp.pageFrame = -1;
		pageTable[i] = vp;
	}	
}

//Fills physical memory with default pages.  
void initPhysicalMemory()
{
	int i;
	for(i=0;i<physicalPages;i++)
	{
		struct my_queue *new_physical_page = (struct my_queue*) malloc(sizeof(struct my_queue));
		//need error handling here.....
		
		new_physical_page->page = i; 
		new_physical_page->virtual_page = -1;  //not in virtual memory
		pushQ(&memoryQ,&new_physical_page,&memoryQ_tail);
	}
}
