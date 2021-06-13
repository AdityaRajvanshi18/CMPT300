To make part 2 of the assignment please follow the following steps:

1. type make
2. type gcc -g -pthread -Wall -o scheduler scheduler.o worker.o list.o smp5_tests.o testrunner.o -lrt

The above step is essential for the files to run. I'm not entirely sure why this works, but it will not compile without this.

