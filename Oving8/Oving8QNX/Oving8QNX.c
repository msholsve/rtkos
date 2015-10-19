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

int main(int argc, char *argv[]) {
	set_priority(3);
	// Creating shared memory
	int skynetDesc = shm_open("/skynetcore", O_RDWR | O_CREAT, S_IRWXU);
	ftruncate(skynetDesc, sizeof(struct skynetCore_t));
	struct skynetCore_t* skynetCore = mmap(0, sizeof(struct skynetCore_t), PROT_READ|PROT_WRITE, MAP_SHARED, skynetDesc, 0);

	pthread_mutexattr_t skynetProtectorAttr;
	pthread_mutexattr_init(&skynetProtectorAttr);
	pthread_mutexattr_setpshared(&skynetProtectorAttr, PTHREAD_PROCESS_SHARED);
	pthread_mutex_init(&skynetCore->protector, &skynetProtectorAttr);

	pthread_mutex_lock(&skynetCore->protector);
	skynetCore->pid = getpid();
	pthread_mutex_unlock(&skynetCore->protector);

	int SkynetChannelId = ChannelCreate(0);
	int SkynetCode;
	struct _msg_info SkynetClientInfo;
	while(1){
		printf("My priority before receiving is %i\n", get_priority());
		int receivedMessageId = MsgReceive(SkynetChannelId, &SkynetCode, sizeof(int), &SkynetClientInfo);
		printf("My priority after receiving is %i\n", get_priority());
		printf("I got %i from the skynet client with pid %i and thread id %i\n", SkynetCode, SkynetClientInfo.pid, SkynetClientInfo.tid);
		SkynetCode += 1;
		MsgReply(receivedMessageId, 0, &SkynetCode, sizeof(int));
	}

	return 0;
}
