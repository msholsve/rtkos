
/**********************************************************
	Includes
**********************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include <sys/mman.h>
#include <native/task.h>
#include <native/timer.h>
#include <native/sem.h>
#include <native/mutex.h>
#include <rtdk.h>


/**********************************************************
	Defines
**********************************************************/
#define MS_TO_NS(x) ((x) * 1000 * 1000)

/**********************************************************
	Globals
**********************************************************/

static RT_SEM sync;
static RT_SEM resource;
static RT_MUTEX mutexA, mutexB;

/**********************************************************
	Utils
**********************************************************/

void checkRetVal(int v, char* name)
{
	switch (v)
	{
	case EINVAL: 	  rt_printf("%s, EINVAL fault\n", name); break;
	case EIDRM: 	  rt_printf("%s, EIDRM fault\n", name); break;
	case EWOULDBLOCK: rt_printf("%s, EWOULDBLOCK fault\n", name); break;
	case EINTR: 	  rt_printf("%s, EINTR fault\n", name); break;
	case ETIMEDOUT:   rt_printf("%s, ETIMEDOUT fault\n", name); break;
	case EPERM: 	  rt_printf("%s, EPERM fault\n", name); break;
	default:    	  rt_printf("%s, other fault %i\n", name, v); break;
	}
}

void busy_wait_ms(unsigned long delay)
{
	unsigned long count = 0;
	while (count <= delay*10)
	{
		rt_timer_spin(1000*100);
		count++;
	}
}

/**********************************************************
	Assignment A
**********************************************************/

#define A_TASKS 2

void waitForSemaphore(void *arg)
{
	(void)arg;

	rt_sem_p(&sync, TM_INFINITE);
	
	RT_TASK_INFO taskInfo;
	rt_task_inquire(NULL, &taskInfo);

	rt_printf("%s Released!\n", taskInfo.name);
}

void assignmentA()
{
	printf("Starting Assignment A!\n");

	// Initilize rt_prinft
	rt_print_auto_init(1);

	RT_TASK rt[A_TASKS];
	char *names[A_TASKS] = { "thread1", "thread2" };

	rt_sem_create(&sync, "sync", 0, 0);
	mlockall(MCL_CURRENT | MCL_FUTURE);

	int i;
	for(i = 0; i < A_TASKS; i++)
	{
		rt_task_create(&rt[i], names[i], 0, 98-i, 0);
		rt_task_start(&rt[i], waitForSemaphore, NULL);
	}

	// Sync 
	rt_task_shadow(NULL, "DA MAIN", 99, 0);
	rt_task_sleep(100000000);
	rt_sem_broadcast(&sync);
	rt_task_sleep(100 * 1000 * 1000);
	rt_sem_delete(&sync);
}

/**********************************************************
	Assignment B
**********************************************************/

#define B_TASKS 3

typedef void (* taskFxn)(void *);

void lowPriority(void* arg)
{
	(void)arg;

	rt_sem_p(&sync, TM_INFINITE);

	RT_TASK_INFO taskInfo;
	rt_task_inquire(NULL, &taskInfo);

	rt_printf("\t\t%s Starting!\n", taskInfo.name);

	rt_sem_p(&resource, TM_INFINITE);
	rt_printf("\t\t%s Got hoe!\n", taskInfo.name);
	busy_wait_ms(3);

	rt_printf("\t\t%s Released hoe!\n", taskInfo.name);
	rt_sem_v(&resource);

	rt_printf("\t\t%s done!\n", taskInfo.name);
}

void mediumPriority(void* arg)
{
	(void)arg;

	rt_sem_p(&sync, TM_INFINITE);

	RT_TASK_INFO taskInfo;
	rt_task_inquire(NULL, &taskInfo);

	rt_printf("\t%s Starting!\n", taskInfo.name);

	rt_task_sleep(MS_TO_NS(1));

	rt_printf("\t%s busywaiting!\n", taskInfo.name);
	busy_wait_ms(5);

	rt_printf("\t%s done!\n", taskInfo.name);
}

void highPriority(void* arg)
{
	(void)arg;

	rt_sem_p(&sync, TM_INFINITE);

	RT_TASK_INFO taskInfo;
	rt_task_inquire(NULL, &taskInfo);

	rt_printf("%s Starting!\n", taskInfo.name);

	rt_task_sleep(MS_TO_NS(2));

	rt_sem_p(&resource, TM_INFINITE);
	rt_printf("%s Got hoe!\n", taskInfo.name);
	busy_wait_ms(2);

	rt_printf("%s Released hoe!\n", taskInfo.name);
	rt_sem_v(&resource);

	rt_printf("%s done!\n", taskInfo.name);
}

