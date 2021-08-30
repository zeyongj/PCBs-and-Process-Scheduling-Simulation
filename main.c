/*
 * @file main.c
 * Assignment 3: PCB's and Process Scheduling Simulation
 * Driver routine for process scheduling simulation
 */

#include "list.h"
#include "process.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char* argv[]) {
    // Printing
    printf("*********************************************\n");
    printf("**** Process Scheduling Simulation Begin ****\n");
    printf("*********************************************\n");
    printf("******* A List Of Available Commands: *******\n");
    printf("C: Create\n");
    printf("F: Fork\n");
    printf("K: Kill\n");
    printf("E: Exit\n");
    printf("Q: Quantum\n");
    printf("S: Send\n");
    printf("R: Receive\n");
    printf("Y: Reply\n");
    printf("N: New Semaphore\n");
    printf("P: Semaphore P\n");
    printf("V: Semaphore V\n");
    printf("I: Procinfo\n");
    printf("T: Totalinfo\n");
    printf("*********************************************\n");

    // Initilizing
    init();
    char cmd[CMD_MAX_LENGTH];
    char param[CMD_MAX_LENGTH];
    char* find_char;
    char msg[MESSAGE_MAX_LENGTH];
    int res_pid;  // response pid return by functions
    // Begin simulation
    while (true) {  // event loop
        printf("Please Enter The Command or X to quit:\n");
        if (fgets(cmd, 10, stdin) == NULL) {  // fail
            perror("Fail to read from standard input!\n");
            exit(1);
        }
        if (strlen(cmd) != 2) {  // check whether input is legal
            printf("Invalid Input! Enter only the upper case letter list above. Please try again!\n");
        }
        // check commands
        switch (cmd[0]) {
            case 'C':
                printf("Ready to craete a process... Please enter the priority:\n0=high, 1=norm, 2=low.\n");
                if (fgets(param, 10, stdin) == NULL) {  // fail
                    perror("Fail to read from standard input!\n");
                    exit(1);
                }
                // check whether input is legal
                if (strlen(param) != 2 || (param[0] != '0' && param[0] != '1' && param[0] != '2')) {
                    printf("Invalid Input! Legal input: 0 or 1 or 2!\n");
                    break;
                }
                int c_pid = procCreate(param[0] - '0');
                if (c_pid == -1) {
                    printf("Fail to create a new process!\n");
                } else {
                    printf("Create a new process with PID: %d, Priority: %d\n", c_pid, param[0]-'0');
                }
                break;
            case 'F':
                printf("Ready to fork the currently running process...\n");
                res_pid = procFork();
                if (res_pid == -1) {
                    printf("Attempting to fork the init process results failure!\n");
                } else {
                    printf("Fork succeed. New process has PID: %d\n", res_pid);
                }
                break;
            case 'K':
                printf("Ready to kill the process... Please enter the PID:\n");
                if (fgets(param, 10, stdin) == NULL) {  // fail
                    perror("Fail to read from standard input!\n");
                    exit(1);
                }
                find_char = strchr(param, '\n');  // find the newline character
                if (find_char != NULL) {
                    *find_char = '\0';  // replace the newline character with a terminator
                }
                res_pid = procKill(atoi(param));
                if (res_pid == -1) {
                    printf("Fail to kill the named process.\n");
                } else if (res_pid == 0) {
                    printf("Init process is killed. Simulation terminates.\n");
                    exit(0);
                } else {
                    printf("Kill the process with PID: %d\n", res_pid);
                }
                break;
            case 'E':
                printf("Ready to exit the currently running process...\n");
                res_pid = procExit();
                if (res_pid == -1) {
                    printf("Fail to kill the currently running process (init process).\n");
                } else if (res_pid == 0) {
                    printf("Init process is killed. Simulation terminates.\n");
                    exit(0);
                } else {
                    printf("Kill the process with PID: %d\n", res_pid);
                }
                break;
            case 'Q':
                printf("Ready to signal that the currently running process's time quantum has expires...\n");
                res_pid = procQuantum();
                break;
            case 'S':
                printf("Ready to send message to another process... Please enter the PID of that process:\n");
                // read pid from input
                if (fgets(param, 10, stdin) == NULL) {  // fail
                    perror("Fail to read from standard input!\n");
                    exit(1);
                }
                find_char = strchr(param, '\n');  // find the newline character
                if (find_char != NULL) {
                    *find_char = '\0';  // replace the newline character with a terminator
                }
                // read message from input
                printf("Please enter the message you want to send to process with PID %d:\n", atoi(param));
                if (fgets(msg, 42, stdin) == NULL) {  // fail
                    perror("Fail to read from standard input!\n");
                    exit(1);
                }
                find_char = strchr(msg, '\n');  // find the newline character
                if (find_char != NULL) {
                    *find_char = '\0';  // replace the newline character with a terminator
                }
                res_pid = procSend(atoi(param), msg);
                if (res_pid == -1) {
                    printf("Send failed, unable to find the recipient!\n");
                } else {
                    printf("Send succeeded!\n");
                }
                break;
            case 'R':
                printf("Ready to receive message...\n");
                res_pid = procReceive();
                break;
            case 'Y':
                printf("Ready to reply message to another process... Please enter the PID of that process:\n");
                // read pid from input
                if (fgets(param, 10, stdin) == NULL) {  // fail
                    perror("Fail to read from standard input!\n");
                    exit(1);
                }
                find_char = strchr(param, '\n');  // find the newline character
                if (find_char != NULL) {
                    *find_char = '\0';  // replace the newline character with a terminator
                }
                // read message from input
                printf("Please enter the message you want to reply to process with PID %d:\n", atoi(param));
                if (fgets(msg, 42, stdin) == NULL) {  // fail
                    perror("Fail to read from standard input!\n");
                    exit(1);
                }
                find_char = strchr(msg, '\n');  // find the newline character
                if (find_char != NULL) {
                    *find_char = '\0';  // replace the newline character with a terminator
                }
                res_pid = procReply(atoi(param), msg);
                if (res_pid == -1) {
                    printf("Reply failed, unable to find the recipient!\n");
                } else {
                    printf("Reply succeeded!\n");
                }
                break;
            case 'N':
                printf("Ready to initialize a semaphore... Please enter the semaphore ID [0 to 4]:\n");
                // read semaphore ID from input
                if (fgets(param, 10, stdin) == NULL) {  // fail
                    perror("Fail to read from standard input!\n");
                    exit(1);
                }
                find_char = strchr(param, '\n');  // find the newline character
                if (find_char != NULL) {
                    *find_char = '\0';  // replace the newline character with a terminator
                }
                // read the second parameter (value) from input
                printf("Please enter the value of the semaphore:\n");
                if (fgets(msg, 42, stdin) == NULL) {  // fail
                    perror("Fail to read from standard input!\n");
                    exit(1);
                }
                find_char = strchr(msg, '\n');  // find the newline character
                if (find_char != NULL) {
                    *find_char = '\0';  // replace the newline character with a terminator
                }
                res_pid = newSemaphore(atoi(param), atoi(msg));
                if (res_pid == -1) {
                    printf("Fail to initialize the semaphore!\n");
                } else {
                    printf("Succeed in initilizing the semaphore!\n");
                }
                break;
            case 'P':
                printf("Ready to execute the semaphore P operation... Please enter the semaphore ID [0 to 4]:\n");
                // read semaphore ID from input
                if (fgets(param, 10, stdin) == NULL) {  // fail
                    perror("Fail to read from standard input!\n");
                    exit(1);
                }
                find_char = strchr(param, '\n');  // find the newline character
                if (find_char != NULL) {
                    *find_char = '\0';  // replace the newline character with a terminator
                }
                res_pid = semaphoreP(atoi(param));
                if (res_pid == -1) {
                    printf("Fail to execute the semaphore P operation!\n");
                } else {
                    printf("Succeed in executing the semaphore P operation!\n");
                }
                break;
            case 'V':
                printf("Ready to execute the semaphore V operation... Please enter the semaphore ID [0 to 4]:\n");
                // read semaphore ID from input
                if (fgets(param, 10, stdin) == NULL) {  // fail
                    perror("Fail to read from standard input!\n");
                    exit(1);
                }
                find_char = strchr(param, '\n');  // find the newline character
                if (find_char != NULL) {
                    *find_char = '\0';  // replace the newline character with a terminator
                }
                res_pid = semaphoreP(atoi(param));
                if (res_pid == -1) {
                    printf("Fail to execute the semaphore V operation!\n");
                } else {
                    printf("Succeed in executing the semaphore V operation!\n");
                }
                break;
            case 'I':
                printf("Ready to display information about the process... Please enter the PID of the process you want to check:\n");
                if (fgets(param, 10, stdin) == NULL) {  // fail
                    perror("Fail to read from standard input!\n");
                    exit(1);
                }
                find_char = strchr(param, '\n');  // find the newline character
                if (find_char != NULL) {
                    *find_char = '\0';  // replace the newline character with a terminator
                }
                res_pid = procInfo(atoi(param));
                if (res_pid == -1) {
                    printf("Fail to find the named process.\n");
                }
                break;
            case 'T':
                printf("**** Printing all process queues and their contents: ****\n");
                totalInfo();
                printf("********************* End of printing *******************\n");
                break;
            case 'X':
                exit(0);
                break;
        }
    }

    return 0;
}