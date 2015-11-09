#ifndef __UDP_H__
#define __UDP_H__

void udpInit(char* ip, int sig);

void udpCleanup(void);

void udpSend(char* packet);

void* udpTaskFunction(void *arg);

#endif
