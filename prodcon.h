/*******************************************************************************
 * FILE:                prodcon.h
 * TITLE:               Header file for producer consumer application.
 * Date:                September 19, 2012.
 * Author:              Rohit Sinha
 * Email:				sinha049@umn.edu
 * Assignment:          CSCI 5103: Operating Systems - Assignment 1 Part B
 * Due Date:            September 21, 2012
 *
 * Problem Statement:   create shared memory between two processes  to act as a
 *                      buffer for data transfer. These two processes will
 *                      communicate in the producer-consumer mode.  The producer
 *                      process will read a file, transfer its contents to the
 *                      consumer through the shared memory, and the consumer
 *                      process will store the received data in a new file
 *                      called output.
********************************************************************************/


#ifndef _prodcon_h
#define _prodcon_h

/* Including Standard libraries */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>

/* Defining constants */
#define MAX_BUF_SIZE 1024
#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1
#define KEY_CODE 1234

/* Global Variables */
static volatile sig_atomic_t signalflag; // atmoic flag for synchronization
static sigset_t newmask, oldmask, nomask; // signal set variables



/* Structure of the shared memory */
struct shared_mem_struct
{
    char buffer [MAX_BUF_SIZE]; // buffer to interchage  data
    int count;                  // count for number of bytes
};

/* Defining prototypes */
static void signalHandler(int signo);
void initializeWait(void);
void signalParent(pid_t pid);
void waitParent(void);
void signalChild(pid_t pid);
void waitChild(void);

#endif
