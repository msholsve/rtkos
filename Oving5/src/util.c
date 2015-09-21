
#include <pthread.h>
#include <sched.h>
#include <unistd.h>
#include <time.h>

#include "io.h"
#include "util.h"

int setCpu(int cpuNumber)
{
	// setting cpu set to the selected cpu
	cpu_set_t cpu;
	CPU_ZERO(&cpu);
	CPU_SET(cpuNumber, &cpu);

	// set cpu set to current thread and return
	return pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpu);
}

void doResponse(int channel)
{
	io_write(channel, LOW);
	usleep(5);
	io_write(channel, HIGH);
}

void testPin(int channel)
{
	io_write(channel, HIGH);

	while (1)
	{
		while(io_read(channel));
		
		doResponse(channel);

		//printf("Channel %i activated!\n", channel);
	}
}

void timespec_add_us(struct timespec *t, long us)
{
	// add microseconds to timespecs nanosecond counter
	t->tv_nsec += us * 1000;

	// if wrapping nanosecond counter, increment second counter
	if (t->tv_nsec > 1000000000)
	{
		t->tv_nsec = t->tv_nsec - 1000000000;
		t->tv_sec++;
	}
}
