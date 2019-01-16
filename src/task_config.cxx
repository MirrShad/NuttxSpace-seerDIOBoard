#include <nuttx/config.h>

#include <sys/socket.h>
#include <netinet/in.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#include <arpa/inet.h>

#include "task_config.h"
#include "udp.h"

namespace{
    uint8_t destip[4];
	uint16_t destport;
}

extern "C"
{
    int task_config(int argc, char *argv[])
    {
        struct sockaddr_in server5003;
        struct sockaddr_in client5003;
        in_addr_t tmpaddr;

        socklen_t addrlen;
        socklen_t recvlen;
        int sockfd;
        int nbytes;
        int optval;
        unsigned char _configBuffer[1024];
    
        printf("Start config task\r\n");
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

        server5003.sin_family      = AF_INET;
        server5003.sin_port        = HTONS(5003);
        server5003.sin_addr.s_addr = HTONL(INADDR_ANY);

        addrlen                = sizeof(struct sockaddr_in);
    
        if (bind(sockfd, (struct sockaddr*)&server5003, addrlen) < 0)
        {
            printf("server: bind failure: %d\n", errno);
            exit(1);
        }

        for(;;)
        {
            nbytes = recvfrom(sockfd, _configBuffer, 1024, 0,
                        (struct sockaddr*)&client5003, &recvlen);
            if (nbytes < 0)
            {
                printf("config task server recv failed: %d\n", errno);
                close(sockfd);
                exit(-1);
            }

            int32_t int_ct;
            memcpy(&int_ct,_configBuffer,sizeof(int_ct));
            printf("get Num 0x%x\r\n",int_ct);
            nbytes = sendto(sockfd, _configBuffer, sizeof(int_ct), 0, (sockaddr *)&client5003, recvlen);
        }

    }
}