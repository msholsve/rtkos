#ifndef MINIPROJECT_H_
#define MINIPROJECT_H_

#include <arpa/inet.h>
#include <time.h>

// typedef
typedef void (*sighandler_t)(int);

// structs that store the information needed for an udp connection
struct udp_conn
{
	int sock;
	struct sockaddr_in server;
	struct sockaddr_in client;
	socklen_t client_len;
};

// initialize the struct and connect to a udp server on the given port and ip
int udp_init_client(struct udp_conn *udp, int port, char *ip);

// function for sending a string over an udp connection
int udp_send(struct udp_conn *udp, char *buf, int len);

// function for receiving a string over an udp connection
int udp_receive(struct udp_conn *udp, char *buf, int len);

// function for closing a connection
void udp_close(struct udp_conn *udp);

// function replacing clock_nanosleep
// DO NOT use for periods over 500 ms
int clockNanosleep(struct timespec *next);

void timespec_add_us(struct timespec *t, long us);

// Convenience macro for struct timespec comparing
#define timercmp(a, b, CMP)          \
  (((a)->tv_sec == (b)->tv_sec) ?    \
   ((a)->tv_nsec CMP (b)->tv_nsec) : \
   ((a)->tv_sec CMP (b)->tv_sec))

#endif /* MINIPROJECT_H_ */
