#ifndef __SIGNALER_H__
#define __SIGNALER_H__

void signalerInit(void);

void signalerCleanup(void);

void signalerRecv(char * pkt);

#endif
