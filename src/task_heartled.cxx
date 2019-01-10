#include <nuttx/config.h>

#include <sys/ioctl.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <sched.h>
#include <errno.h>

#include <nuttx/leds/heartled.h>

#include <sys/boardctl.h>
#include <sys/time.h>
#include "task_heartled.h"

extern "C"
{
  int task_heartled(int argc, char *argv[])
  {
    int fd = open("/dev/heartled", O_WRONLY);
    if (fd < 0)
    {
      int errcode = errno;
      printf("led_daemon: ERROR: Failed to open %s: %d\n",
             "/dev/heartled", errcode);
      return 1;
    }

    bool bLedOn = false;
    struct timeval xTimeLast;
    struct timeval xTimeCur;
    int ret;
    for (; ; )
    {
      bLedOn = !bLedOn;
      gettimeofday(&xTimeLast, NULL);
      uint32_t ulDeltaMS;
      do{
        gettimeofday(&xTimeCur, NULL);    
        ulDeltaMS = (xTimeCur.tv_sec - xTimeLast.tv_sec) * 1000 + \
        (xTimeCur.tv_usec - xTimeLast.tv_usec) / 1000L;
      }while(ulDeltaMS < 500);
      if(bLedOn)
      {
        ret = ioctl(fd, HEARTLEDIOC_SETON, NULL);
      }
      else
      {
          ret = ioctl(fd, HEARTLEDIOC_SETOFF, NULL);
      }
      if (ret < 0)
      {
        int errcode = errno;
        printf("led_daemon: ERROR: ioctl(ULEDIOC_SUPPORTED) failed: %d\n",
                 errcode);
        return 1;
      }
    }
  }
}
