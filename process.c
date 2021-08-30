/*
 * @file process.c
 * Assignment 3: PCB's and Process Scheduling Simulation
 * PCB ADT implementations
 */

#include "process.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern List* high_priority_queue;
extern List* mid_priority_queue;
extern List* low_priority_queue;
extern List* send_blocked_queue;
extern List* receive_blocked_queue;
extern Semaphore* semaphores[5];
extern PCB* init_process;
extern PCB* running_process;
extern int pid;

void init() {
    // Initialize queues
    if ((high_priority_queue = List_create()) == NULL) {
        perror("Fail to create the list!\n");
        exit(1);
    }
    if ((mid_priority_queue = List_create()) == NULL) {
        perror("Fail to create the list!\n");
        exit(1);
    }
    if ((low_priority_queue = List_create()) == NULL) {
        perror("Fail to create the list!\n");
        exit(1);
    }
    if ((send_blocked_queue = List_create()) == NULL) {
        perror("Fail to create the list!\n");
        exit(1);
    }
    if ((receive_blocked_queue = List_create()) == NULL) {
        perror("Fail to create the list!\n");
        exit(1);
    }
    // Initialize the semaphore pointer array to all nullptr
    for (int i=0; i<5; i++) {
        semaphores[i] = NULL;
    }
    // Initialize the initial process
    if ((init_process = (PCB*)malloc(sizeof(PCB))) == NULL) {
        perror("Faile to allocate memory!\n");
        exit(1);
    }
    init_process->pid = pid++;  // init process has PID=0
    init_process->priority = 0;
    init_process->state = RUNNING;
    running_process = init_process;
    printf("The init process is created and running. Pid: %d. Priority: %d\n", init_process->pid, init_process->priority);
}

PCB* reSchedule() {
    // Scheduling happens
    int proc_count = 0;
    PCB* next_ptr = NULL;
    PCB* res_ptr = NULL;
    proc_count += List_count(high_priority_queue);
    proc_count += List_count(mid_priority_queue);
    proc_count += List_count(low_priority_queue);
    if (proc_count == 0) {
        init_process->state = RUNNING;
        res_ptr = init_process;
    } else {
        // Check ready queues
        if (List_count(high_priority_queue) != 0) {
            next_ptr = (PCB*)List_first(high_priority_queue);
            next_ptr = (PCB*)List_remove(high_priority_queue);
        } else if (List_count(mid_priority_queue) != 0) {
            next_ptr = (PCB*)List_first(mid_priority_queue);
            next_ptr = (PCB*)List_remove(mid_priority_queue);
        } else if (List_count(low_priority_queue) != 0) {
            next_ptr = (PCB*)List_first(low_priority_queue);
            next_ptr = (PCB*)List_remove(low_priority_queue);
        }
        next_ptr->state = RUNNING;
        res_ptr = next_ptr;
    }
    // The case when sender is unblocked and replied (currently running)
    if (strlen(res_ptr->proc_message) > 0) {
        printf("Process with PID %d, priority %d has text message: %s\n", res_ptr->pid, res_ptr->priority, res_ptr->proc_message);
        // Clear the message field after disaplaying
        memset(res_ptr->proc_message, 0, sizeof(res_ptr->proc_message));
    }

    return res_ptr;
}

int procCreate(int priority) {
    PCB* new_process;
    // Allocate heap memory for a new process
    if ((new_process = (PCB*)malloc(sizeof(PCB))) == NULL) {
        perror("Faile to allocate memory for a new process!\n");
        return -1;
    }
    // Initilize fields for the newly created process, increment global PID
    new_process->pid = pid++;
    new_process->priority = priority;
    new_process->state = READY;

    if (running_process->pid == 0) {  // if the currently running process is the init process, then preempt it
        running_process->state = READY;
        new_process->state = RUNNING;
        running_process = new_process;
    } else {  // if the currently running process is not the init process, then put the new one in the ready queue
        if (priority == 0) {
            List_append(high_priority_queue, (void*)new_process);
        } else if (priority == 1) {
            List_append(mid_priority_queue, (void*)new_process);
        } else {
            List_append(low_priority_queue, (void*)new_process);
        }
    }

    return new_process->pid;
}

