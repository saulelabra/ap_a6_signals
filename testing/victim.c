/*
    Program to test signals on. It declares its PID and waits for a signal
    It can take a number of seconds to live as an argument

    Gilberto Echeverria
    gilecheverria@yahoo.com
    28/02/2017
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define LIFETIME 20

int main(int argc, char * argv[])
{
    int i;
    int remaining;

    // Use a timeout obtained as an argument
    if (argc >= 2)
        remaining = atoi(argv[1]);
    // Otherwise use a default time
    else
        remaining = LIFETIME;

    // Show the PID so that it can be sent signals form the terminal
    //  or other process
    printf("My PID is: %d\n", getpid());

    printf("Will finish on my own in %3d s", remaining);
    fflush(stdout);
    // Loop to wait for something to happen
    for (i=remaining-1; i>=0; i--)
    {
        sleep(1);
        printf("\b\b\b\b\b%3d s", i);
        fflush(stdout);
    }
    printf("\nTime out. Good bye\n");

    return 0;
}
