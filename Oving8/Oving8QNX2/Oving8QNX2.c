#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <sys/dispatch.h>
#include <sys/mman.h>

struct skynetCore_t{
	pthread_mutex_t protector;
	pid_t pid;
};

#define NUM_NAGGERS 4

// Utils

int set_priority(int priority)
{
	int policy;
     struct sched_param param;
     // check priority in range
     if (priority < 1 || priority > 63) return -1;
     // set priority
     pthread_getschedparam(pthread_self(), &policy, &param);
     param.sched_priority = priority;
     return pthread_setschedparam(pthread_self(), policy, &param);
}

int get_priority()
{
	int policy;
     struct sched_param param;
     // get priority
     pthread_getschedparam(pthread_self(), &policy, &param);
     return param.sched_curpriority;
}

//   N _ G G E R S
// people who annoy you

void nagger(void *arg){
	set_priority(*(int*)arg);

	// Creating shared memory
	int skynetDesc = shm_open("/skynetcore", O_RDWR, S_IRWXU);
	struct skynetCore_t* skynetCore = mmap(0, sizeof(struct skynetCore_t), PROT_READ|PROT_WRITE, MAP_SHARED, skynetDesc, 0);

	pthread_mutex_lock(&skynetCore->protector);
	int corePid = skynetCore->pid;
	pthread_mutex_unlock(&skynetCore->protector);

	int message = *(int*)arg;
	int skynetChannelId = ConnectAttach(0, corePid, 1, 0, 0);
	MsgSend(skynetChannelId, &message, sizeof(int), &message, sizeof(int));
	printf("I sent %i and got %i back!\n", *(int*)arg, message);
	ConnectDetach(skynetChannelId);
}

int main(int argc, char *argv[]) {
	// Creating shared memory
	/*int skynetDesc = shm_open("/skynetcore", O_RDWR, S_IRWXU);
	struct skynetCore_t* skynetCore = mmap(0, sizeof(struct skynetCore_t), PROT_READ|PROT_WRITE, MAP_SHARED, skynetDesc, 0);

	pthread_mutex_lock(&skynetCore->protector);
	int corePid = skynetCore->pid;
	pthread_mutex_unlock(&skynetCore->protector);

	printf("Skynet PID is: %i\n", corePid);

	int skynetChannelId = ConnectAttach(0, corePid, 1, 0, 0);
	int huehue = 5;
	MsgSend(skynetChannelId, &huehue, sizeof(int), &huehue, sizeof(int));
	printf("I sent %i and got %i back!\n", 5, huehue);

	ConnectDetach(skynetChannelId);*/
	set_priority(6);
	int i;
	pthread_t naggers[NUM_NAGGERS];
	int ids[NUM_NAGGERS] = { 1, 2, 4, 5 };
	for (i = 0; i < NUM_NAGGERS; i++)
	{
		pthread_create(&naggers[i], 0, nagger, &ids[i]);
	}
	for (i = 0; i < NUM_NAGGERS; i++)
	{
		pthread_join(naggers[i], 0);
	}

	return 0;
}

