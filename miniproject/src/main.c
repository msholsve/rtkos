
/**********************************************************
    Includes
**********************************************************/

#include <stdio.h>
#include <pthread.h> 
#include <stdlib.h>

#include "miniproject.h"
#include "udp.h"
#include "controller.h"
#include "signaler.h"

/**********************************************************
    Defines
**********************************************************/

#define NUM_THREADS 3

typedef void* (*taskFunc_t)(void*);

/**********************************************************
    Main
**********************************************************/

int main(int argc, char *argv[])
{
    printf("Start Client\n");

    char *ip = "127.0.0.1";
    if (argc > 1)
        ip = argv[1];

    int do_sig = 0;
    if (argc > 2)
        do_sig = atoi(argv[2]);

    // Init modules
    udpInit(ip, do_sig);
    signalerInit();
    controllerInit();

    // Tasks with corresponding functions
    pthread_t tasks[NUM_THREADS];
    taskFunc_t taskFunctions[NUM_THREADS] = { udpTaskFunction, controllerTaskFunction, signalerTaskFunction };

    for (int i = 0; i < NUM_THREADS - (1 ^ do_sig); ++i)    
        pthread_create(&tasks[i], NULL, taskFunctions[i], NULL);

    // Only wait for controller, NB task id
    pthread_join(tasks[1], NULL);

    // Cleanup modules
    controllerCleanup();
    signalerCleanup();
    udpCleanup();

	return 0;
}
