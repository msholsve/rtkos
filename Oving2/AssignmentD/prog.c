#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>

sem_t mut;

bool running = true;
int var1 = 0;
int var2 = 0;

void *thread1(void *arg)
{
    while (running)
    {
        sem_wait(&mut);
        var1++;
        var2 = var1;
        sem_post(&mut);
    }

    return NULL;
}

void *thread2(void *arg)
{
    for (int i = 0; i < 20; i++)
    {
        sem_wait(&mut);
        printf("Number 1 is %i, number 2 is %i\n", var1, var2);
        sem_post(&mut);
        usleep(1000000);
    }
    running = false;

    return NULL;
}

int main(int argc, char *argv[])
{
    if(sem_init(&mut, 0, 1)) 
    { 
        printf("sem_init failed\n"); 
        exit(1); 
    }

    pthread_t t1, t2;

    pthread_create(&t1, 0, thread1, NULL);    
    pthread_create(&t2, 0, thread2, NULL);    
    
    pthread_join(t1, 0);
    pthread_join(t2, 0);

    exit(0);
}