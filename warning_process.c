#include <stdio.h>
#include <stdint.h>
#include <unistd.h>

#include <sys/mman.h>
#include <sys/neutrino.h>

#define GPIO_BASE 0xFE200000
#define BLOCK_SIZE 4096

#define GPFSEL2 (0x08/4)

#define GPSET0  (0x1C/4)
#define GPCLR0  (0x28/4)

/* WARNING LIGHTS */

#define WHITE_LED (1<<20)
#define RED_LED   (1<<21)

volatile uint32_t *gpio;

int main()
{
    ThreadCtl(_NTO_TCTL_IO,0);

/* CPU1 */

    unsigned runmask = 0x2;

    ThreadCtl(_NTO_TCTL_RUNMASK,
              (void *)(uintptr_t)runmask);

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
        printf("GPIO mmap failed\n");
        return -1;
    }

/* WHITE LED OUTPUT */

    gpio[GPFSEL2] &= ~(7<<0);
    gpio[GPFSEL2] |=  (1<<0);

/* RED LED OUTPUT */

    gpio[GPFSEL2] &= ~(7<<3);
    gpio[GPFSEL2] |=  (1<<3);
/* WARNING PATTERN */
    while(1)
    {
        gpio[GPSET0] = WHITE_LED;
        delay(500);
        gpio[GPCLR0] = WHITE_LED;
        gpio[GPSET0] = RED_LED;
        delay(500);
        gpio[GPCLR0] = RED_LED;
    }
    return 0;
}
