#include "scheduler.h"

bool cpuBusy = false;

int totalWait = 0;
int processesStarted = 0; //if a job is blocked, it counts twice.
int maxWait = 0;

//Simply used for statistics reporting
double getAvgWait()
{
	return (double) totalWait / processesStarted;
}

//Routine triggered once per quantum (on simulated clock interrupt) to determine which
//process gets CPU time and maintains queues
int scheduler()
{
	int i;

	//cpu needs to be free to dispatch.....
	if(cpuBusy == false && readyq != NULL)
	{
		my_queue* head = popQ(&readyq, &ready_tail);
		dispatch(&head);
	}	
}


// This routine adds the specified task to the Processing queue.
// Maintaining FCFS philosophy, it is added to end
void dispatch(my_queue** t) {

	my_queue* task = *t;
	// pushing the process dispatched from ready queue into running queue
	pushQ(&processing_head, &task, &processing_tail);
	cpuBusy = true;
        task->this_task.state = RUNNING;
	cout << cpuTimer << "| Running "<< task->this_task.taskid;
	cout <<" RunTime "<< task->this_task.time_run <<"\n";
	//update Statistics
	int waitTime = cpuTimer - task->this_task.arrival_time;
	totalWait+=waitTime;
	processesStarted++;
	if (waitTime > maxWait)
		maxWait = waitTime;
	*t = task;	
	my_queue* current = processing_head;
	current->this_task.state = RUNNING; //process state is set to running
	current->this_task.time_run++;	//incrementing time_run variable for each process 
	//checks condition for process to block and update the arrival_timeand current quantum of the process and push it ito job queue
	if(current->this_task.est_runtime <= current->this_task.time_run)
	{  
		current->this_task.state = TERMINATED;
		updateState(current);
		cout << cpuTimer;
		cout << "| Terminted task : "<<current->this_task.taskid<<"\n";			
	}
	//after the process reaches estimate time it terminates
	else if(current->this_task.block_startTime <= current->this_task.time_run)
	{
		cout << cpuTimer;
		cout << "| Blocked task : "<<current->this_task.taskid<<"\n";
		//	cpuTimer = cpuTimer+1;
		cout << "Block Wait Time : " << current->this_task.block_waitTime << "\n";
		current->this_task.block_startTime = INT_MAX;
		current->this_task.state = BLOCKED;
		current->this_task.arrival_time = cpuTimer + current->this_task.block_waitTime + 1;
	        updateState(current);
	}
	//if the process has not completed its entire estimated time,it is kept back into the ready queue
	else 
	{      
		current->this_task.state = READY;
		updateState(current);
	}
	cpuBusy = false;	
	current = current->next;
}



//This routine updates the process states in the PCB (Process Control Block)
//Queues are updated maintaining FCFS philosophy
void updateState(my_queue* task)
{    
	switch(task->this_task.state) 
	{
        	case 0:
	    		{
			my_queue* proQ = popQ(&processing_head, &processing_tail);	//pop from running queue and
			pushQ(&readyq, &proQ, &ready_tail);	//push the same process into ready queue
	       		}
			break;
       
        	case 3:
			{
	        	my_queue* proQ = popQ(&processing_head, &processing_tail);	//pop the process from running queue and
			pushQ(&jobq, &proQ, &job_tail);		//push the same process into job queue
			} 
           		break;
        	case 5:
            		{
			popQ(&processing_head, &processing_tail);	//removing the process permanently
			}
            		break;
		default:
			break;
	}
}





