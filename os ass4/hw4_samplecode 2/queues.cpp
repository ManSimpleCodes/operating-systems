#include "scheduler.h"

//Jobs that have not READY yet (hence, not loaded into memory, etc...)
my_queue* jobq;
my_queue* job_tail;

//Jobs that are READY to run, but need access to the CPU
my_queue* readyq;
my_queue* ready_tail;

//Jobs running on a CPU.  This does not need to be a Queue, since we only have one CPU in this scenerio
my_queue* processing_head;
my_queue* processing_tail;


//Used for debugging.  Print the contents of queue
void printQ(my_queue* temp)
{
	while(temp!=NULL)
	{
		cout << "--Task: " << temp->this_task.taskid << " Arrive: " << temp->this_task.arrival_time << " State: " << temp->this_task.state << endl;
		temp=temp->next;
	}
}

void pushQ(my_queue** head, my_queue** node, my_queue** tail)
{
		my_queue* h = *head;
		my_queue* n = *node;
		my_queue* t = *tail;

		n->next = NULL;
		n->prev = NULL;

		//if queue is empty
		if (t == NULL && h == NULL)
			h = n;

		if (t != NULL)
		{
			t->next = n;
			n->prev = t;
		}
		t = n;

		*head = h;
		*tail = t;
		*node = n;
}


my_queue* popQ(my_queue** head, my_queue** tail)
{

	my_queue* h = *head;
	my_queue* t = *tail;
	my_queue* old_head = h;

	if (h != NULL)
	{
		h = h->next;

		//don't point to the rest of the queue
		old_head->next = NULL;
		old_head->prev = NULL;

		if (h != NULL)
			h->prev = NULL;
		else
			t = NULL;
	}
	else
		return NULL;

	*head = h;
	*tail = t;

	return old_head;
}


void removeQ(my_queue** head, my_queue** removeNode, my_queue** tail)
{
	my_queue* h = *head;
	my_queue* t = *tail;
	my_queue* n = *removeNode;

	my_queue* left = n->prev;
	my_queue* right = n->next;

	if(left != NULL)
		left->next = right;
	if (right != NULL)
		right->prev = left;
	if(n->prev == NULL)
		h = right;
	if(n->next == NULL)
		t = left;

	*head = h;
	*tail = t;
}


