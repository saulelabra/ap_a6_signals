/*
Saúl Enrique Labra Cruz A01020725
Activity 6 - Blocking with signals
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
// Sockets libraries
#include <netdb.h>
#include <arpa/inet.h>
// Custom libraries
#include "sockets.h"
#include "fatal_error.h"
//signal libraries
#include <signal.h>
//poll library
#include "poll.h"
#include "errno.h"


#define BUFFER_SIZE 1024

int interrupted = 0;

///// FUNCTION DECLARATIONS
void usage(char * program);
void requestPI(int connection_fd);
void setupHandlers();
void detectInterruption(int signal);

///// MAIN FUNCTION
int main(int argc, char * argv[])
{
    //Configuration for interruptions handling
    setupHandlers();

    int connection_fd;

    printf("\n=== CLIENT FOR COMPUTING THE VALUE OF pi ===\n");

    // Check the correct arguments
    if (argc != 3)
    {
        usage(argv[0]);
    }

    // Connect to the server
    connection_fd = connectSocket(argv[1], argv[2]);
	// Request PI
    requestPI(connection_fd);
    // Close the socket
    close(connection_fd);

    return 0;
}

///// FUNCTION DEFINITIONS

/*
    Explanation to the user of the parameters required to run the program
*/
void usage(char * program)
{
    printf("Usage:\n");
    printf("\t%s {server_address} {port_number}\n", program);
    exit(EXIT_FAILURE);
}


void requestPI(int connection_fd)
{
    // Variables for polling
    struct pollfd test_fd;
    int poll_result;
    int timeout = 1000;

    // Prepare the polling configuration
    test_fd.fd = connection_fd;     // Polling server connection_fd
    test_fd.events = POLLIN;

    char buffer[BUFFER_SIZE];
    unsigned long int iterations;
    int iterationsSoFar = 0;
    double result;

    printf("Enter the number of iterations for PI: ");
    scanf("%lu", &iterations);

    // Prepare the request to the server
    sprintf(buffer, "%lu", iterations);
    // SEND
    // Send the response
    sendString(connection_fd, buffer);

    //While server doesn't finish computing
    while(1)
    {
        //printf("Enters loop\n");
        poll_result = poll(&test_fd, 1, timeout);

        if(poll_result == 0)//Timeout finished without incidents
        {
            if(interrupted) //If user sends CTRL-C, client program notifies server to stop
            {
                printf("Before sending stop message\n");
                fflush(stdout);

                /*sprintf(buffer, "STOP");
                sendString(connection_fd, buffer);*/

                printf("Sent Stop message\n");
                fflush(stdout);
            }else{
                printf(".");
                fflush(stdout);
            }
        }else{
            if(poll_result > 0) //Server replied with results
            {
                if (test_fd.revents & POLLIN)
                {
                    // RECV
                    // Receive the response
                    recvString(connection_fd, buffer, BUFFER_SIZE);
                    sscanf(buffer, "%lf", &result);

                    printf("Received pi value\n");
                    fflush(stdout);

                    //Send ACK
                    sprintf(buffer, "ACK");
                    sendString(connection_fd, buffer);

                    //Receive Iterations so Far
                    recvString(connection_fd, buffer, BUFFER_SIZE);
                    sscanf(buffer, "%d", &iterationsSoFar);

                    printf("Received number of iterations\n");
                    fflush(stdout);

                    break;
                }
            }else{
                if (errno == EINTR)
                {
                    printf("Error\n");
                    break;
                }
            }
        }

    }

    // Print the result
    printf("The value for PI is: %.20lf\n", result);
    printf("Iterations computed: %d\n", iterationsSoFar);
}

void setupHandlers() {
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
    printf("Detected CTRL-C\n");
    interrupted = 1;

    printf("Changed var interrupted to 1\n");
}
