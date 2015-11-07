
/**********************************************************
    Includes
**********************************************************/

#include <semaphore.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

#include "controller.h"

/**********************************************************
    Static members
**********************************************************/

// PID variables
static const double Kp = 10.0;
static const double Ki = 800.0;
static const double Kd = 0.0;

static double reference;
static double integral;
static double previous_error;

// Period for taskFunction
static long period_ns = 4000; 

// Recieved y variable
static double recv_y;

// Controll flow variables
static sem_t sem;
static pthread_t task;

/**********************************************************
    Functions
**********************************************************/

static double calculateInput(double y)
{
	double error;

	error = reference - y;
	integral += error * args->period_ns;
	derivative = error - previous_error;
	previous_error = error;

	return Kp * error + Ki * integral + Kd * derivative;
}


void controllerRecv(char * pkt)
{
	recv_y = atof(&pkt[8]);

	sem_post(&sem);
}

static void* taskFunction(void *arg)
{
	double u;
	struct timespec period;

	period_ns = *(long *)arg;

	udpSend("START");
	clock_gettime(CLOCK_REALTIME, &period);

	for (;;)
	{
		timespec_add_us(&period, period_ns/1000);
		
		udpSend("GET");

		sem_wait(&sem);

		u = calculateInput(recv_y);

		char pkt[14];
		snprintf(pkt, 14, "SET:%4.4f", u);
		udpSend(pkt);

		clock_nanosleep(&period);
	}

	updSend("STOP"):

	return NULL;
}

void controllerInit(void)
{
	sem_init(&sem, 0, 0);

	pthread_create(&task, NULL, taskFunction, NULL);
}

void controllerCleanup(void)
{
	ptrhead_join(task, NULL);

	sem_destroy(&sem);
}