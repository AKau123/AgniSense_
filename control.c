#include "common.h"

#include <stdio.h>
#include <stdint.h>
#include <unistd.h>

#include <sys/mman.h>
#include <sys/neutrino.h>
#include <sys/dispatch.h>

#define GPIO_BASE 0xFE200000
#define BLOCK_SIZE 4096

#define GPFSEL1 (0x04/4)
#define GPFSEL2 (0x08/4)

#define GPSET0 (0x1C/4)
#define GPCLR0 (0x28/4)

/* MOTOR */

#define IN1 (1<<17)
#define ENA (1<<18)

#define IN3 (1<<22)
#define ENB (1<<23)

volatile uint32_t *gpio;

int main()
{
    name_attach_t *attach;

    vehicle_data_t data;

    ThreadCtl(_NTO_TCTL_IO,0);

/* CPU1 */

    unsigned runmask = 0x2;

    ThreadCtl(
        _NTO_TCTL_RUNMASK,
        (void *)(uintptr_t)runmask
    );

/* GPIO */

    gpio = mmap_device_memory(
            NULL,
            BLOCK_SIZE,
            PROT_READ |
            PROT_WRITE |
            PROT_NOCACHE,
            0,
            GPIO_BASE);

    if(gpio == (void *)MAP_DEVICE_FAILED)
    {
        printf("GPIO FAILED\n");
        return -1;
    }

/* MOTOR GPIO */

    gpio[GPFSEL1] &= ~(7<<21);
    gpio[GPFSEL1] |=  (1<<21);

    gpio[GPFSEL1] &= ~(7<<24);
    gpio[GPFSEL1] |=  (1<<24);

    gpio[GPFSEL2] &= ~(7<<6);
    gpio[GPFSEL2] |=  (1<<6);

    gpio[GPFSEL2] &= ~(7<<9);
    gpio[GPFSEL2] |=  (1<<9);

/* FORWARD */

    gpio[GPSET0] = IN1 | IN3;

/* IPC */

    attach = name_attach(
                NULL,
                "CONTROL",
                0);

    printf("CONTROL PROCESS STARTED\n");

    while(1)
    {
        int rcvid;

        rcvid = MsgReceive(
                    attach->chid,
                    &data,
                    sizeof(data),
                    NULL);

        if(rcvid > 0)
        {
            if(data.distance < 20)
            {
                gpio[GPCLR0] = ENA | ENB;

                printf("MOTOR STOPPED\n");
            }
            else
            {
                gpio[GPSET0] = ENA | ENB;

                printf("MOTOR RUNNING\n");
            }

            MsgReply(rcvid,0,NULL,0);
        }
    }

    return 0;
}
