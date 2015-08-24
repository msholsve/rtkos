#include <sys/times.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

#define SECONDS 5

void *busyWaitDelay(void *arg)
{
	char *thread_name = (char *)arg;

	int tps = sysconf(_SC_CLK_TCK);
	clock_t start;
	struct tms exec_time;

	times(&exec_time);
	start = exec_time.tms_utime;

	printf("Thread name %s starting\n", thread_name);

	while ((exec_time.tms_utime - start) < (SECONDS * tps)) 
	{
		times(&exec_time);
	}

	printf("Thread name %s done\n", thread_name);

	return NULL;
}

void *sleeper(void *arg)
{
	char *thread_name = (char *)arg;

	printf("Thread name %s starting\n", thread_name);

	sleep(SECONDS);

	printf("Thread name %s done\n", thread_name);

	return NULL;
}

int main()
{
	pthread_t t1, t2;

	char *t1_name = "Busy Ola", *t2_name = "Sleeping Per";

	pthread_create(&t1, 0, busyWaitDelay, t1_name);
	pthread_create(&t2, 0, sleeper, t2_name);

	pthread_join(t1, 0);
	pthread_join(t2, 0);

	return 0;
}