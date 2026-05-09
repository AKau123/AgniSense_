#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sched.h>
#include <stdint.h>

#include <sys/neutrino.h>
void* overload_task(void* arg)
{
    volatile unsigned long i;

    while(1)
    {
        printf("MEDIA CPU LOAD SPIKE...\n");

        for(i = 0; i < 900000000; i++)
        {
        }
    }

    return NULL;
}

int main()
{
    pthread_t t1;
    pthread_t t2;
    pthread_t t3;

    int runtime = 0;

/* LOW PRIORITY MEDIA PROCESS */

    struct sched_param param;

    param.sched_priority = 5;

    pthread_setschedparam(
        pthread_self(),
        SCHED_RR,
        &param
    );

/* CPU3 ONLY */

    unsigned runmask = 0x8;

    ThreadCtl(
        _NTO_TCTL_RUNMASK,
        (void *)(uintptr_t)runmask
    );

    printf("MEDIA PROCESS STARTED\n");

    printf("MEDIA DOMAIN RUNNING ON CPU3\n");

/* CREATE MASSIVE LOAD */

    pthread_create(
        &t1,
        NULL,
        overload_task,
        NULL
    );

    pthread_create(
        &t2,
        NULL,
        overload_task,
        NULL
    );

    pthread_create(
        &t3,
        NULL,
        overload_task,
        NULL
    );

/* SIMULATED FAILURE */

    while(1)
    {
        delay(1000);

        runtime++;

        printf("MEDIA OVERLOAD ACTIVE...\n");

        if(runtime >= 10)
        {
            printf("\n");
            printf("MEDIA PROCESS CRASHED\n");
            printf("INFOTAINMENT FAILURE DETECTED\n");

            printf("\n");

            printf("CRITICAL SYSTEMS STILL ACTIVE:\n");

            printf("obstacle detection active\n");

            printf(" warning system active\n");

            printf(" motor control active\n");

            printf(" supervisor IPC active\n");

            printf(" UART/V2V active\n");

            printf("\n");

            exit(1);
        }
    }

    return 0;
}
