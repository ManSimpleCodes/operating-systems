#ifndef QUEUES_H
#define QUEUES_H

struct task_PCB {
	int taskid;
	int userno;
	int state;
	int cpu;
	int est_runtime;
	int time_run;
// Special for this assignment
	int arrival_time;
	int block_startTime;
	int block_waitTime;
};

struct my_queue {
	 task_PCB this_task;
	 my_queue* next;
	 my_queue* prev;
};


extern my_queue* jobq;
extern my_queue* job_tail;
extern my_queue* readyq;
extern my_queue* ready_tail;
extern my_queue* processing_head;
extern my_queue* processing_tail;


void printQ(my_queue*);
void pushQ(my_queue**, my_queue**, my_queue**);
my_queue* popQ(my_queue**, my_queue**);
void removeQ(my_queue**, my_queue**, my_queue**);

#endif