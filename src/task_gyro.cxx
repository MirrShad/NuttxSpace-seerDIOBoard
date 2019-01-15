#include <nuttx/config.h>

#include <errno.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "Gyro.h"

extern "C"
{
  int task_gyro(int argc, char *argv[])
  {
    //CGyroDevice gyro;
/*  
    int fd;
    int i;

    //FAR char *buf;
    FAR char *devpath;
    char *buf;
    devpath = "/dev/ttyS1";
    
    printf("Task Gyro start\r\n");
   
    buf = (FAR char *)malloc(11520);
    if (buf == NULL)
    {
      fprintf(stderr, "GYRO ERROR: malloc failed: %d\n", errno);
      goto errout;
    }

    printf("Task Gyro open dev\r\n");
    fd = open(devpath, O_RDWR);
    if (fd < 0)
    {
      fprintf(stderr, "GYRO ERROR: open failed: %d\n", errno);
      goto errout_with_buf;
    }

    printf("Reading from %s\n", devpath);
    fflush(stdout);
  
    
    if(write(fd,gyro_init_data_buffer,GYRO_INIT_CMD_LENGTH)<0)
    {
      fprintf(stderr, "GYRO ERROR: gyro write failed: %d\n", errno);
    }

    for(;;)
    {
      ssize_t n = read(fd, buf, 11520);
      if (n == 0)
      {
        printf("read 0 bytes\r\n");
      }
      else if (n < 0)
      {
        printf("read failed: %d\n", errno);
        fflush(stdout);
      }
      else
      {
        for (i = 0; i < (int)n; i++)
        {
          //printf("get %d 0x%x \r\n", i, buf[i]);
	        GyroDevice::Instance()->Decode_frame(buf[i]);
        }
        write(fd,buf,n);
        fflush(stdout);
      }    
    }
*/

  if(GyroDevice::Instance()->doInit()==1) goto errout;
  while(true)
    GyroDevice::Instance()->rcvFrame();

errout_with_buf:
  //free(buf);

errout:
  //fflush(stderr);
  return EXIT_FAILURE;
  }
}
