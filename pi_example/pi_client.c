/*
    Client program to get the value of PI
    This program connects to the server using sockets

    Gilberto Echeverria
    gilecheverria@yahoo.com
    21/10/2018
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


#define BUFFER_SIZE 1024

///// FUNCTION DECLARATIONS
void usage(char * program);
void requestPI(int connection_fd);

///// MAIN FUNCTION
int main(int argc, char * argv[])
{
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
    char buffer[BUFFER_SIZE];
    unsigned long int iterations;
    double result;

    printf("Enter the number of iterations for PI: ");
    scanf("%lu", &iterations);

    // Prepare the response to the server
    sprintf(buffer, "%lu", iterations);
    // SEND
    // Send the response
    sendString(connection_fd, buffer);

    // RECV
    // Receive the request
    recvString(connection_fd, buffer, BUFFER_SIZE);
    sscanf(buffer, "%lf", &result);

    // Print the result
    printf("The value for PI is: %.20lf\n", result);
}
