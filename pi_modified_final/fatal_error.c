/*
Saúl Enrique Labra Cruz A01020725
Activity 6 - Blocking with signals
*/

#include "fatal_error.h"

void fatalError(const char * message)
{
    perror(message);
    exit(EXIT_FAILURE);
}
