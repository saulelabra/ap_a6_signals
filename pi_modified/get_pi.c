/*
	Program to approximate pi by using the infinite series
		pi = 4 - (4/3) + (4/5) - (4/7) + (4/9) - ...
	
	Based on a program proposed in:
		http://www.dreamincode.net/forums/topic/99268-calculate-the-value-of-pi-using-infinite-series/

	Gilberto Echeverria
	gilecheverria@yahoo.com
	20/02/2018
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

        printf("Interrupted: %d\n", *interrupted);

        delay(1000000);
    }

    printf("Number of iterations computed: %ld\n", counter);
    *iterationsSoFar = counter;

    return result;
}
