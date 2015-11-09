#ifndef __SIGNALER_H__
#define __SIGNALER_H__

void signalerInit(void);

void signalerCleanup(void);

// Called by the UDP module when relevant packet recieved
void signalerNotify(void);

void* signalerTaskFunction(void *arg);

#endif
