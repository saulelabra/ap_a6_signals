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
#include <sys/signalfd.h>
//poll library
#include "poll.h"
#include "errno.h"

#define handle_error(msg) \
           do { perror(msg); exit(EXIT_FAILURE); } while (0)

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
    //Signal handling
    sigset_t mask;
    int sfd;
    struct signalfd_siginfo fdsi;
    ssize_t s;

    //Blocking Ctrl-C signal
    sigemptyset(&mask);
    sigaddset(&mask, SIGINT);

    if (sigprocmask(SIG_BLOCK, &mask, NULL) == -1)
        handle_error("sigprocmask");

    sfd = signalfd(-1, &mask, 0);
    if (sfd == -1)
        handle_error("signalfd");

    // Variables for polling server connection socket
    struct pollfd test_fd;
    int poll_result;
    int timeout = 1000;

    //Variables for polling signals
    struct pollfd testsig_fd;
    int poll_result_sig;
    int timeoutSig = 1;

    // Prepare the polling configuration
    test_fd.fd = connection_fd;     // Polling server connection_fd
    test_fd.events = POLLIN;

    //Prepare polling configuration for signals
    testsig_fd.fd = sfd;
    testsig_fd.events = POLLIN;

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
        poll_result = poll(&test_fd, 1, timeout);
        poll_result_sig = poll(&testsig_fd, 1, timeoutSig);

        if(poll_result == 0)//Server hasn't sent results
        {
            if(poll_result_sig == 0)//User didn't press Ctrl-C
            {
                printf(".");
                fflush(stdout);
            }else{
                if(poll_result_sig > 0)//User pressed Ctrl-C
                {
                    s = read(sfd, &fdsi, sizeof(struct signalfd_siginfo));
                    if (s != sizeof(struct signalfd_siginfo))
                        handle_error("read");

                    if (fdsi.ssi_signo == SIGINT) {

                        sprintf(buffer, "STOP");
                        sendString(connection_fd, buffer);
                    }
                }                
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

                    //Send ACK
                    sprintf(buffer, "ACK");
                    sendString(connection_fd, buffer);

                    //Receive Iterations so Far
                    recvString(connection_fd, buffer, BUFFER_SIZE);
                    sscanf(buffer, "%d", &iterationsSoFar);


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

// Signal handler
void detectInterruption(int signal)
{
    // Change the global variable
    printf("Detected CTRL-C\n");
    interrupted = 1;

    printf("Changed var interrupted to 1\n");
}
