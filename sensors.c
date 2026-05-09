#include "common.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <math.h>
#include <time.h>

#include <sys/mman.h>
#include <sys/neutrino.h>
#include <sys/dispatch.h>

#include <hw/inout.h>

#define GPIO_BASE 0xFE200000
#define BLOCK_SIZE 4096

#define GPFSEL0 (0x00/4)
#define GPFSEL1 (0x04/4)

#define GPSET0  (0x1C/4)
#define GPCLR0  (0x28/4)
#define GPLEV0  (0x34/4)

#define TRIG      (1<<5)
#define ECHO_PIN  (1<<6)

#define HALL_SENSOR (1<<19)

volatile uint32_t *gpio;

vehicle_data_t data;

pthread_mutex_t mutex =
PTHREAD_MUTEX_INITIALIZER;

/* ========================================== */

double get_distance()
{
    struct timespec start,end;

    gpio[GPCLR0] = TRIG;

    usleep(2);

    gpio[GPSET0] = TRIG;

    usleep(10);

    gpio[GPCLR0] = TRIG;

    clock_gettime(CLOCK_MONOTONIC,
                  &start);

    while(!(gpio[GPLEV0] & ECHO_PIN))
    {
        clock_gettime(CLOCK_MONOTONIC,
                      &end);

        double t =
        (end.tv_sec - start.tv_sec) +
        (end.tv_nsec - start.tv_nsec)/1e9;

        if(t > 0.03)
            return 999;
    }

    clock_gettime(CLOCK_MONOTONIC,
                  &start);

    while(gpio[GPLEV0] & ECHO_PIN)
    {
        clock_gettime(CLOCK_MONOTONIC,
                      &end);

        double t =
        (end.tv_sec - start.tv_sec) +
        (end.tv_nsec - start.tv_nsec)/1e9;

        if(t > 0.03)
            return 999;
    }

    clock_gettime(CLOCK_MONOTONIC,
                  &end);

    double t =
    (end.tv_sec - start.tv_sec) +
    (end.tv_nsec - start.tv_nsec)/1e9;

    return (t * 34300) / 2;
}

/* ========================================== */

void *ultrasonic_thread(void *arg)
{
    while(1)
    {
        double d = get_distance();

        pthread_mutex_lock(&mutex);

        data.distance = d;

        pthread_mutex_unlock(&mutex);

        delay(50);
    }
}

/* ========================================== */

void *hall_thread(void *arg)
{
    int last = 1;

    struct timespec prev, now;

    clock_gettime(CLOCK_MONOTONIC,
                  &prev);

    while(1)
    {
        int current =
        (gpio[GPLEV0] & HALL_SENSOR)
        ? 1 : 0;

        if(last == 1 && current == 0)
        {
            clock_gettime(CLOCK_MONOTONIC,
                          &now);

            double dt =
            (now.tv_sec - prev.tv_sec) +
            (now.tv_nsec - prev.tv_nsec)/1e9;

            double rpm =
            (1.0 / dt) * 60.0;

            double wheel_diameter = 0.06;

            double circumference =
            3.14159 * wheel_diameter;

            double speed_kmph =
            (rpm * circumference * 60.0)
            / 1000.0;

            pthread_mutex_lock(&mutex);

            data.speed = speed_kmph;

            pthread_mutex_unlock(&mutex);

            prev = now;
        }

        last = current;

        delay(1);
    }
}

/* ========================================== */

void *sender_thread(void *arg)
{
    int coid;

    coid = name_open("SUPERVISOR",0);

    if(coid == -1)
    {
        printf("name_open failed\n");
        return NULL;
    }

    while(1)
    {
        pthread_mutex_lock(&mutex);

        if(data.distance > 60)
        {
            strcpy(data.message,"CLEAR");
        }
        else if(data.distance > 40)
        {
            strcpy(data.message,"TRAFFIC");
        }
        else if(data.distance > 20)
        {
            strcpy(data.message,"OBSTACLE");
        }
        else
        {
            strcpy(data.message,
                   "CRITICAL");
        }

        MsgSend(coid,
                &data,
                sizeof(data),
                NULL,
                0);

        pthread_mutex_unlock(&mutex);

        delay(500);
    }
}

/* ========================================== */

int main()
{
    pthread_t ultra_t;
    pthread_t hall_t;
    pthread_t sender_t;

    ThreadCtl(_NTO_TCTL_IO,0);

    unsigned runmask = 0x1;

    ThreadCtl(_NTO_TCTL_RUNMASK,
              (void *)(uintptr_t)runmask);

    gpio = mmap_device_memory(
            NULL,
            BLOCK_SIZE,
            PROT_READ |
            PROT_WRITE |
            PROT_NOCACHE,
            0,
            GPIO_BASE);

    gpio[GPFSEL0] &= ~(7<<15);
    gpio[GPFSEL0] |=  (1<<15);

    gpio[GPFSEL0] &= ~(7<<18);

    gpio[GPFSEL1] &= ~(7<<27);

    pthread_create(&ultra_t,
                   NULL,
                   ultrasonic_thread,
                   NULL);

    pthread_create(&hall_t,
                   NULL,
                   hall_thread,
                   NULL);

    pthread_create(&sender_t,
                   NULL,
                   sender_thread,
                   NULL);

    pthread_join(ultra_t,NULL);
    pthread_join(hall_t,NULL);
    pthread_join(sender_t,NULL);

    return 0;
}
