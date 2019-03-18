/*
Saúl Enrique Labra Cruz A01020725
Activity 6 - Blocking with signals
*/

#include "get_pi.h"

void delay(int milliseconds) {  
    // Storing start time 
    clock_t start_time = clock(); 
  
    // looping till required time is not acheived 
    while (clock() < start_time + milliseconds) 
        ; 
}

double computePI(unsigned long int iterations, int * interrupted, int * iterationsSoFar, int * poll_result, struct pollfd * test_fd, int timeout, char * buffer, int BUFFER_SIZE, int * client_fd)
{
    double result = 4;
    int sign = -1;
    unsigned long int divisor = 3;
    unsigned long int counter = 0;

    //Also checks if there isn't an interruption
    for (counter = 0; counter<iterations && !*interrupted; counter++)
    {
        *poll_result = poll(test_fd, 1, timeout);

        if(*poll_result > 0) //There is data to be read
        {
            if(test_fd->revents & POLLIN)
            {
                *interrupted = 1;
                recvString(*client_fd, buffer, BUFFER_SIZE);

                printf("Computation stopped by client: %s\n", buffer);
            }
        }else{
            if(*poll_result < 0)
            {
                if(errno = EINTR)
                {
                    break;
                }
            }
        }

        result += sign * (4.0/divisor);
        sign *= -1;
        divisor += 2;
    }

    printf("Number of iterations computed: %ld\n", counter);
    *iterationsSoFar = counter;

    return result;
}
