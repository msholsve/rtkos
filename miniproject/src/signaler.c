/**********************************************************
    Includes
**********************************************************/

#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>

#include "signaler.h"
#include "udp.h"    

/**********************************************************
    Static members
**********************************************************/

// Used by UDP module
static sem_t notify_sem;

/**********************************************************
    Functions
**********************************************************/

void signalerNotify(void)
{
	sem_post(&notify_sem);
}

// Thread function
void* signalerTaskFunction(void *arg)
{
	(void)arg;

	for (;;)
	{
		// Wait for notification from UDP
		sem_wait(&notify_sem);

		udpSend("SIGNAL_ACK");
	}

	return NULL;
}

void signalerInit(void)
{
	sem_init(&notify_sem, 0, 0);
}

void signalerCleanup(void)
{
	sem_destroy(&notify_sem);
}
