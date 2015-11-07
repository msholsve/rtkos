
/**********************************************************
    Includes
**********************************************************/

#include <stdio.h>

#include "miniproject.h"
#include "udp.h"
#include "controller.h"
#include "signaler.h"

/**********************************************************
    Defines
**********************************************************/

#define NUM_THREADS 3

typedef void (*initFunc_t)(void);
typedef void (*cleanupFunc_t)(void);

/**********************************************************
    Main
**********************************************************/

int main()
{
    printf("Start Client\n");

    initFunc_t initFuncs[NUM_THREADS] = { udpInit, signalerInit, controllerInit };

    cleanupFunc_t cleanupFuncs[NUM_THREADS] = { controllerCleanup, signalerCleanup, udpCleanup };

    for (int i = 0; i < NUM_THREADS; ++i)
        initFuncs[i]();

    for (int i = 0; i < NUM_THREADS; ++i)
        cleanupFuncs[i]();

	return 0;
}