int procFork() {
    if (running_process->pid == 0) {  // check whether the currently running process is the init process
        return -1;
    }
    PCB* new_process;
    return procCreate(running_process->priority);
}

int procKill(int pid) {
    int proc_count = 0;
    PCB* cur_ptr;
    int res;
    if (pid == 0) {  // special case: check whether we need to kill the init process
        proc_count += List_count(high_priority_queue);
        proc_count += List_count(mid_priority_queue);
        proc_count += List_count(low_priority_queue);
        proc_count += List_count(send_blocked_queue);
        proc_count += List_count(receive_blocked_queue);
        for (int i=0; i<5; i++) {
            if (semaphores[i] != NULL) {
                proc_count += List_count(semaphores[i]->blocked_process);
            }
        }
        if (proc_count == 0) {  // if there are no other processes, init process can be killed
            freeAll();
            return 0;
        }
    } else if (running_process->pid == pid) {  // if the running process is to be killed
        // Scheduling required in this case
        if (List_count(high_priority_queue) != 0) {
            List_first(high_priority_queue);
            cur_ptr = (PCB*)List_remove(high_priority_queue);
            cur_ptr->state = RUNNING;
            free(running_process);
            running_process = cur_ptr;
        } else if (List_count(mid_priority_queue) != 0) {
            List_first(mid_priority_queue);
            cur_ptr = (PCB*)List_remove(mid_priority_queue);
            cur_ptr->state = RUNNING;
            free(running_process);
            running_process = cur_ptr;
        } else if (List_count(low_priority_queue) != 0) {
            List_first(low_priority_queue);
            cur_ptr = (PCB*)List_remove(low_priority_queue);
            cur_ptr->state = RUNNING;
            free(running_process);
            running_process = cur_ptr;
        } else {  // if all other processes are blocked, the init process runs
            init_process->state = RUNNING;
            free(running_process);
            running_process = init_process;
        }
        return pid;
    } else {  // process in the ready queue or blocked queue is to be killed
        // No scheduling required in this case
        // Check ready queues
        if (List_count(high_priority_queue) != 0) {
            cur_ptr = (PCB*)List_first(high_priority_queue);
            while (cur_ptr != NULL && cur_ptr->pid != pid) {
                cur_ptr = (PCB*)List_next(high_priority_queue);
            }
            if (cur_ptr != NULL) {  // founded
                cur_ptr = (PCB*)List_remove(high_priority_queue);
                free(cur_ptr);
                return pid;
            }
        }
        if (List_count(mid_priority_queue) != 0) {
            cur_ptr = (PCB*)List_first(mid_priority_queue);
            while (cur_ptr != NULL && cur_ptr->pid != pid) {
                cur_ptr = (PCB*)List_next(mid_priority_queue);
            }
            if (cur_ptr != NULL) {  // founded
                cur_ptr = (PCB*)List_remove(mid_priority_queue);
                free(cur_ptr);
                return pid;
            }
        }
        if (List_count(low_priority_queue) != 0) {
            cur_ptr = (PCB*)List_first(low_priority_queue);
            while (cur_ptr != NULL && cur_ptr->pid != pid) {
                cur_ptr = (PCB*)List_next(low_priority_queue);
            }
            if (cur_ptr != NULL) {  // founded
                cur_ptr = (PCB*)List_remove(low_priority_queue);
                free(cur_ptr);
                return pid;
            }
        }
        // Check send and receive block queues
        if (List_count(send_blocked_queue) != 0) {
            cur_ptr = (PCB*)List_first(send_blocked_queue);
            while (cur_ptr != NULL && cur_ptr->pid != pid) {
                cur_ptr = (PCB*)List_next(send_blocked_queue);
            }
            if (cur_ptr != NULL) {  // founded
                cur_ptr = (PCB*)List_remove(send_blocked_queue);
                free(cur_ptr);
                return pid;
            }
        }
        if (List_count(receive_blocked_queue) != 0) {
            cur_ptr = (PCB*)List_first(receive_blocked_queue);
            while (cur_ptr != NULL && cur_ptr->pid != pid) {
                cur_ptr = (PCB*)List_next(receive_blocked_queue);
            }
            if (cur_ptr != NULL) {  // founded
                cur_ptr = (PCB*)List_remove(receive_blocked_queue);
                free(cur_ptr);
                return pid;
            }
        }
        // Check the waiting queues on 5 semaphores
        for (int i=0; i<5; i++) {
            if (semaphores[i] != NULL && List_count(semaphores[i]->blocked_process) != 0) {
                cur_ptr = (PCB*)List_first(semaphores[i]->blocked_process);
                while (cur_ptr != NULL && cur_ptr->pid != pid) {
                    cur_ptr = (PCB*)List_next(semaphores[i]->blocked_process);
                }
                if (cur_ptr != NULL) {  // founded
                    cur_ptr = (PCB*)List_remove(semaphores[i]->blocked_process);
                    free(cur_ptr);
                    return pid;
                }
            }
        }
    }

    return -1;  // failure case
}