void assignmentB()
{
	printf("Starting Assignment B!\n\n");

	// Initilize rt_prinft
	rt_print_auto_init(1);

	RT_TASK rt[B_TASKS];
	char *names[B_TASKS] = { "highPriority", "mediumPriority", "lowPriority"};
	taskFxn	tasks[B_TASKS] = { highPriority, mediumPriority, lowPriority };

	rt_sem_create(&sync, "SyncSem", 0, 0);
	rt_sem_create(&resource, "ResourceSem", 1, 0);
	mlockall(MCL_CURRENT | MCL_FUTURE);

	int i;
	for(i = 0; i < B_TASKS; i++)
	{
		rt_task_create(&rt[i], names[i], 0, 98-i, 0);
		rt_task_start(&rt[i], tasks[i], NULL);
	}

	rt_task_shadow(NULL, "DA MAIN", 99, 0);
	rt_task_sleep(MS_TO_NS(100));
	rt_sem_broadcast(&sync);
	rt_task_sleep(MS_TO_NS(1000));

	rt_sem_delete(&sync);
	rt_sem_delete(&resource);
}

/**********************************************************
	Assignment C
**********************************************************/

#define C_TASKS 2


void lowC(void* arg)
{
	(void)arg;

	rt_sem_p(&sync, TM_INFINITE);

	RT_TASK_INFO taskInfo;
	rt_task_inquire(NULL, &taskInfo);

	rt_printf("\t%s Starting!\n", taskInfo.name);

	checkRetVal(rt_mutex_acquire(&mutexA, TM_INFINITE), taskInfo.name);
	rt_printf("\t%s acquired A!\n", taskInfo.name);
	busy_wait_ms(3);

	checkRetVal(rt_mutex_acquire(&mutexB, TM_INFINITE), taskInfo.name);
	rt_printf("\t%s acquired B!\n", taskInfo.name);
	busy_wait_ms(3);

	rt_printf("\t%s releasing B!\n", taskInfo.name);
	rt_mutex_release(&mutexB);
	rt_printf("\t%s releasing A!\n", taskInfo.name);
	rt_mutex_release(&mutexA);
	busy_wait_ms(1);

	rt_printf("\t%s done!\n", taskInfo.name);
}

void highC(void* arg)
{
	(void)arg;

	rt_sem_p(&sync, TM_INFINITE);

	RT_TASK_INFO taskInfo;
	rt_task_inquire(NULL, &taskInfo);

	rt_printf("\t\t%s Starting!\n", taskInfo.name);

	rt_printf("\t\t%s sleeping!\n", taskInfo.name);
	rt_task_sleep(MS_TO_NS(1));
	
	checkRetVal(rt_mutex_acquire(&mutexB, TM_INFINITE), taskInfo.name);
	rt_printf("\t\t%s acquired B!\n", taskInfo.name);
	busy_wait_ms(1);

	checkRetVal(rt_mutex_acquire(&mutexA, TM_INFINITE), taskInfo.name);
	rt_printf("\t\t%s acquired A!\n", taskInfo.name);
	busy_wait_ms(2);

	rt_printf("\t\t%s releasing A!\n", taskInfo.name);
	rt_mutex_release(&mutexA);
	rt_printf("\t\t%s releasing B!\n", taskInfo.name);
	rt_mutex_release(&mutexB);
	busy_wait_ms(1);

	rt_printf("\t\t%s done!\n", taskInfo.name);
}


void assignmentC()
{
	printf("Starting Assignment C!\n\n");
	// Initilize rt_prinft
	rt_print_auto_init(1);

	RT_TASK rt[C_TASKS];
	char *names[C_TASKS] = { "highC", "lowC"};
	taskFxn	tasks[C_TASKS] = { highC, lowC };

	rt_sem_create(&sync, "SyncSem", 0, 0);
	rt_mutex_create(&mutexA, "mutexA");
	rt_mutex_create(&mutexB, "mutexB");
	mlockall(MCL_CURRENT | MCL_FUTURE);

	int i;
	for(i = 0; i < C_TASKS; i++)
	{
		rt_task_create(&rt[i], names[i], 0, 98-i, 0);
		rt_task_start(&rt[i], tasks[i], NULL);
	}

	rt_task_shadow(NULL, "DA MAIN", 99, 0);
	rt_task_sleep(MS_TO_NS(100));
	rt_sem_broadcast(&sync);
	rt_task_sleep(MS_TO_NS(1000));

	rt_sem_delete(&sync);
	rt_mutex_delete(&mutexA);
	rt_mutex_delete(&mutexB);
}


