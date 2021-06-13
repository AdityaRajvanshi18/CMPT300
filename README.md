# CMPT300
**Operating Systems**

All the code in this class was written in C.

These were the following projects:

1a. Part 1 - Create,  debug and extend C programs that run within a shell environment and utilize basic I/O and string manipulation functions.
1b. Part 2 - Explore and extend a simple Unix shell interpreter and learn the basics of system calls for creating and managing processes.

2a.
2b.

3a. Part 1 - Write a user-mode thread scheduler, implementing both FIFO and round robin for use in a simple cooperative multi-threading system. Learn about implementing object-oriented constructs in low-level procedural languages like C.
3b. Part 2 - Simulate a time-sharing system by using signals and timers. The scheduler is installed with an interval timer. The timer starts ticking when the scheduler picks a thread to use the CPU which in turn signals the thread when its time slice is finished thus allowing the scheduler to pick another thread and so on. When a thread has completely finished its work it leaves the scheduler to allow a waiting thread to enter.

4. Part 2 - Implement routines for allocating and deallocating memory, and keeping track of
what memory is in use and implement four strategies for selecting in
which block to place a new requested memory black:

    1) First-fit: select the first suitable block with smallest address.
    2) Best-fit: select the smallest suitable block.
    3) Worst-fit: select the largest suitable block.
    4) Next-fit: select the first suitable block after
     the last block allocated (with wraparound
     from end to beginning).
