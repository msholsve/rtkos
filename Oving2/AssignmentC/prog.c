#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define NUM_THREADS 5

sem_t sem;

int globalVar = 0;
int n;

void *threadFxn(void *arg)
{
    char *name = (char *)arg;
    int localVar = 0;
    for (int i = 0; i < n; i++)
    {
        sem_wait(&sem);
        localVar++;
        int volatile tmpVar = ++globalVar;
        printf("%s: globalVar = %i, tmpVar = %i, localVar = %i\n", name, globalVar, tmpVar, localVar);
        usleep(1000000);
        sem_post(&sem);
    }

    return NULL;
}

int main(int argc, char *argv[])
{

    if (argc < 2)
    {
        printf("Not enough arguments\n");
        exit(1);
    }
    
    if(sem_init(&sem, 0, 3)) 
    { 
        printf("sem_init failed\n"); 
        exit(1); 
    }

    pthread_t threads[NUM_THREADS];
    char *names[NUM_THREADS] = { "t1", "t2", "t3", "t4", "t5" };
    n = atoi(argv[1]);

    for (int i = 0; i < NUM_THREADS; i++)
    {
        pthread_create(&threads[i], 0, threadFxn, names[i]);    
    }
    
    for (int i = 0; i < NUM_THREADS; i++)
    {
        pthread_join(threads[i], 0);
    }

    exit(0);
}