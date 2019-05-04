#ifndef QUEUES_H
#define QUEUES_H


typedef struct my_queue my_queue;

struct my_queue {
	 int page; //physical page in memory
	 int virtual_page;
	 my_queue* next;
	 my_queue* prev;
};

//pages in physical memory
extern my_queue* memoryQ; 
extern my_queue* memoryQ_tail; 

void printQ(my_queue*);
void pushQ(my_queue**, my_queue**, my_queue**);
my_queue* popQ(my_queue**, my_queue**);
void removeQ(my_queue**, my_queue**, my_queue**);
void moveToTail(my_queue**, my_queue**, my_queue**);
int sizeQ(my_queue*);

my_queue* findPage(my_queue** head, int page);

#endif