int procExit() {
    int proc_count = 0;
    if (running_process->pid == 0) {  // if the currently running process is the init process
        proc_count += List_count(high_priority_queue);
        proc_count += List_count(mid_priority_queue);
        proc_count += List_count(low_priority_queue);
        proc_count += List_count(send_blocked_queue);
        proc_count += List_count(receive_blocked_queue);
        for (int i=0; i<5; i++) {
            if (semaphores[i] != NULL) {
                proc_count += List_count(semaphores[i]->blocked_process);
            }
        }
        if (proc_count == 0) {  // if there are no other processes, init process can be killed
            freeAll();
            return 0;
        }
    } else {
        // Scheduling required in this case
        int res = running_process->pid;
        procKill(running_process->pid);
        return res;
    }

    return -1;
}

int procQuantum() {
    int proc_count = 0;
    PCB* next_ptr;
    proc_count += List_count(high_priority_queue);
    proc_count += List_count(mid_priority_queue);
    proc_count += List_count(low_priority_queue);
    if (proc_count == 0) {
    // If all ready queues are empty, assign a time quantum to the current process again
    // and nothing changes, running process still running
        printf("The ready queues are empty and the currently running process gets another time quantum.\n");
        printf("Process with PID %d, priority %d is still running.\n", running_process->pid, running_process->priority);
        return running_process->pid;
    } else {
    // otherwise look for the next process to execute from ready queues
        printf("The currently running process has PID %d, priority %d. Its removed from CPU and append to the ready queue.\n", running_process->pid, running_process->priority);
        running_process->state = READY;  // state changes from running to ready
        if (running_process->priority == 0) {
            List_append(high_priority_queue, (void*)running_process);
        } else if (running_process->priority == 1) {
            List_append(mid_priority_queue, (void*)running_process);
        } else {
            List_append(low_priority_queue, (void*)running_process);
        }
        running_process = reSchedule();
        printf("Process with PID %d, priority %d gets CPU time and is running next.\n", running_process->pid, running_process->priority);
    }

    return running_process->pid;
}

