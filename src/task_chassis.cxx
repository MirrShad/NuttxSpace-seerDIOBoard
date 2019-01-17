#include <nuttx/config.h>

#include <sys/ioctl.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <sched.h>
#include <errno.h>

#include <sys/time.h>
#include "task_chassis.h"
#include "chassis.h"

extern "C"
{
  int task_chassis(int argc, char *argv[])
  {
    for(;;)
    {
      printf("task chassis wait !!!!!!!!!!\r\n");
      ChassisDevice::Instance()->waitStart();
      printf("task chassis start !!!!!!!!!!\r\n");
    }
  }
}
