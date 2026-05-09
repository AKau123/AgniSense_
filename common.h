#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>

typedef struct
{
    double distance;
    double speed;
    int emergency;

    char message[100];

} vehicle_data_t;

#endif
