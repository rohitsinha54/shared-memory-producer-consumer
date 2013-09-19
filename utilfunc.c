/*******************************************************************************
 * FILE:                utilfunc.c
 * TITLE:               Utility Functions for Prodcuer Consumer implementation
 *                      with shared memory.
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



/* Including header files */
#include "prodcon.h"




/*******************************************************************************
 * Name of Function:    signalHandler
 * Purpose:             Handler for SIGUSR1 and SIGUSR2 signals.
 * Parameters:          signo - Signal number
 * Return Value:        void
 * Calls to:            None
 * Called from:         main
 * Method:              1. Sets signal flag.
 ******************************************************************************/
static void signalHandler(int signo)
{
	signalflag = 1; // Set the signal flag
}



/*******************************************************************************
 * Name of Function:    initializeWait
 * Purpose:             Initializes for signalParent, signalChild, waitParent
 *                      and waitChild.
 * Parameters:          void
 * Return Value:        void
 * Calls to:            none
 * Called from:         main
 * Method:              1. Links the signal handler function for SIGUSR1 and
 *                      SIGUSR2
 *                      2. Empties two signal masks
 *                      3. Add SIGUSR1 and SIGUSR2 to mask sets
 *                      4. Block SIGUSR1 and SIGUSR2 signals and saves current
 *                      signal mask.
 ******************************************************************************/
void initializeWait(void)
{
	/* Link handler functions */
    if (signal(SIGUSR1, signalHandler) == SIG_ERR)
    {
		perror("signal");
        exit(EXIT_FAILURE);
    }
	if (signal(SIGUSR2, signalHandler) == SIG_ERR)
    {
		perror("signal");
        exit(EXIT_FAILURE);
    }
    
    /* Clear masks */
	sigemptyset(&nomask);
	sigemptyset(&newmask);
    
    /* Add signals to mask sets */
	sigaddset(&newmask, SIGUSR1);
	sigaddset(&newmask, SIGUSR2);
    
	/* Block SIGUSR1 and SIGUSR2, and save current signal mask. */
	if (sigprocmask(SIG_BLOCK, &newmask, &oldmask) < 0)
    {
		perror("sigprocmask");
        exit(EXIT_FAILURE);
    }
}



/*******************************************************************************
 * Name of Function:    signalParent
 * Purpose:             Send SIGUSR2 signal to parent function(producer).
 * Parameters:          pid - pid of the parent function
 * Return Value:        void
 * Calls to:            none
 * Called from:         main
 * Method:              1. Sends SIGUSR2 signal to process given by pid.
 ******************************************************************************/
void signalParent(pid_t pid)
{
	kill(pid, SIGUSR2); // Child tells parent that it is done
}




/*******************************************************************************
 * Name of Function:    waitParent
 * Purpose:             to make child process wait for parent process.
 * Parameters:          void
 * Return Value:        void
 * Calls to:            none
 * Called from:         main
 * Method:              1. Waits for parent till SIGUSR1 handler sets the
 *                      signalflag
 *                      2. Clear the signalflag
 *                      3. Restore the signal mask to old value
 ******************************************************************************/
void waitParent(void)
{
	while (signalflag == 0)
    {
		sigsuspend(&nomask);    // Wait for signal from parent
    }
    
	signalflag = 0; // Clear the singalflag
    
	/* Restore the signal mask to old value */
	if (sigprocmask(SIG_SETMASK, &oldmask, NULL) < 0)
    {
		perror("sigprocmask");
        
    }
}



/*******************************************************************************
 * Name of Function:    signalChild
 * Purpose:             Send SIGUSR1 signal to child function (consumer).
 * Parameters:          pid - pid of the child function
 * Return Value:        void
 * Calls to:            none
 * Called from:         main
 * Method:              1. Sends SIGUSR1 signal to process given by pid.
 ******************************************************************************/
void signalChild(pid_t pid)
{
	kill(pid, SIGUSR1); // Parent tells child that it is done
}



/*******************************************************************************
 * Name of Function:    waitChild
 * Purpose:             to make parent process wait for child process.
 * Parameters:          void
 * Return Value:        void
 * Calls to:            none
 * Called from:         main
 * Method:              1. Waits for parent till SIGUSR2 handler sets the
 *                      signalflag
 *                      2. Clear the signalflag
 *                      3. Restore the signal mask to old value
 ******************************************************************************/
void waitChild(void)
{
	while (signalflag == 0)
    {
		sigsuspend(&nomask);    // wait for signal from child
    }
    
	signalflag = 0; // clear the signalflag
    
	/* Restore the signal mask to old value */
	if (sigprocmask(SIG_SETMASK, &oldmask, NULL) < 0)
    {
        perror("sigprocmask");
    }
}