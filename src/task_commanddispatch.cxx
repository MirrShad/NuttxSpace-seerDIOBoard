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
//#include "callbackhandler.h"

/*
typedef struct
{
	uint32_t ECommand;
	CCallbackHandler_base* pHandler;
}Handler_t;
*/
namespace{
  /*
  Handler_t CommandHandlerTab[MAX_HANDLER_NUM] =
	{
		{0x00000001, NULL},
		{0x00000002, NULL},

		{0x00009999, NULL},
		{0x0000100F, NULL},	//EPrintDebugInfo
		{0x00001013, NULL},//{0x00001013, &timeSyncQueryHandler},
		{0x00001014, NULL},
		{0x00001015, NULL},
		{0x00001016, NULL},
		{0x00001017, NULL},
		{0x00001018, NULL},
		{0x00001019, NULL},
		{0x0000101A, &gyroCmdHandler},
		{0x0000101B, NULL},	//EHostErrSwitch
		{0x0000101C, NULL},	//EChassisBrakeSwitch
		{0x0000101D, NULL},	//EInApplicationProgram
		{0x0000101E, NULL},	//EResetDIOBoard
		{0x0000101F, NULL},	//ESetDO
		{0x00001020, NULL},	//EGetUid
		{0x00001021, NULL},	//EUnlock
		{0x00001022, NULL},	//EGetMainVersion
		{0x00001023, NULL},	//EGetFullVersion
		{0x00001024, NULL},	//ESetDefaultDO
		{0x00001025, NULL},	//EGetMcuNtpTime
		{0x00001026, NULL},
		{0x00001027, NULL},
		{0x00001028, NULL},
		{0x00001029, NULL},
		{0x00001031, NULL},
		{0x00001032, NULL},
		{0x00001033, NULL},
		{0x00001034, NULL},//{0x00001034, &chassisCommandHandler},

		{0x00001038, NULL},
		{0x00001039, NULL},
		{0x0000103A, NULL},
		{0x00001040, NULL},
		{0x00001041, NULL},
		{0x00001042, NULL},
		{0x00001043, NULL},
		{0x00001044, NULL},
		{0x00001045, NULL},
		{0x00001046, NULL},
		{0x00001047, NULL},
		{0x00001048, NULL},
		{0x0000104A, NULL},
		{0x0000104B, NULL},
		{0x0000104C, NULL},
		{0x00001050, NULL},
		{0x00001051, NULL},
		{0x00001052, NULL},
		{0x00001053, NULL},
		{0x00001054, NULL},
		{0x00001055, NULL},
		{0x00001056, NULL},
		{0x00001057, NULL},
		{0x00001058, NULL},
		
		{0x0000A001, &gyroCmdHandler},
		{0x0000A002, &addTaskCommandHandler},
		{0x0000A003, &forkConfigCommandHandler},
		{0x0000A004, &configChassisTypeHandler},
		{0x0000A005, &initMileageParamCommandHandler},
		{0x0000A006, NULL},
		{0x0000A007, NULL},
		{0x0000A008, NULL},	
	};
  */
}


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
