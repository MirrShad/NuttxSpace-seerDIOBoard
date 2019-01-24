//#include "GlobalData.h"
#include "Reporter.h"
#include <stdio.h>
#include "udp.h"
#include <errno.h>
//#include <assert.h>

namespace{
	const uint16_t RPT_DESTPORT = 5002;
}

uint8_t CReporter_base::transBuff_[CReporter_base::TRANS_BUFF_LEN];
pthread_mutex_t CReporter_base::reporter_mut;

void CReporter_base::reportLock()
{
	if(socket_n < 0)
	{
		pthread_mutex_init(&reporter_mut,NULL);
	}
	while(pthread_mutex_lock(&reporter_mut)!=0)// && errno== EINTR)
	{
		printf("ERROR Reporter pthread_mutex_lock failed err: %d\n",errno);
	}
}

void CReporter_base::reportUnlock()
{
	if ((pthread_mutex_unlock(&reporter_mut)) != 0)
    {
      printf("ERROR Gyro pthread_mutex_unlock failed %d\n",errno);
    }
}

int CReporter_base::upload(uint32_t len, uint8_t* ip, uint16_t port)
{
	int ret = 0;
	socklen_t addrlen;
	if(socket_n < 0)
	{
	  destaddr.sin_family             = AF_INET;
          destaddr.sin_port               = HTONS(port);
  	  destaddr.sin_addr.s_addr        = HTONL(0xc0a8c005);//192.168.192.5

  	  addrlen                       = sizeof(struct sockaddr_in);
  	  socket_n                        = socket(PF_INET, SOCK_DGRAM, 0);
  	  if (socket_n < 0)
    	  {
      	    fprintf(stderr, "ERROR: socket() failed: %d\n", errno);
      	    return 1;
    	  }
	}

	ret = sendto(socket_n, transBuff_, len, 0, (struct sockaddr *)&destaddr, addrlen);
//	Console::Instance()->printf("rpt\r\n");
//	ret = sendto(socket_n, transBuff_, len, ip, port);
	if(ret < 0)
	{
		printf("Reporter upload failed: %d\r\n",errno);
	}

	return ret;
}

//int CReporter_base::reply(uint32_t len)
//{
//	return upload(len, (uint8_t*)GlobalData::destReplyIp, GlobalData::destReplyPort);
//}

int CReporter_base::uploadRbk(uint32_t len)
{
	
	return upload(len, (uint8_t*)'192.168.192.5', RPT_DESTPORT);
}

