#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "queues.h"

#include  <stdio.h>
#include  <string.h>
#include  <sys/types.h>
#include <unistd.h>
#include <signal.h>

#include <sys/time.h>
#include <pthread.h>
#include <iostream>
#include <stdbool.h>
#include <stddef.h>
#include <assert.h>
#include <stdlib.h>

using namespace std;

const int INT_MAX = 2147483647;
extern int cpuTimer;
extern int maxWait;

#define TRUE 1
#define FALSE 0

#define READY 0
#define RUNNING 1
#define HALTED 2
#define BLOCKED 3
#define SUSPENDED 4
#define TERMINATED 5

double getAvgWait();

int scheduler();
void dispatch(my_queue**);
void updateState(my_queue*);
void process();

#endif