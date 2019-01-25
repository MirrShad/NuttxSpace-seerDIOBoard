#include "DIO.h"
#include "task_DIO.h"
#include <nuttx/config.h>

#include <sys/ioctl.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <sched.h>
#include <errno.h>

extern "C"
{
    int task_DIO(int argc, char *argv[])
    {
        for(;;)
        {
            DIO::Instance()->report();
            usleep(100*1000L);
        }
    }
}
