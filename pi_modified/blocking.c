/*
    Program to test the blocking of signals
    It can take a number of seconds to live as an argument

    Gilberto Echeverria
    gilecheverria@yahoo.com
    28/02/2017
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include "fatal_error.h"

#define LIFETIME 20

// Function declarations
sigset_t setupMask();
void unsetMask(sigset_t old_set);
void detectBlocked(int signal);
void waitLoop();

int main(int argc, char * argv[])
{
    int seconds;
    sigset_t old_set;

    // Use a timeout obtained as an argument
    if (argc >= 2)
        seconds = atoi(argv[1]);
    // Otherwise use a default time
    else
        seconds = LIFETIME;

    // Change the signal handlers
    old_set = setupMask();

    // Wait a few seconds before quiting on its own
    waitLoop(seconds);

    detectBlocked(SIGINT);
    detectBlocked(SIGUSR1);

    // Remove the masks
    unsetMask(old_set);

    // Wait a few seconds before quiting on its own
    waitLoop(seconds);

    return 0;
}

// Function definitions

// Modify the signal mask
sigset_t setupMask()
{
    sigset_t blocked_set;
    sigset_t old_set;

    printf("Setting a mask to block Ctlr-C\n");

    // Clear the set from any signal
    sigemptyset(&blocked_set);
    // Add the signals to block
    if (sigaddset(&blocked_set, SIGINT) == -1)      // Ctrl-C
    {
        perror("sigaddset");
    }
    sigaddset(&blocked_set, SIGTSTP);   // Ctrl-Z
 
    // Block the signals indicated in the mask
    if (sigprocmask(SIG_BLOCK, &blocked_set, &old_set) == -1)
    {
        perror("sigprocmask");
    }
    
    // Check if the set does contain SIGINT
    if (sigismember(&blocked_set, SIGINT))
    {
        printf("SIGINT is in the new set to block\n");
    }
    else
    {
        printf("SIGINT is NOT in the new set to block\n");
    }
    
    // Check if the old set does contain SIGTSTP
    if (sigismember(&old_set, SIGTSTP))
    {
        printf("SIGTSTP is in the old set to block\n");
    }
    else
    {
        printf("SIGTSTP is NOT in the old set to block\n");
    }

    return old_set;
}

// Reset the blocking mask to the previous one
void unsetMask(sigset_t old_set)
{
    //sigset_t previous_set;
    printf("Resetting the old mask\n");

    // Restore the previous set of blocked signals
    if (sigprocmask(SIG_SETMASK, &old_set, NULL) == -1)
    {
        perror("sigprocmask");
    }
}

void detectBlocked(int signal)
{
    sigset_t pending_set;

    // Detect pending signals
    sigpending(&pending_set);
    
    // Check if the signal indicated is in the set
    if (sigismember(&pending_set, signal))
    {
        printf("Signal %d received while blocked\n", signal);
    }
}

// Wait a few seconds until the program ends
void waitLoop(int seconds)
{
    int i;

    // Show the PID so that it can be sent signals form the terminal
    //  or other process
    printf("My PID is: %d\n", getpid());

    printf("Will finish on my own in %3d s", seconds);
    fflush(stdout);
    // Loop to wait for something to happen
    for (i=seconds-1; i>=0; i--)
    {
        sleep(1);
        printf("\b\b\b\b\b%3d s", i);
        fflush(stdout);
    }
    printf("\nTime out. Good bye\n");
}
