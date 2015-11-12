SUMISSION CONTENTS

In this submission are four files:
	[1] A README.txt document (this file)
	[2] prog2.cpp, the C++ source code for the problem solution
	[3] A makefile to compile prog2.cpp
	[4] A PDF containing the assignment
The makefile comes with two targets, a PHONY clean target to remove
outdated binaries and a target to build prog2. Usage:
	make clean	<----- Cleans up old binaries    
	make prog2	<----- Compiles prog2.cpp into runnnable object file prog2.o
In addition, there are two global variables inside prog2.cpp which determine the output of the
program. Setting OUTPUT to 1 will cause the program to display the information requested by the 
assignment PDF. Setting DEBUG to 1 will instead display debug messages about the behavior of the
threads. Do not set both to 1 simultaneously or your terminal will look very messy!

PROBLEM STATEMENT

The Classic Dining Philosophers Problem:
Five philosophers are seated around a circular table.  Each philosopher has a plate in front of him.  
In the middle of the table is a bowl of spaghetti.  The spaghetti is so slippery that a philosopher 
needs two forks to eat it.  Between each pair of plates is one fork.  The life of a philosopher consists 
of alternate periods of eating and thinking.  When a philosopher gets hungry, he tries to pick up 
his left fork and his right fork, one at a time, in any order.  No two philosophers may use the same 
fork at the same time.  If the philosopher successfully acquires two forks, he proceeds to eat for a 
while.  When he is done eating, he puts down his two forks and continues thinking.  
The only possible activities are eating and thinking.  
The problem: devise a solution (algorithm) that will allow the philosophers to eat.  The algorithm 
must satisfy mutual exclusion while avoiding deadlock and starvation.

For additional information about the problem statement, view the included PDF.

SOLUTION

To allow the philosophers to eat and thinking while avoiding deadlock and starvation, I imposed 
a total ordering on the forks (i.e. the semaphores). Starting with the fork to the left of Philosopher 0
and moving clockwise, each fork is labeled incrementally. The philosophers have a policy of always selecting
the fork with the lower label, so Philosopher 0 will try to pick up Fork 0 and then try to pick up Fork 1, 
Philosopher 1 will try to pick up Fork 1 and then try to pick up Fork 2, etc. Note: if there are N total
Philosophers then the last philosopher, Philosopher N-1, will try to pick up Fork 0 and then try to pick up
Fork N-1. This ordering ensures that at least one philosopher will be able to pick up two forks at once and eat,
thus ensuring that deadlock does not occur. After a philosopher eats they will think for some time, allowing
the neighboring philosophers to use the freed forks.

Note that there is one fork between every two philosophers, so for N>1 there are also N forks. However, for N=1
there are two forks.

For my implementation, I decided to use pthreads to spawn the philosophers and the semaphore.h library to provide
the locks on the forks. Each fork gets its own binary semaphore so that only one philosopher may use them at a time.

References:

I referenced the Chapter 6 class slides as my main reference. I also referenced the Wikipedia page on POSIX threads:
https://en.wikipedia.org/wiki/POSIX_Threads.


