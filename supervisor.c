#include "common.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/neutrino.h>
#include <sys/dispatch.h>

int main()
{
    name_attach_t *attach;

    vehicle_data_t data;

    attach = name_attach(NULL,
                         "SUPERVISOR",
                         0);

    if(attach == NULL)
    {
        printf("name_attach failed\n");
        return -1;
    }

    printf("SUPERVISOR STARTED\n");

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
            printf(
            "MODE:%s | DIST:%.2f cm | SPEED:%.2f km/hr | MSG:%s\n",
            data.emergency ?
            "AMBULANCE" : "SDV",
            data.distance,
            data.speed,
            data.message);

            MsgReply(rcvid,
                     0,
                     NULL,
                     0);
        }
    }

    return 0;
}
