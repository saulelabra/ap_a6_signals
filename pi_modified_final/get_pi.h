/*
	Program to approximate pi by using the infinite series
		pi = 4 - (4/3) + (4/5) - (4/7) + (4/9) - ...
	
	Based on a program proposed in:
		http://www.dreamincode.net/forums/topic/99268-calculate-the-value-of-pi-using-infinite-series/

	Gilberto Echeverria
	gilecheverria@yahoo.com
	20/02/2018
*/

#ifndef GET_PI_H
#define GET_PI_H

#include "time.h"
#include "stdio.h"
#include "poll.h"
#include "errno.h"
#include "sockets.h"

double computePI(unsigned long int iterations, int * interrupted, int * iterationsSoFar, int * poll_result, struct pollfd * test_fd, int timeout, char * buffer, int BUFFER_SIZE, int * client_fd);

#endif  /* not GET_PI_H */
