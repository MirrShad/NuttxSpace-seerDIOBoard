#include <nuttx/config.h>

#include <errno.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

const uint8_t GYRO_INIT_CMD_LENGTH = 9;
const unsigned char gyro_init_data_buffer[GYRO_INIT_CMD_LENGTH] = {0xA5, 0x5A, 0x07, 0xB3, 0x01, 0x05, 0x07, 0xC7, 0xAA};	

extern "C"
{
  int task_gyro(int argc, char *argv[])
  {
    int fd;
    int i;

    bool eof = false;
    FAR char *buf;
    FAR char *devpath;

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
errout_with_buf:
  free(buf);

errout:
  fflush(stderr);
  return EXIT_FAILURE;
  }
}
