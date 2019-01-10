#include <nuttx/config.h>

#include <sys/socket.h>
#include <netinet/in.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#include <arpa/inet.h>

#include "task_commanddispatch.h"
#include "udp.h"
extern "C"
{
  int task_commanddispatch(int argc, char *argv[])
  {
    struct sockaddr_in server;
    struct sockaddr_in client;
    in_addr_t tmpaddr;

    unsigned char inbuf[1024];
    socklen_t addrlen;
    socklen_t recvlen;
    int sockfd;
    int nbytes;
    int optval;
    int offset;

    printf("Start command dispatch task\r\n");
    /* Create a new UDP socket */

    sockfd = socket(PF_INETX, SOCK_DGRAM, 0);
    if (sockfd < 0)
    {
      printf("server: socket failure: %d\n", errno);
      exit(1);
    }

    /* Set socket to reuse address */

    optval = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (void*)&optval, sizeof(int)) < 0)
    {
      printf("server: setsockopt SO_REUSEADDR failure: %d\n", errno);
      exit(1);
    }

    server.sin_family      = AF_INET;
    server.sin_port        = HTONS(15003);
    server.sin_addr.s_addr = HTONL(INADDR_ANY);

    addrlen                = sizeof(struct sockaddr_in);
    
    if (bind(sockfd, (struct sockaddr*)&server, addrlen) < 0)
    {
      printf("server: bind failure: %d\n", errno);
      exit(1);
    }

    for(;;)
    {
      printf("wait a socket to come\r\n");
      nbytes = recvfrom(sockfd, inbuf, 1024, 0,
                        (struct sockaddr*)&client, &recvlen);
      if (nbytes < 0)
      {
        printf("server: %d. recv failed: %d\n", offset, errno);
        close(sockfd);
        exit(-1);
      }

      int32_t int_ct;
      memcpy(&int_ct,inbuf,sizeof(int_ct));
      printf("receive cmd %d",int_ct);
    }
  }
}
