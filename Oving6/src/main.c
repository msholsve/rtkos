
/**********************************************************
	Includes
**********************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include <sys/mman.h>
#include <native/task.h>
#include <native/timer.h>

#include "io.h"
#include "util.h"

/**********************************************************
	Defines
**********************************************************/

#define N_RT 3

/**********************************************************
	Globals
**********************************************************/

int period_ns = 0;

/**********************************************************
	Assignment A
**********************************************************/

void* disturbanceInTheForce(void *arg)
{
	(void)arg;

	int volatile dummy = 0;

	while (1)
	{
		dummy++;
	}

	return NULL;
}

void responseHandlerPeriodic(void *arg)
{
	int channel = *(int *)arg;

	rt_task_set_periodic(NULL, TM_NOW, period_ns);

	while (1)
	{
		rt_task_wait_period(NULL);

		testPin(channel);
	}
}

void assignmentA(int period_us, int ndisturbance)
{
	printf("Starting Assignment A!\n");
	
	period_ns = period_us * 1000;

	RT_TASK rt[N_RT];
	pthread_t ts[ndisturbance];
	char *names[N_RT] = { "thread_a", "thread_b", "thread_c" };
	int channels[N_RT] = { CHANNEL_A, CHANNEL_B, CHANNEL_C };

	mlockall(MCL_CURRENT | MCL_FUTURE);

	int i;
	for (i = 0; i < ndisturbance; i++)
	{
		pthread_create(&ts[i], 0, disturbanceInTheForce, NULL);
	}

	for (i = 0; i < N_RT; i++)
	{
		rt_task_create(&rt[i], names[i], 0, 99, 0);
		rt_task_start(&rt[i], responseHandlerPeriodic, &channels[i]);
	}

	while (1);
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

	if (argc != 3)
	{
		printf("need two args\n");
		exit(1);
	}

	int p = atoi(argv[1]);
	int nd = atoi(argv[2]);

	assignmentA(p, nd);

	return 0;
}