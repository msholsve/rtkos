#include <stdio.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>

#define NUM_PHILS 5

pthread_mutex_t forks[NUM_PHILS];

// Philosopher #i needs fork #i and #(i+1)%N
void *philosopherFxn(void *arg)
{
    int i = *(int *)arg;
    int fork1 = i, fork2 = (i+1)%NUM_PHILS;

    for (;;)
    {
        if (pthread_mutex_lock(&forks[fork1]) != 0) 
            goto cleanup_done; 
        if (pthread_mutex_trylock(&forks[fork2]) != 0) 
            goto cleanup_partial;

        printf("Philosopher %i is eating!\n", i);

        pthread_mutex_unlock(&forks[fork2]);
    cleanup_partial:  
        pthread_mutex_unlock(&forks[fork1]);
    cleanup_done:
        
        ;//usleep(10000);
    }

    return NULL;
}

int main(int argc, char *argv[])
{
    for (int i = 0; i < NUM_PHILS; i++)
    {
        if(pthread_mutex_init(&forks[i], 0)) 
        { 
            printf("mutex_init failed\n"); 
            exit(1); 
        }
    }

    pthread_t philosophers[NUM_PHILS];
    int ids[NUM_PHILS] = { 0, 1, 2, 3, 4 };
    for (int i = 0; i < NUM_PHILS; i++)
    {
        pthread_create(&philosophers[i], 0, philosopherFxn, &ids[i]);    
    }
    
    for (int i = 0; i < NUM_PHILS; i++) 
    {
        pthread_join(philosophers[i], 0);
    }

    exit(0);
}