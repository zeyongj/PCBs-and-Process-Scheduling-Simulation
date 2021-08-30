/*
 * @file process.h
 * Assignment 3: PCB's and Process Scheduling Simulation
 * PCB ADT
 */

#ifndef _PROCESS_H_
#define _PROCESS_H_
#include "list.h"

// Maximum length of the message between sender and receiver,
// according to the instructions, 40 char max
#define MESSAGE_MAX_LENGTH 42
#define CMD_MAX_LENGTH 10

// Three states of the process, running, ready or blocked 
enum ProcessState {
    RUNNING,
    READY,
    BLOCKED
};

// PCB data structure
typedef struct PCB_s PCB;
struct PCB_s {
    int pid;
    int priority;
    enum ProcessState state;
    char proc_message[MESSAGE_MAX_LENGTH];
};

// Semaphore data structure
typedef struct Semaphore_s Semaphore;
struct Semaphore_s {
    int val;  // the value of the semaphore
    List* blocked_process;  // a list of processes waiting on the semaphore
};

// Queues
List* high_priority_queue;  // ready queue with priority 0
List* mid_priority_queue;  // ready queue with priority 1
List* low_priority_queue;  // ready queue with priority 2
List* send_blocked_queue;  // a queue of processes waiting on a send operation
List* receive_blocked_queue;  // a queue of processes waiting on a receive operation
Semaphore* semaphores[5];  // an array of 5 semaphore pointers

PCB* init_process;  // the init process, we assume the init process always has PID=0
PCB* running_process;  // the currently running process
int pid;  // global pid counter, assigned to newly created processes

// Functions related to process scheduling simulation

// Initialize global queues
void init();

// Function to find the next process in the ready queue to execute, and return a pointer to its PCB. 
// If none is found, return the pointer to init process.
PCB* reSchedule();

// Function to create a process and put it on the appropriate ready queue,
// identified by the priority argument. Return the pid of the created process
// on success or -1 for failure.
int procCreate(int priority);

// Function to copy the currently running process and put it on the ready queue,
// the newly created process has the same priority as the original one. Attemping
// to copy the init process will result in failure. Return the pid of the resulting
// process or -1 for failure.
int procFork();

// Function to kill the named process and remove it from the system, identified by
// the parameter pid. Allowing to kill a process in any state.
// Return the pid of the killed process on success or -1 for failure.
int procKill(int pid);

// Function to kill the currently running process. Rescheduling required.
// Return the pid of the currently running process on success or -1 for failure.
int procExit();

// Function to signal that the time quantum for the currently running process
// has expired. Using round robin scheduling.
// Return the pid of the next scheduled process. We assume that using this function
// to signal the init process will do nothing.
int procQuantum();

// Function to send a message from the currently running process to another process
// the sender is blocked on a blocked queue until being replied.
// Return pid of the recipient on success (the recipient can be found) or -1 if the
// recipient doesn't exist.
int procSend(int pid, char* message);

// Function to receive a message. It checks if there is a message waiting for the
// currently executing process, it there is it receives it, otherwise it gets blocked
// on the receive blocked queue. (can be unblocked by the procSend routine and be ready)
// Return the next executing process's PID on success or -1 on failure.
int procReceive();

// Function to delivers reply to sender (blocked in the send blocked queue) and unblock
// the sender. Return the pid of the sender being replied.
int procReply(int pid, char* message);

// Function to initialize the named semaphore with the value given. ID's can take a value
// from 0 to 4. Initilization for each can only be done once. The initial value must be 0 or higher.
// Return the semaphore ID on success or -1 on failure.
int newSemaphore(int semID, int val);

// Function to execute the semaphore P operation on behalf of the runing process.
// Return 0 for success or -1 for failure. Decrement.
int semaphoreP(int semID);

// Function to execute the semaphore V operation on behalf of the running process.
// Return 0 for success or -1 for failure. Increment.
int semaphoreV(int semID);

// Function to dump complete state information of the process identified by the
// pid. Return its pid if found, else return -1.
int procInfo(int pid);

// Display all process queues and their contents.
void totalInfo();

// Function to free all allocated memory, invoked when the simulation terminates.
void freeAll();

#endif
