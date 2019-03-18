/*
Saúl Enrique Labra Cruz A01020725
Activity 6 - Blocking with signals
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <poll.h>
#include <errno.h>

#define MAX_NUMBERS 5

// Global variable to detect when a signal arrived
int interrupted = 0;

// Function declarations
void countInputs();
void setupHandlers();
void detectInterruption(int signal);

int main(int argc, char * argv[])
{
    setupHandlers();
    countInputs();

    return 0;
}

// Ask the user for a series of numbers and add them up
void countInputs()
{
    int total = 0;
    int number;

    // Variables for polling
    struct pollfd test_fd;
    int poll_result;
    int timeout = 1000;     // In milliseconds, equal to 1 second

    // Prepare the polling configuration
    test_fd.fd = 0;     // Polling stdin
    test_fd.events = POLLIN;

    for (int i=0; i<MAX_NUMBERS && !interrupted; i++)
    {
        printf("#%2d/%2d. Enter a number: ", i+1, MAX_NUMBERS);
        // Use fflush to ensure the message is printed immediately
        fflush(stdout);
        // Test whether to do the scanf before getting blocked
        while (1)
        {
            poll_result = poll(&test_fd, 1, timeout);
            // Timeout finished without incidents
            if (poll_result == 0)
            {
                printf(".");
                fflush(stdout);
            }
            // When the input is ready to be read, do the scanf and add the total
            else if (poll_result > 0)
            {
                if (test_fd.revents & POLLIN)
                {
                    scanf("%d", &number);
                    total += number;
                    break;
                }
            }
            // The poll returned an error
            else // if (poll_result == -1)
            {
                // If poll detected an error due to an interruption, exit the loop
                if (errno == EINTR)
                {
                    break;
                }
            }
        }
    }

    // Show the final total
    printf("\nThe total is: %d\n", total);
}
//
void setupHandlers()
{
    struct sigaction new_action;
    
    // Configure the action to take
    // Block all signals during the time the handler funciton is running
    sigfillset(&new_action.sa_mask);
    new_action.sa_handler = detectInterruption;
    // Clear the flags
    new_action.sa_flags = 0;

    // Set the handler
    sigaction(SIGINT, &new_action, NULL);
}

// Signal handler
void detectInterruption(int signal)
{
    // Change the global variable
    interrupted = 1;
}
