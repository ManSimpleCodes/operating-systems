#include "scheduler.h"
#include <iostream>
#include <sstream>
#include <string>

int taskID = 0;
int cpuTimer = 0;

//Takes line from file and initializes PCB node
my_queue* constructTask(string s)
{
	struct my_queue *new_task = (struct my_queue*) malloc(sizeof(struct my_queue));
        new_task->this_task.taskid = taskID++;
	new_task->this_task.state = READY;
	new_task->this_task.est_runtime = INT_MAX;
	new_task->this_task.time_run = 0;
	new_task->this_task.arrival_time = INT_MAX;
	new_task->this_task.block_startTime = INT_MAX;
	new_task->this_task.block_waitTime = INT_MAX;

    char buffer[256];
    istringstream iss(s);
    int count = 0;
    do
    {
          string sub;
          iss >> sub;
          switch(count)
          {
		  	case 1:
		 		new_task->this_task.taskid = atoi(sub.c_str());
			case 3:
				new_task->this_task.arrival_time = atoi(sub.c_str());
			case 5:
				new_task->this_task.est_runtime = atoi(sub.c_str());
			case 7:
				new_task->this_task.block_startTime = atoi(sub.c_str());
			case 8:
				new_task->this_task.block_waitTime = atoi(sub.c_str());


		  }
		 count++;
    } while (iss);
    return new_task;
}


int main(void)
{
    	//Read input file and load process jobs in jobs Queue
    	string t;
    	while(getline(cin, t))
    	{
        	cout << "file text is = " << t << endl;
		my_queue* new_task = constructTask(t);
		pushQ(&jobq, &new_task, &job_tail);

	}
	cout << "Reporting items in JOB queue" << endl;
	printQ(jobq);


	//RUN SIMULATION
	cpuTimer=0; //each increment represents one quantum.
	while(jobq != NULL || readyq != NULL || processing_head != NULL && cpuTimer < 1000)
	{
		cpuTimer++;
		my_queue* current = jobq;

		while(current!=NULL)
		{
			//task has arrived and is can be moved to readyQ
			if (current->this_task.arrival_time <= cpuTimer)
			{
				cout << "ADDING TO READYQ: " << current->this_task.taskid << endl;
				removeQ(&jobq, &current, &job_tail);
				pushQ(&readyq, &current, &ready_tail);
				current = jobq;
			}
			else
				current = current->next;
		}
		scheduler();

	}

	cout << "---------------------" << endl;
	cout << "Total Processing Time: " << cpuTimer << endl;
	cout << "Average Wait Time: " << getAvgWait() << endl;
	cout << "Max Wait Time: " << maxWait << endl;


    return 0;
}
