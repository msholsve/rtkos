#ifndef __UDP_H__
#define __UDP_H__

void udpInit(void);

void udpCleanup(void);

void udpSend(char* packet);

#endif