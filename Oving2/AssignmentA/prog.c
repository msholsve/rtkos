#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>

int globalVar = 0;

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        printf("Not enough arguments\n");
        exit(1);
    }

    pid_t childPid;
    int localVar = 0;
    int n = atoi(argv[1]);

    //childPid = fork();
    childPid = vfork();

    // fork was successful
    if (childPid >= 0)
    {
        char *name = (childPid == 0) ? "Child" : "Parent";

        for (int i = 0; i < n; i++)
        {
            globalVar++;
            localVar++;
            printf("%s: globalVar = %i, localVar = %i\n", name, globalVar, localVar);
        }
    }
    // fork failed
    else 
    {
        printf("\n Fork failed, quitting\n");
        exit(1);
    }

    exit(0);
}