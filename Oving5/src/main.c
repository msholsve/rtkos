
/**********************************************************
	Includes
**********************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "io.h"
#include "util.h"

/**********************************************************
	Defines
**********************************************************/

#define ASSIGNMENT_A 0
#define ASSIGNMENT_B 1
#define ASSIGNMENT_C 2

#define N_THREADS 3
#define N_DUMMY   10

/**********************************************************
	Assignment A
**********************************************************/

void* responseHandler(void *arg)
{
	int channel = *(int *)arg;

	testPin(channel);

	return NULL;
}

void assignmentA()
{
	printf("Starting Assignment A!\n");

	pthread_t ts[N_THREADS];
	int channels[N_THREADS] = { CHANNEL_A, CHANNEL_B, CHANNEL_C };

	for (int i = 0; i < N_THREADS; i++)
	{
		pthread_create(&ts[i], 0, responseHandler, &channels[i]);
	}

	for (int i = 0; i < N_THREADS; i++)
	{
		pthread_join(ts[i], 0);
	}	
}

/**********************************************************
	Assignment B
**********************************************************/

void* responseHandlerOneCore(void *arg)
{
	int channel = *(int *)arg;
	
	setCpu(1);

	testPin(channel);

	return NULL;
}

void* disturbanceInTheForce(void *arg)
{
	(void)arg;

	setCpu(1);

	int volatile dummy = 0;
	while (1)
	{
		dummy++;
	}
}

void assignmentB()
{
	printf("Starting Assignment B!\n");
	
	pthread_t ts[N_THREADS + N_DUMMY];
	int channels[N_THREADS] = { CHANNEL_A, CHANNEL_B, CHANNEL_C };

	for (int i = 0; i < N_THREADS; i++)
	{
		pthread_create(&ts[i], 0, responseHandlerOneCore, &channels[i]);
	}

	for (int i = N_THREADS; i < N_THREADS + N_DUMMY; i++)
	{
		pthread_create(&ts[i], 0, disturbanceInTheForce, NULL);
	}

	for (int i = 0; i < N_THREADS + N_DUMMY; i++)
	{
		pthread_join(ts[i], 0);
	}	
}

/**********************************************************
	Assignment C
**********************************************************/

void* responseHandlerPeriodic(void *arg)
{
	int channel = *(int *)arg;

	setCpu(1);

	struct timespec next;

	io_write(channel, HIGH);

	while (1)
	{
		clock_gettime(CLOCK_REALTIME, &next);
		timespec_add_us(&next, 1000);

		clock_nanosleep(CLOCK_REALTIME, TIMER_ABSTIME, &next, NULL);

		if (!io_read(channel))
		{
			doResponse(channel);
		}
	}

	return NULL;
}

void assignmentC()
{
	printf("Starting Assignment C!\n");
	
	pthread_t ts[N_THREADS];
	int channels[N_THREADS] = { CHANNEL_A, CHANNEL_B, CHANNEL_C };

	for (int i = 0; i < N_THREADS; i++)
	{
		pthread_create(&ts[i], 0, responseHandlerPeriodic, &channels[i]);
	}

	for (int i = N_THREADS; i < N_THREADS + N_DUMMY; i++)
	{
		pthread_create(&ts[i], 0, disturbanceInTheForce, NULL);
	}

	for (int i = 0; i < N_THREADS + N_DUMMY; i++)
	{
		pthread_join(ts[i], 0);
	}	
}

/**********************************************************
	Main
**********************************************************/

int main(int argc, char *argv[])
{
	if (io_init() != 1)
	{
		printf("IO init failed, exiting\n");
		exit(1);
	}

	char assignment = 'A';

	if (argc > 1)
	{
		assignment = *argv[1];
	}

	switch (assignment)
	{
	case 'A': assignmentA(); break;
	case 'B': assignmentB(); break;
	case 'C': assignmentC(); break;
	default: printf("Nein Nein Nein Nein Nein Nein Nein!\n"); break; 
	}

	return 0;
}