# PCB's and Process Scheduling Simulation

## Introduction
- SFU CMPT 300: Operating Systems I, Individual Project: PCB's and Process Scheduling Simulation
- This application is to create an interactive operating system simulation which supports a few basic O/S functions. The simulation will be keyboard driven, and
will make reports to the screen. 
- The files of list.h and list.o are taken from the file of list.zip which is provided by the instructor. 
- It is assumed that there are three states for all processes: "RUNNING", "READY" and "BLOCKED".
- The init process exists as long as the simulation is running. It only runs when ready queues are all empty. It never blocks and can only be killed if it is the only process left in the system.


## Execution
- Compile: `make`
- Execute: `./main`
- Clean the object files: `make clean

## Commands

- `C`: Create a new process.
- `F`: Fork the currently running process.
- `K`: Kill the named process.
- `E`: Kill the currently running process.
- `Q`: Signal that the time quantum for the currently running process has expireed.
- `S`: Send message to another process (or send to itself)
- `R`: Receive a message.
- `Y`: Reply a message to a sender.
- `N`: Initialize a new semaphore.
- `P`: Execute the semaphore P (decrement) operation.
- `V`: Execute the semaphore V (increment) operation.
- `I`: Dump complete information about a particular process.
- `T`: Dump total information about processes, ready queues, blocked queues.

## License

This work is licensed under [Apache License, Version 2.0](https://www.apache.org/licenses/LICENSE-2.0) (or any later version). 

`SPDX-License-Identifier: Apache-2.0-or-later`

## Disclaimer

**This repository is *ONLY* for backup. Students should *NEVER* use this repository to finish their works, *IN ANY WAY*.**

It is expected that within this course, the highest standards of academic integrity will be maintained, in
keeping with SFU’s Policy S10.01, `Code of Academic Integrity and Good Conduct`.

In this class, collaboration is encouraged for in-class exercises and the team components of the assignments, as well
as task preparation for group discussions. However, individual work should be completed by the person
who submits it. Any work that is independent work of the submitter should be clearly cited to make its
source clear. All referenced work in reports and presentations must be appropriately cited, to include
websites, as well as figures and graphs in presentations. If there are any questions whatsoever, feel free
to contact the course instructor about any possible grey areas.

Some examples of unacceptable behaviour:
- Handing in assignments/exercises that are not 100% your own work (in design, implementation,
wording, etc.), without a clear/visible citation of the source.
- Using another student's work as a template or reference for completing your own work.
- Using any unpermitted resources during an exam.
- Looking at, or attempting to look at, another student's answer during an exam.
- Submitting work that has been submitted before, for any course at any institution.

All instances of academic dishonesty will be dealt with severely and according to SFU policy. This means
that Student Services will be notified, and they will record the dishonesty in the student's file. Students
are strongly encouraged to review SFU’s Code of Academic Integrity and Good Conduct (S10.01) available
online at: http://www.sfu.ca/policies/gazette/student/s10-01.html.

## Author

Zeyong, JIN

August 23, 2021
