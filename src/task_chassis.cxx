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
    printf("task chassis wait !!!!!!!!!!\r\n");
    ChassisDevice::Instance()->waitStart();
    printf("task chassis start !!!!!!!!!!\r\n");
    for(;;)
    {
      int tempCmd = ChassisDevice::Instance()->waitCmd();
      if(1 == tempCmd)
        ChassisDevice::Instance()->sendSpeedCmd();//send speed cmd
      else if(2 == tempCmd)
        ;//query odo cmd
      /*else if(3 == tempCmd)
        ;//query state cmd*/
    }
  }
}
