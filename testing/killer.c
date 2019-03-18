/*
    Program to send signals to other processes
    
*/

#include <stdio.h>
#include <signal.h>
#include <unistd.h>

int main()
{
    pid_t victim;
    unsigned int remaining;
    
    while (1)
    {
        printf("Enter the pid of the process to kill: ");
        scanf("%d", &victim);
        
        if (victim > 0)
        {
            kill(victim, SIGTSTP);
        }
        else if (victim == -1)
        {
            //raise(SIGKILL);
            remaining = alarm(10);
            printf("Restarted timer with %d seconds remainig\n", remaining);
        }
    }
    
    return 0;
}