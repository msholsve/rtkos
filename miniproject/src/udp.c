
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
static pthread_mutex_t udp_send_mutex;

static int signal_enable = 0;

/**********************************************************
    Functions
**********************************************************/

void udpSend(char *packet)
{
	// +1 for termination char
	int len = strlen(packet) + 1;

	pthread_mutex_lock(&udp_send_mutex);

	udp_send(&conn, packet, len);

	pthread_mutex_unlock(&udp_send_mutex);
}

// Thread function
void* udpTaskFunction(void *arg)
{
	(void)arg;

	char buf[50];
	int nbytes;

	// Event loop
	for (;;)
	{
		// udp_receive is blocking
		nbytes = udp_receive(&conn, buf, 50);
		if (nbytes == 0)
			continue;

		// Only two possible packets, testing
		// for first char
		switch(buf[0])
		{
		// Packet: "SIGNAL"
		case 'S':
			if (signal_enable)
				signalerNotify();
			break;

		// Packet: "GET_ACK:xxx.xxx"
		case 'G':
			controllerRecv(buf);
			break;

		default:
			printf("Invalid UDP packet\n");
			break;
		}
	}

	return NULL;
}

void udpInit(char* ip, int sig)
{
	signal_enable = sig;

	// Hard coded port to match the server
	int port = 9999;

	int result = udp_init_client(&conn, port, ip);
	if (result == -1)
	{
		printf("UDP init failed\n");
		exit(1);
	}

	pthread_mutex_init(&udp_send_mutex, NULL);
}

void udpCleanup(void)
{
	udp_close(&conn);

	pthread_mutex_destroy(&udp_send_mutex);
}
