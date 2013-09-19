SharedMemoryProducerConsumer
============================

prodcon:
---------
Producer Consumer Implementation via shared memory.

Problem Statement:
------------------
create shared memory between two processes  to act as a  buffer for data transfer. These two processes will communicate in the producer-consumer mode.  The producer process will read a file, transfer its contents to the consumer through the shared memory, and the consumer process will store the received data in a new file called output.

Files
-----
Filename | Description 
----------|------------
/prodcon.c:| the main file
/utilfunc.c:| file containing function definitions 
/prodcon.h:| the header file
/makefile:| the make file
/input.txt:| the input file
/Readme.txt:| the README file

How to compile and run
-----------------------
1. execute "make" on terminal when in directory containing these files
2. ./prodcon [inputFilename]


Contact Information
--------------------
Name: Rohit Sinha

Email id: sinha049@umn.edu
