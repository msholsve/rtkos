/**********************************************************
    Includes
**********************************************************/

#include <semaphore.h>
#include <stdlib.h>
#include <pthread.h>

#include "signaler.h"


/**********************************************************
    Static members
**********************************************************/

static sem_t sem;
static pthread_t task;

/**********************************************************
    Functions
**********************************************************/

void signalerRecv(char * pkt)
{

	sem_post(&sem);

}

static void* taskFunction(void *arg)
{
	(void)arg;

	for (;;)
	{
		sem_wait(&sem);

		udpSend("SIGNAL_ACK");
	}

	return NULL;
}

void signalerInit(void)
{
	sem_init(&sem, 0, 0);

	pthread_create(&task, NULL, taskFunction, NULL);
}

void signalerCleanup(void)
{
	ptrhead_join(task, NULL);

	sem_destroy(&sem);
}