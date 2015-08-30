#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

int globalVar = 0;

void threadFxn(char *name, int n);

void *thread1(void *arg)
{
    int n = *(int *)arg;
    char *name = "thread 1";
    threadFxn(name, n);

    return NULL;
}

void *thread2(void *arg)
{
    int n = *(int *)arg;
    char *name = "thread 2";
    threadFxn(name, n);

    return NULL;
}

void threadFxn(char *name, int n)
{
    int localVar = 0;
    for (int i = 0; i < n; i++)
    {
        globalVar++;
        localVar++;
        printf("%s: globalVar = %i, localVar = %i\n", name, globalVar, localVar);
    }
}

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        printf("Not enough arguments\n");
        exit(1);
    }

    pthread_t t1, t2;
    int n = atoi(argv[1]);

    pthread_create(&t1, 0, thread1, &n);
    pthread_create(&t2, 0, thread2, &n);

    pthread_join(t1, 0);
    pthread_join(t2, 0);

    exit(0);
}