int procSend(int pid, char* message) {
    // Try to find pid from ready queues or receive_block_queue
    // We doesn't need to check processes in the send block queue
    // Because it may cause deadlock
    // Check ready queues first
    PCB* cur_ptr;
    PCB* next_ptr;
    int found = false;
    // Check whether the message is sent to itself (silly case)
    if (pid == running_process->pid) {
        cur_ptr = running_process;  // sender and recipient are the same
        found = true;
    }

    // Search the ready queues
    if (found == false && List_count(high_priority_queue) != 0) {
        cur_ptr = (PCB*)List_first(high_priority_queue);
        while (cur_ptr != NULL && cur_ptr->pid != pid) {
            cur_ptr = (PCB*)List_next(high_priority_queue);
        }
        if (cur_ptr != NULL) {  // founded
            found = true;
        }
    }
    if (found == false && List_count(mid_priority_queue) != 0) {
        cur_ptr = (PCB*)List_first(mid_priority_queue);
        while (cur_ptr != NULL && cur_ptr->pid != pid) {
            cur_ptr = (PCB*)List_next(mid_priority_queue);
        }
        if (cur_ptr != NULL) {  // founded
            found = true;
        }
    }
    if (found == false && List_count(low_priority_queue) != 0) {
        cur_ptr = (PCB*)List_first(low_priority_queue);
        while (cur_ptr != NULL && cur_ptr->pid != pid) {
            cur_ptr = (PCB*)List_next(low_priority_queue);
        }
        if (cur_ptr != NULL) {  // founded
            found = true;
        }
    }
    // Search the receive block queue
    if (found == false && List_count(receive_blocked_queue) != 0) {
        cur_ptr = (PCB*)List_first(receive_blocked_queue);
        while (cur_ptr != NULL && cur_ptr->pid != pid) {
            cur_ptr = (PCB*)List_next(receive_blocked_queue);
        }
        if (cur_ptr != NULL) {  // founded in the receive block queue
            found = true;
            cur_ptr->state = READY;  // change from block to ready
            // Unblock and add to the ready queue
            if (cur_ptr->priority == 0) {
                List_append(high_priority_queue, (void*)cur_ptr);
            } else if (cur_ptr->priority == 1) {
                List_append(mid_priority_queue, (void*)cur_ptr);
            } else {
                List_append(low_priority_queue, (void*)cur_ptr);
            }
        }
    }

    if (found == true) {
        // If recipient found, block the sender and do scheduling
        if (running_process->pid != 0) {
            printf("The currently running process with PID %d, priority %d is blocked.\n", running_process->pid, running_process->priority);
            printf("Message %s is sent to process with PID %d\n", message, cur_ptr->pid);
            running_process->state = BLOCKED;
            List_append(send_blocked_queue, (void*)running_process);
        }
        char msg_extended[MESSAGE_MAX_LENGTH];
        sprintf(msg_extended, "Sender's PID is %d, text is %s", running_process->pid, message);
        strcpy(cur_ptr->proc_message, msg_extended);
        running_process = reSchedule();
        printf("The next running process has PID %d, priority %d\n", running_process->pid, running_process->priority);
        return pid;
    }

    return -1;
}

int procReceive() {
    if (strlen(running_process->proc_message) == 0) {  // the case when no message arrives
        // Block the running process (if it is not the init process) and do scheduling
        if (running_process->pid != 0) {
            printf("The message has not arrived.\n");
            printf("The currently running process with PID %d, priority %d is blocked.\n", running_process->pid, running_process->priority);
            running_process->state = BLOCKED;
            List_append(receive_blocked_queue, (void*)running_process);
        }
        // Scheduling happens
        running_process = reSchedule();
        printf("The next running process has PID %d, priority %d\n", running_process->pid, running_process->priority);
        return running_process->pid;
    } else {
        printf("The current running process with PID %d, priority %d, received the message: %s\n",running_process->pid, running_process->priority, running_process->proc_message);
        // Clear the message field and nothing else changes
        memset(running_process->proc_message, 0, sizeof(running_process->proc_message));
        return running_process->pid;
    }

    return -1;
}

int procReply(int pid, char* message) {
    char msg_extended[MESSAGE_MAX_LENGTH] = "reply received, text is: ";
    strcat(msg_extended, message);
    PCB* cur_ptr;
    cur_ptr = (PCB*)List_first(send_blocked_queue);
    while (cur_ptr != NULL && cur_ptr->pid != pid) {
        cur_ptr = (PCB*)List_next(send_blocked_queue);
    }
    if (pid == 0) {  // if we replying the init process, it doesn't need to be unblocked
        cur_ptr = init_process;
        strcpy(cur_ptr->proc_message, msg_extended);
        return pid;
    } else if (cur_ptr != NULL) {  // founded, reply the message to it and unblock the sender
        strcpy(cur_ptr->proc_message, msg_extended);
        List_remove(send_blocked_queue);  // unblock
        if (cur_ptr->priority == 0) {  // add to the ready queue
            List_append(high_priority_queue, (void*)cur_ptr);
        } else if (cur_ptr->priority == 1) {
            List_append(mid_priority_queue, (void*)cur_ptr);
        } else {
            List_append(low_priority_queue, (void*)cur_ptr);
        }
        return pid;
    }

    return -1;
}

int newSemaphore(int semID, int val) {
    if (semID < 0 || semID > 4 || val < 0) {
        return -1;
    }
    if (semaphores[semID] == NULL) {  // semaphore can be initialized
        if ((semaphores[semID] = (Semaphore*)malloc(sizeof(Semaphore))) == NULL) {
            perror("Faile to allocate memory!\n");
            exit(1);
        }
        semaphores[semID]->val = val;  // the initial value of the semaphore
        semaphores[semID]->blocked_process = List_create();
        if (semaphores[semID]->blocked_process == NULL) {
            perror("Fail to create the blocked list for the semaphore!\n");
            exit(1);
        }
        return semID;
    }

    return -1;
}

