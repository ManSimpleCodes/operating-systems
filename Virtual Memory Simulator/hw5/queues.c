
#include "queues.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>


my_queue* memoryQ = NULL;  //Elements in Physical Memory (Queue Head)
my_queue* memoryQ_tail = NULL; //Elements in Physical Memory (Queue Tail)


//Used for debugging.  Print the contents of queue
void printQ(my_queue* temp)
{
	while(temp!=NULL)
	{
		print(LOG,cat("--Queue Element is: ", itoa(temp->page)));
		temp=temp->next;
	}
}

/*Find an element in the queue based on the physical page.  
Returns element in FoundNode, if not found (return NULL) */
my_queue* findPage(my_queue** head, int page)
{
	my_queue* current = *head;
	while(current!=NULL)
	{
		if (current->page == page)
		{
			print(LOG,cat("---FOUND: ",itoa(page)));
			return current;
		}
		current=current->next;
	}	
	return NULL;
}

//Returns the number of elements in the queue
int sizeQ(my_queue* head)
{
	my_queue* current = head;
	int i=0;
	while(current!=NULL)
	{
		i++;
		current=current->next;
	}	
	return i;
}

//Given a node in the queue, remove it from it's current location and put it at the end of the queue.
void moveToTail(my_queue** head, my_queue** node, my_queue** tail)
{
	removeQ(head,node,tail);
	print(LOG,"After RemoveQ");
	printQ(memoryQ);
	pushQ(head, node, tail);
	print(LOG,"After pushQ");
	printQ(memoryQ);
}

//Add a node to the end of the queue (node will equal tail)
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


//Remove the element at the end of the queue.  Returns the element that is removed.
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

//Remove element from the queue.  Element can be anywhere in the queue.  
void removeQ(my_queue** head, my_queue** removeNode, my_queue** tail)
{
	my_queue* h = *head;
	my_queue* t = *tail;
	my_queue* n = *removeNode;
	
	print(LOG, cat("REMOVING: ", itoa(n->page)));
	
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




