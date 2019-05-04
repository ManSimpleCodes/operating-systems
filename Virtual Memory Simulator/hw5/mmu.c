#include "mmu.h"
#include "memory.h"

int hits = 0;
int faults = 0;

//Find the index of the page table, based on a virtual address
int getVirtualPage(int virtualAddr)
{
        return (virtualAddr / pageSize);
}

//Convert a Virtual Address to a Physical Page. Returns -1 for a page fault
int getPhysicalPage(int virtualAddr)
{
        // first obtain virtual page and the get physcial page by looking into pageTable
        int vpage = getVirtualPage(virtualAddr); 
        return pageTable[vpage].pageFrame;
}

//Convert a virtual address into a physical address
int getPhysicalAddress(int virtualAddr)
{
        int physical_page = getPhysicalPage(virtualAddr);
        return physical_page + (virtualAddr % pageSize);
}


// Access Memory: Given the virtual address, access the page in memory. Returns the physical address
int access(int virtualAddr)
{
        int physicalAddr = getPhysicalAddress(virtualAddr);
	// if physcial page value is -1 then its is a page fault else a page hit
        int ppage = getPhysicalPage(virtualAddr);
        if(ppage == -1)
        	fault_handler(virtualAddr);
        else
        	hit(virtualAddr);
        return physicalAddr;
}


//Page was found in memory.  Update the queue - Move accessed page to tail to maintain LRU order.
void hit(int virtualAddr)
{
	int ppage = getPhysicalPage(virtualAddr);
	struct my_queue* page1 = findPage(&memoryQ, ppage);	// finding the page in queue by passing physcial page as parameter
	while(page1 != NULL)	 //loop executes only when obtained element has a value other than null
	{	
		moveToTail(&memoryQ, &page1, &memoryQ_tail);	//update the position of the queue
		reportChange(true, virtualAddr,  page1->page, -1);
		break;
	}
	hits++;		// inrement hits value 	
}


//Page Fault has occurred.  Trap to the OS and handle page fault.
void fault_handler(int virtualAddr)
{  
	struct my_queue* head = memoryQ;	//fininding the LRU page and removing it from queue
        removeQ(&memoryQ, &head, &memoryQ_tail);
	while(head != NULL)
	{
		pageTable[getVirtualPage(virtualAddr)].pageFrame = head->page;
        	pageTable[head->virtual_page].pageFrame = -1;	//setting the old page value to -1 and
       		pushQ(&memoryQ, &head, &memoryQ_tail);	 // push the  page to the end of the queue
        	reportChange(false, virtualAddr,  head->page, head->virtual_page);
        	head->virtual_page =  getVirtualPage(virtualAddr);
		break;
	}
 	faults++;	//increment faults value
 }


 //Used for outputing to standard out.   You do not need to edit this function, but you will likely want to call it.
  void reportChange(bool hit, int virtualAddr, int physical_page, int removed_page)
 {
        char *s0 = cat("Address = ", itoa(virtualAddr));
        char *s1 = "";
        if (hit)
                s1 = cat(" | HIT! Virtual Page=",itoa(getVirtualPage(virtualAddr)));
        else
                s1 = cat(" | PAGE FAULT! Virtual Page=",itoa(getVirtualPage(virtualAddr)));
        char *s2 = cat(" is in physical page=", itoa(physical_page));
        char *s3 = "";
        if (!hit)
                s3 = cat(" | removed virtual page: ",itoa(removed_page));
        print(NOTIFY,cat( cat(s0,s1), cat(s2,s3) ));
 }

                  
                                                                                                  