int semaphoreP(int semID) {
    if (semID < 0 || semID > 4 || semaphores[semID] == NULL) {  // invalid semID
        return -1;
    }

    semaphores[semID]->val -= 1;  // decrement the semaphore value
    if (semaphores[semID] -> val < 0) {  // we need to block the currently running process
        if (running_process->pid == 0) {  // init process cannot be blocked
            return -1;
        }
        printf("The currently running process with PID %d, priority %d is blocked!\n", running_process->pid, running_process->priority);
        running_process->state = BLOCKED;
        List_append(semaphores[semID]->blocked_process, (void*)running_process);
        running_process = reSchedule();
    } else {
        printf("The currently running process with PID %d, priority %d is not blocked!\n", running_process->pid, running_process->priority);
    }

    return semID;
}

int semaphoreV(int semID) {
    if (semID < 0 || semID > 4 || semaphores[semID] == NULL) {  // invalid semID
        return -1;
    }
    PCB* cur_ptr;
    semaphores[semID]->val += 1;  // increment the semaphore value
    if (semaphores[semID] -> val >= 0 && List_count(semaphores[semID]->blocked_process) != 0) {  // we need to unblock a process
        cur_ptr = (PCB*)List_first(semaphores[semID]->blocked_process);
        List_remove(semaphores[semID]->blocked_process);
        printf("Process with PID %d, priority %d is readied!\n", cur_ptr->pid, cur_ptr->priority);
        if (cur_ptr->priority == 0) {
            List_append(high_priority_queue, (void*)cur_ptr);
        } else if (cur_ptr->priority == 1) {
            List_append(mid_priority_queue, (void*)cur_ptr);
        } else {
            List_append(low_priority_queue, (void*)cur_ptr);
        }
    } else {
        printf("No process is not unblocked!\n");
    }

    return semID;
}

int procInfo(int pid) {
    int found = false;
    PCB* cur_ptr = NULL;

    if (pid == 0) {  // init process
        found = true;
        cur_ptr = init_process;
    } else {  // try to find the process with pid in all the queues
        // Check ready queues
        if (List_count(high_priority_queue) != 0) {
            cur_ptr = (PCB*)List_first(high_priority_queue);
            while (cur_ptr != NULL && cur_ptr->pid != pid) {
                cur_ptr = (PCB*)List_next(high_priority_queue);
            }
            if (cur_ptr != NULL) {  // founded
                found = true;
            }
        }
        if (found == false && List_count(mid_priority_queue) != 0) {
            cur_ptr = (PCB*)List_first(mid_priority_queue);
            while (cur_ptr != NULL && cur_ptr->pid != pid) {
                cur_ptr = (PCB*)List_next(mid_priority_queue);
            }
            if (cur_ptr != NULL) {  // founded
                found = true;
            }
        }
        if (found == false && List_count(low_priority_queue) != 0) {
            cur_ptr = (PCB*)List_first(low_priority_queue);
            while (cur_ptr != NULL && cur_ptr->pid != pid) {
                cur_ptr = (PCB*)List_next(low_priority_queue);
            }
            if (cur_ptr != NULL) {  // founded
                found = true;
            }
        }
        // Check send and receive block queues
        if (found == false && List_count(send_blocked_queue) != 0) {
            cur_ptr = (PCB*)List_first(send_blocked_queue);
            while (cur_ptr != NULL && cur_ptr->pid != pid) {
                cur_ptr = (PCB*)List_next(send_blocked_queue);
            }
            if (cur_ptr != NULL) {  // founded
                found = true;
            }
        }
        if (found == false && List_count(receive_blocked_queue) != 0) {
            cur_ptr = (PCB*)List_first(receive_blocked_queue);
            while (cur_ptr != NULL && cur_ptr->pid != pid) {
                cur_ptr = (PCB*)List_next(receive_blocked_queue);
            }
            if (cur_ptr != NULL) {  // founded
                found = true;
            }
        }
        // Check the waiting queues on 5 semaphores
        for (int i=0; i<5; i++) {
            if (found == false && semaphores[i] != NULL && List_count(semaphores[i]->blocked_process) != 0) {
                cur_ptr = (PCB*)List_first(semaphores[i]->blocked_process);
                while (cur_ptr != NULL && cur_ptr->pid != pid) {
                    cur_ptr = (PCB*)List_next(semaphores[i]->blocked_process);
                }
                if (cur_ptr != NULL) {  // founded
                    found = true;
                }
            }
        }
    }
    // If the queried process is found, report it.
    if (found == true) {
        printf("**** Printing information about the queried process ****\n");
        printf("Process PID: %d\n", cur_ptr->pid);
        printf("Process priority: %d\n", cur_ptr->priority);
        printf("Process state: ");
        if (cur_ptr->state == READY) {
            printf("READY\n");
        } else if (cur_ptr->state == RUNNING) {
            printf("RUNNING\n");
        } else {
            printf("BLOCKED\n");
        }
        printf("Process carry message: %s\n", cur_ptr->proc_message);
        printf("********************* End of printing ******************\n");
        return pid;
    }

    return -1;
}

