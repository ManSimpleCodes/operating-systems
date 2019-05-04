#ifndef MMU_H
#define MMU_H

#include <stdbool.h>

extern int hits;
extern int faults;

int access(int virtualAddr);
void hit(int page);
void fault_handler(int virtuallAddr);

int getVirtualPage(int virtualAddr);
int getPhysicalPage(int virtualAddr);
int getPhysicalAddress(int virtualAddr);

void reportChange(bool, int virtualAddr, int physical_page, int removed_page);

#endif