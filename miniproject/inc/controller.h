#ifndef __CONTROLLER_H__
#define __CONTROLLER_H__

void controllerInit(void);

void controllerCleanup(void);

// Called by the UDP module when relevant packet recieved
void controllerRecv(char * pkt);

void* controllerTaskFunction(void *arg);

#endif