void totalInfo() {
    PCB* cur_ptr;
    printf("The currently running process's PID is: %d, Priority: %d\n", running_process->pid, running_process->priority);
    // Display information of all ready queues
    printf("There are %d processes in the ready queue with priority 0: ", List_count(high_priority_queue));
    cur_ptr = (PCB*)List_first(high_priority_queue);  // move the pointer to the head
    while (cur_ptr) {
        printf("PID: %d, ", cur_ptr->pid);
        cur_ptr = (PCB*)List_next(high_priority_queue);
    }
    printf("\nThere are %d processes in the ready queue with priority 1: ", List_count(mid_priority_queue));
    cur_ptr = (PCB*)List_first(mid_priority_queue);  // move the pointer to the head
    while (cur_ptr) {
        printf("PID: %d, ", cur_ptr->pid);
        cur_ptr = (PCB*)List_next(mid_priority_queue);
    }
    printf("\nThere are %d processes in the ready queue with priority 2: ", List_count(low_priority_queue));
    cur_ptr = (PCB*)List_first(low_priority_queue);  // move the pointer to the head
    while (cur_ptr) {
        printf("PID: %d, ", cur_ptr->pid);
        cur_ptr = (PCB*)List_next(low_priority_queue);
    }
    // Display information of send and receive blocked queues
    printf("\nThere are %d processes in the send blocked queue: ", List_count(send_blocked_queue));
    cur_ptr = (PCB*)List_first(send_blocked_queue);  // move the pointer to the head
    while (cur_ptr) {
        printf("PID: %d priority %d, ", cur_ptr->pid, cur_ptr->priority);
        cur_ptr = (PCB*)List_next(send_blocked_queue);
    }
    printf("\nThere are %d processes in the receive blocked queue: ", List_count(receive_blocked_queue));
    cur_ptr = (PCB*)List_first(receive_blocked_queue);  // move the pointer to the head
    while (cur_ptr) {
        printf("PID: %d priority %d, ", cur_ptr->pid, cur_ptr->priority);
        cur_ptr = (PCB*)List_next(receive_blocked_queue);
    }
    // Display information of semaphores' blocked queues
    for (int i=0; i<5; i++) {
        if (semaphores[i] != NULL) {
            printf("\nThere are %d processes in semaphore's (id: %d, value: %d) blocked queue: ", List_count(semaphores[i]->blocked_process), i, semaphores[i]->val);
            cur_ptr = (PCB*)List_first(semaphores[i]->blocked_process);  // move the pointer to the head
            while (cur_ptr) {
                printf("PID: %d priority %d, ", cur_ptr->pid, cur_ptr->priority);
                cur_ptr = (PCB*)List_next(semaphores[i]->blocked_process);
            }
        }
    }
    printf("\n");
}

void freeAll() {
    // Deallocate memory
    List_free(high_priority_queue, NULL);
    List_free(mid_priority_queue, NULL);
    List_free(low_priority_queue, NULL);
    List_free(send_blocked_queue, NULL);
    List_free(receive_blocked_queue, NULL);
    for (int i=0; i<5; i++) {
        if (semaphores[i] != NULL) {
            List_free(semaphores[i]->blocked_process, NULL);
            free(semaphores[i]);
        }
    }
    free(init_process);
}
