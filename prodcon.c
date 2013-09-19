/*******************************************************************************
 * FILE:                prodcon.c
 * TITLE:               Prodcuer Consumer implementation with shared memory.
 * Date:                September 19, 2012.
 * Author:              Rohit Sinha
 * Email:               sinha049@umn.edu
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
 * Name of Function:    main
 * Purpose:             Provides shared memory to transfar data between producer
 *                      and consumer process in a synchronised manner.
 * Parameters:          Command line arguments (takes input file name)
 * Return Value:        int
 * Calls to:            initializeWait, waitParent, waitChild, signalParent, 
 *                      signalChild
 * Called from:         --
 * Method:              1. Check validity of command line arguments
 *                      2. Calls intializeWait and blocks SIGUSR1 and SIGUSR2
 *                      3. forks a process
 *                      In parent process
 *                          a. opens input file for reading
 *                          b. creates shared memory segment
 *                          c. Attaches shared memory segment
 *                          d. typecasts it to structure
 *                          e. set the count to 0
 *                          f. reads from input file till EOF
 *                          g. writes to shared mem buffer and set count
 *                          h. at the end of file set count to -1
 *                          i. wait for child to terminate
 *                          j. close the input file
 *                          k. detach the shared memory
 *                          l. remove the shared memory
 *                      In child process:
 *                          a. opens output file for writing
 *                          b. creates shared memory segment
 *                          c. Attaches shared memory segment
 *                          d. typecasts it to structure 
 *                          e. reads from shared memory till EOF (count = -1)
 *                          f. writes to output file 
 *                          g. Close the output file
 *                          h. detach the shared memory
 *                          i. child process terminates itself
 *                      4. exits with success status
 ******************************************************************************/
int main(int argc, char * argv[])
{ // starting of main function
    
    /* Declaring Variables */
    pid_t	pid;    
    
    FILE *input_file, *output_file;
    char file_buffer[MAX_BUF_SIZE];
    
    int smem_id;
    key_t smem_key = KEY_CODE;
    void *temp_ptr = (void *)0;
    struct shared_mem_struct *shared_mem;
    
    /* Check the passed command line argument */
    if(argc == 1)
    {
        printf("Error: Please provide input file name\n");
        exit(EXIT_FAILURE);
    }
    if(argc != 2)
    {
        printf("Error: Incorrect number of arguments passed\n");
        exit(EXIT_FAILURE);
    }
    
    /* If command line arguments are correct then go ahead */
    
    initializeWait();   // Call initializeWait to block SIGUSR1 and SIGUSR2
    
    /* fork a process */
	if ((pid = fork()) < 0) 
	{ // if fork fails
		perror("fork");
		exit(EXIT_FAILURE);
	}
    /* Parent Process */
	else if (pid != 0)
	{   // starting of else if (pid != 0)
        
        /* Try opening input file whose name is passed from command line */
		if((input_file = fopen(argv[1],"r")) == NULL)
		{   // if fopen fails
		    perror("fopen");
		    exit(EXIT_FAILURE);
		}
		
		/* Creating shared memory segment */
	    if((smem_id = shmget(smem_key, sizeof(struct shared_mem_struct), IPC_CREAT | 0666))<0)
	    {   // if shmget fails
	        perror("shmget");
	        exit(EXIT_FAILURE);
	    }
	    
	    /* Attaching the shared memory to data space */
	    if((temp_ptr = shmat(smem_id, (void *)0, 0)) == (void *)-1)
	    {   // if shmat fails
	        perror("shmat");
	        exit(EXIT_FAILURE);
	    }
        
        /* Typecast the shared memory block to our structure */
	    shared_mem = (struct shared_mem_struct *) temp_ptr;
        
	    shared_mem->count = 0;    // Set the counter to 0
        
        /* Do this till the EOF for input file */
	    while(fgets(file_buffer, (MAX_BUF_SIZE-1), input_file) != NULL)
	    {
            /* Wait for child to read everything from shared memory */
	        while(shared_mem->count != 0)
	        {
	            waitChild();
	        }
            
	        /* Copy read data from the buffer to shared memory */
	        strcpy(shared_mem->buffer,file_buffer);
            /* Set the count to bytes copied to shared memory */
	        shared_mem->count = strlen(shared_mem->buffer);
            
            /* signal child process to read from shared memory */
	        signalChild(pid);
            /* again set masks to block SIGUSR1 and SIGUSR2 */
	        initializeWait();
	    }
        /* Block SIGUSR1 and SIGUSR 2 */
	    initializeWait();
        /* Wait for child to read everything from shared memory */
	    while(shared_mem->count != 0)
	    {
	        waitChild();
	    }
        /* Set the count to -1 to show EOF for input file */
	    shared_mem->count = -1;
        /* Signal child to read */
	    signalChild(pid);
	    /* Wait for child to terminate */
	    waitpid(pid, NULL, 0);
        
        /* Close the input file */
        fclose(input_file);
        
        /* deatach the shared memory */
        if(shmdt(shared_mem) == -1)
        {
            perror("shmdt");
            exit(EXIT_FAILURE);
        }
        
        /* Remove the deatached shared memory */
        if(shmctl(smem_id, IPC_RMID, 0) == -1)
        {
            perror("shmctl");
            exit(EXIT_FAILURE);
        }
        
        /* Display success message on console */
        printf("\nSuccess: The input file provided by you has been successfully copied via shared memory to output file named \"ouput.txt\" in the current working directory.\n\n");
    }   // end of else if (pid != 0)
    
    /* Child Process */
	else 
    {   // starting of else
        
        /* Opening output file in write mode */
        if((output_file = fopen("output.txt","w")) == NULL)
        {   //if fopen fails
            perror("fopen");
            exit(EXIT_FAILURE);
        }
        /* Creating shared memory segment */
        if((smem_id = shmget(smem_key, sizeof(struct shared_mem_struct), IPC_CREAT | 0666))<0)
        {   // if shmget fails
            perror("shmget");
            exit(EXIT_FAILURE);
        }
        
        /* Attaching the shared memory to data space */
        if((temp_ptr = shmat(smem_id, (void *)0, 0)) == (void *)-1)
        {   // if shmat fails
            perror("shmat");
            exit(EXIT_FAILURE);
        }
        
        /* Typecast the shared memory block to our structure */
        shared_mem = (struct shared_mem_struct *) temp_ptr;
        
        /* do this till count is -1 i.e. eof of file is passed
         by parent process */
        while(shared_mem->count != -1)
        {
            /* Wait for parent parent process to write something */
            while(shared_mem->count == 0)
            {
                waitParent();
            }
            
            /* If written data by parent is not EOF */
            if(shared_mem->count != -1)
            {
                /* Copy the text written on shared memory buffer to ouput file */
                fputs(shared_mem->buffer, output_file);
                shared_mem->count = 0;    // set the counter back to 0
                signalParent(getppid());    // signal parent read is done
            }
        }
        fclose(output_file);    // Close the output file
        
        /* deatach the shared memory */
        if(shmdt(shared_mem) == -1)
        {
            perror("shmdt");
            exit(EXIT_FAILURE);
        }
        
        kill(getpid(), SIGTERM);    // Kill the child process

	} // end of else
    
	exit(EXIT_SUCCESS); // Exit successfully
    
}   // end of main
