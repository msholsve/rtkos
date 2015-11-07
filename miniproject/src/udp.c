/**********************************************************
    Includes
**********************************************************/

#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <stdio.h>

#include "udp.h"
#include "miniproject.h"
#include "signaler.h"
#include "controller.h"


/**********************************************************
    Static members
**********************************************************/
static struct udp_conn conn;
static pthread_mutex_t mutex;
static pthread_t task;

/**********************************************************
    Functions
**********************************************************/

void udpSend(char *packet)
{
	int len = strlen(packet);

	pthread_mutex_lock(&mutex);

	udp_send(&conn, packet, len);

	pthread_mutex_unlock(&mutex);
}

static void* taskFunction(void *arg)
{
	(void)arg;
	char buf[50];
	int result;
	for (;;)
	{
		result = udp_receive(&conn, buf, 50);
		if (result != 0)
			continue;

		switch(buf[0])
		{
		case 'S':
			signalerRecv(buf);
			break;

		case 'G':
			controllerRecv(buf);
			break;

		default:
			printf("udp FML\n");
			break;
		}
	}

	return NULL;
}

void udpInit(void)
{
	pthread_mutex_init(&mutex, NULL);

	pthread_mutex_lock(&mutex);

	char *ip = "localhost";
	int port = 9999;
	int result;

	result = udp_init_client(&conn, port, ip);
	printf("UDP init result: %i\n", result);

	pthread_mutex_unlock(&mutex);

	pthread_create(&task, NULL, taskFunction, NULL);
}

void udpCleanup(void)
{
	pthread_join(task, NULL);

	udp_close(&conn);

	pthread_mutex_destroy(&mutex);
}
