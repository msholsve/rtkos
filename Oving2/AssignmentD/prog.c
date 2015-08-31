#include <stdio.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>

pthread_mutex_t mut;

bool volatile running = true;
int volatile var1 = 0;
int volatile var2 = 0;

void *thread1(void *arg)
{
    while (running)
    {
        pthread_mutex_lock(&mut);
        var1++;
        var2 = var1;
        pthread_mutex_unlock(&mut);
    }

    return NULL;
}

void *thread2(void *arg)
{
    for (int i = 0; i < 20; i++)
    {
        pthread_mutex_lock(&mut);
        printf("Number 1 is %i, number 2 is %i\n", var1, var2);
        pthread_mutex_unlock(&mut);
        usleep(1000000);
    }
    running = false;

    return NULL;
}

int main(int argc, char *argv[])
{
    if(pthread_mutex_init(&mut, 0)) 
    { 
        printf("mutex_init failed\n"); 
        exit(1); 
    }

    pthread_t t1, t2;

    pthread_create(&t1, 0, thread1, NULL);    
    pthread_create(&t2, 0, thread2, NULL);    
    
    pthread_join(t1, 0);
    pthread_join(t2, 0);

    exit(0);
}