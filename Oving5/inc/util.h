#ifndef __UTIL_H__
#define __UTIL_H__

#include <time.h>

#define CHANNEL_A 1
#define CHANNEL_B 2
#define CHANNEL_C 3

#define HIGH 1
#define LOW  0

int setCpu(int cpuNumber);
void doResponse(int channel);
void testPin(int channel);
void timespec_add_us(struct timespec *t, long us);

#endif /* __UTIL_H__ */