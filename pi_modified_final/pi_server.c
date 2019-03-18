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
#include "get_pi.h"
#include "sockets.h"
#include "fatal_error.h"
//Signal library
#include <signal.h>
//poll library
#include <poll.h>


#define BUFFER_SIZE 1024
#define MAX_QUEUE 5

//Interruption detection global variable
int interrupted = 0;

///// FUNCTION DECLARATIONS
void usage(char * program);
void waitForConnections(int server_fd);
void attendRequest(int client_fd);
void blockCtrlC();
void setupHandlers();
void detectInterruption(int signal);

///// MAIN FUNCTION
int main(int argc, char * argv[])
{
    //Configuration for interruption handling
    setupHandlers();

    int server_fd;

    printf("\n=== SERVER FOR COMPUTING THE VALUE OF pi ===\n");

    // Check the correct arguments
    if (argc != 2)
    {
        usage(argv[0]);
    }

	// Show the IPs assigned to this computer
	printLocalIPs();

    // Start the server
    server_fd = initServer(argv[1], MAX_QUEUE);
	// Listen for connections from the clients
    waitForConnections(server_fd);
    // Close the socket
    close(server_fd);

    return 0;
}

///// FUNCTION DEFINITIONS

/*
    Explanation to the user of the parameters required to run the program
*/
void usage(char * program)
{
    printf("Usage:\n");
    printf("\t%s {port_number}\n", program);
    exit(EXIT_FAILURE);
}


/*
    Main loop to wait for incomming connections
*/
void waitForConnections(int server_fd)
{
    struct sockaddr_in client_address;
    socklen_t client_address_size;
    char client_presentation[INET_ADDRSTRLEN];
    int client_fd;
    pid_t new_pid;

    // Get the size of the structure to store client information
    client_address_size = sizeof client_address;

    while (1)
    {
        // ACCEPT
        // Wait for a client connection
        client_fd = accept(server_fd, (struct sockaddr *)&client_address, &client_address_size);
        if (client_fd == -1)
        {
            fatalError("accept");
        }
         
        // Get the data from the client
        inet_ntop(client_address.sin_family, &client_address.sin_addr, client_presentation, sizeof client_presentation);
        printf("Received incomming connection from %s on port %d\n", client_presentation, client_address.sin_port);

        // FORK
        // Create a new child process to deal with the client
        new_pid = fork();
        // Parent
        if (new_pid > 0)
        {
            // Close the new socket
            close(client_fd);
        }
        else if (new_pid == 0)
        {
            // Close the main server socket to avoid interfering with the parent
            close(server_fd);
            printf("Child process %d dealing with client\n", getpid());
            // Deal with the client
            attendRequest(client_fd);
            // Close the new socket
            close(client_fd);
            // Terminate the child process
            exit(EXIT_SUCCESS);
        }
        else
        {
            fatalError("fork");
        }

    }
}

/*
    Hear the request from the client and send an answer
*/
void attendRequest(int client_fd)
{
    char buffer[BUFFER_SIZE];
    unsigned long int iterations;
    double result;
    int iterationsSoFar = 0;

    //Polling variables
    struct pollfd test_fd;
    int poll_result;
    int timeout = 1;

    //Polling configuration
    test_fd.fd = client_fd;//setting client communication as the stream to monitor
    test_fd.events = POLLIN;//monitor if there's data to read

    // RECV
    // Receive the request
    recvString(client_fd, buffer, BUFFER_SIZE);
    sscanf(buffer, "%lu", &iterations);

    printf(" > Got request from client with iterations=%lu\n", iterations);

    // Compute the value of PI
    result = computePI(iterations, &interrupted, &iterationsSoFar, &poll_result, &test_fd, timeout, buffer, BUFFER_SIZE, &client_fd);

    printf(" > Sending PI=%.20lf\n", result);

    // Prepare the response to the client
    sprintf(buffer, "%.20lf", result);
    // SEND
    // Send the response
    sendString(client_fd, buffer);

    //Receive ACK
    recvString(client_fd, buffer, BUFFER_SIZE);

    //Send Number of iterations to client
    sprintf(buffer, "%d", iterationsSoFar);
    sendString(client_fd, buffer);
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
    interrupted = 1;
}