/**********************************************************
	Assignment D
**********************************************************/

#define D_TASKS 2


void lowD(void* arg)
{
	(void)arg;

	rt_sem_p(&sync, TM_INFINITE);

	RT_TASK_INFO taskInfo;
	rt_task_inquire(NULL, &taskInfo);

	rt_printf("\t%s Starting!\n", taskInfo.name);

	checkRetVal(rt_mutex_acquire(&mutexA, TM_INFINITE), taskInfo.name);
	rt_printf("\t%s acquired A!\n", taskInfo.name);
	rt_task_set_priority(NULL, 98);
	busy_wait_ms(3);

	checkRetVal(rt_mutex_acquire(&mutexB, TM_INFINITE), taskInfo.name);
	rt_printf("\t%s acquired B!\n", taskInfo.name);
	busy_wait_ms(3);

	rt_printf("\t%s releasing B!\n", taskInfo.name);
	rt_mutex_release(&mutexB);
	rt_printf("\t%s releasing A!\n", taskInfo.name);
	rt_mutex_release(&mutexA);
	rt_task_set_priority(NULL, 97);
	busy_wait_ms(1);

	rt_printf("\t%s done!\n", taskInfo.name);
}

void highD(void* arg)
{
	(void)arg;

	rt_sem_p(&sync, TM_INFINITE);

	RT_TASK_INFO taskInfo;
	rt_task_inquire(NULL, &taskInfo);

	rt_printf("\t\t%s Starting!\n", taskInfo.name);

	rt_printf("\t\t%s sleeping!\n", taskInfo.name);
	rt_task_sleep(MS_TO_NS(1));
	
	checkRetVal(rt_mutex_acquire(&mutexB, TM_INFINITE), taskInfo.name);
	rt_printf("\t\t%s acquired B!\n", taskInfo.name);
	busy_wait_ms(1);

	checkRetVal(rt_mutex_acquire(&mutexA, TM_INFINITE), taskInfo.name);
	rt_printf("\t\t%s acquired A!\n", taskInfo.name);
	busy_wait_ms(2);

	rt_printf("\t\t%s releasing A!\n", taskInfo.name);
	rt_mutex_release(&mutexA);
	rt_printf("\t\t%s releasing B!\n", taskInfo.name);
	rt_mutex_release(&mutexB);
	busy_wait_ms(1);

	rt_printf("\t\t%s done!\n", taskInfo.name);
}

void assignmentD()
{
	printf("Starting Assignment D!\n\n");
	// Initilize rt_prinft
	rt_print_auto_init(1);

	RT_TASK rt[C_TASKS];
	char *names[C_TASKS] = { "highD", "lowD"};
	taskFxn	tasks[C_TASKS] = { highD, lowD };

	rt_sem_create(&sync, "SyncSem", 0, 0);
	rt_mutex_create(&mutexA, "mutexA");
	rt_mutex_create(&mutexB, "mutexB");
	mlockall(MCL_CURRENT | MCL_FUTURE);

	int i;
	for(i = 0; i < D_TASKS; i++)
	{
		rt_task_create(&rt[i], names[i], 0, 98-i, 0);
		rt_task_start(&rt[i], tasks[i], NULL);
	}

	rt_task_shadow(NULL, "DA MAIN", 99, 0);
	rt_task_sleep(MS_TO_NS(100));
	rt_sem_broadcast(&sync);
	rt_task_sleep(MS_TO_NS(1000));

	rt_sem_delete(&sync);
	rt_mutex_delete(&mutexA);
	rt_mutex_delete(&mutexB);
}


/**********************************************************
	Main
**********************************************************/

int main(int argc, char *argv[])
{
	char assignment = 'A';

	if (argc > 1)
	{
		assignment = *argv[1];
	}

	switch (assignment)
	{
	case 'A': assignmentA(); break;
	case 'B': assignmentB(); break;
	case 'C': assignmentC(); break;
	case 'D': assignmentD(); break;
	default: printf("Nein Nein Nein Nein Nein Nein Nein!\n"); break; 
	}

	return 0;
}