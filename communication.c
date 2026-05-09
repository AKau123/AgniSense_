#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <termios.h>
#include <stdint.h>

#include <sys/neutrino.h>

#define HC05_PORT "/dev/ser1"

int main()
{
    int fd;

    struct termios options;

/* CPU2 */

    unsigned runmask = 0x4;

    ThreadCtl(_NTO_TCTL_RUNMASK,
              (void *)(uintptr_t)runmask);

/* SERIAL OPEN */

    fd = open(HC05_PORT,
              O_RDWR | O_NOCTTY);

    if(fd == -1)
    {
        printf("UART open failed\n");
        return -1;
    }

/* UART CONFIG */

    tcgetattr(fd, &options);

    cfsetispeed(&options, B9600);
    cfsetospeed(&options, B9600);

    options.c_cflag |=
    (CLOCAL | CREAD);

    tcsetattr(fd,
              TCSANOW,
              &options);

    printf("HC05 CONNECTED\n");

/* V2V MESSAGE */

    while(1)
    {
        write(fd,
              "V2V ACTIVE\n",
              11);

        printf("V2V SENT\n");

        delay(1000);
    }

    return 0;
}
