
/**********************************************************
    Includes
**********************************************************/

#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include "controller.h"
#include "udp.h"
#include "miniproject.h"

/**********************************************************
    Static members
**********************************************************/

// PID variables
static const double Kp = 5.0;
static const double Ki = 800.0;

static double reference = 1.0;
static double integral;
static double previous_error;

// Period for taskFunction
static const long period_ms = 4; 

// Recieved y variable, synchronized by recv_y_sem
static double recv_y;
static sem_t recv_y_sem;

/**********************************************************
    Functions
**********************************************************/

static double calculateInput(double y)
{
	double error;

	error = reference - y;
	integral += error * (double)period_ms / 1000.0;
	previous_error = error;

	return Kp * error + Ki * integral;
}

void controllerRecv(char * pkt)
{
	// Float begins at pos 9
	// Packet structued as: GET_ACK:xxx.xx
	recv_y = atof(&pkt[8]);

	sem_post(&recv_y_sem);
}

// Thread function
void* controllerTaskFunction(void *arg)
{
	(void)arg;

	double u;
	struct timespec period, endtime;

	// This starts the simulation on the server
	udpSend("START");
	
	// Initialize the timespecs	
	clock_gettime(CLOCK_REALTIME, &period);
	clock_gettime(CLOCK_REALTIME, &endtime);

	// Setting endtime to 500 ms
	timespec_add_us(&endtime, 500 * 1000);

	// Terminates when period surpases endtime
	while (timercmp(&period, &endtime, <))
	{		
		udpSend("GET");

		// Triggered by UDP module
		sem_wait(&recv_y_sem);
		u = calculateInput(recv_y);

		char pkt[14];
		snprintf(pkt, 14, "SET:%4.4f", u);
		udpSend(pkt);

		// Setting next resume time by adding one period
		// Note conversion from ms to us
		timespec_add_us(&period, period_ms*1000);
		clockNanosleep(&period);
	}

	udpSend("STOP");

	puts("Ended!");
	return NULL;
}

void controllerInit(void)
{
	sem_init(&recv_y_sem, 0, 0);
}

void controllerCleanup(void)
{
	sem_destroy(&recv_y_sem);
}
