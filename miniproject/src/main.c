
/**********************************************************
    Includes
**********************************************************/

#include <stdio.h>
#include <pthread.h>

#include "miniproject.h"

/**********************************************************
    Defines
**********************************************************/

#define NUM_THREADS 2

/**********************************************************
    Types
**********************************************************/

typedef void* (*pfunc_t)(void*);

typedef struct con_args
{
    long period_ns;
} con_args_t, *pCon_args_t;

typedef struct sig_args
{
    long dummy;
} sig_args_t, *pSig_args_t;

/**********************************************************
    Globals
**********************************************************/

static const con_args_t default_cargs =
{
    .period_ns = 4000L,
};
static const sig_args_t default_sargs =
{
    .dummy = 0L,
};

static const double Kp = 10.0;
static const double Ki = 800.0;
static const double Kd = 0.0;

static double reference;
static double integral;
static double previous_error;

/**********************************************************
    Functions
**********************************************************/

void* controller_func(void *pArgs)
{
    pCon_args_t args = (pCon_args_t)pArgs;

	double y, error, derivative, u;

	error = reference - y;
	integral += error * args->period_ns;
	derivative = error - previous_error;
	previous_error = error;
	u = Kp * error + Ki * integral + Kd * derivative;

    return NULL;
}

void* signal_func(void *pArgs)
{
    pSig_args_t args = (pSig_args_t)pArgs;

    return NULL;
}

/**********************************************************
    Main
**********************************************************/

int main()
{
    printf("Start Controller\n");

	pthread_t threads[NUM_THREADS];
    pfunc_t funcs[NUM_THREADS] = { controller_func, signal_func };
    void *args[NUM_THREADS] = { (void *)&default_cargs, (void *)&default_sargs };

    for (int i = 0; i < NUM_THREADS; ++i)
        pthread_create(&threads[i], NULL, funcs[i], args[i]);

    for (int i = 0; i < NUM_THREADS; ++i)
        pthread_join(threads[i], NULL);

	return 0;
}
