//#include "GlobalData.h"
#include "Reporter.h"
#include <stdio.h>
#include "udp.h"
#include <errno.h>
//#include <assert.h>

namespace{
	int socket_n = -1;
	const uint16_t RPT_DESTPORT = 5002;
	struct sockaddr_in destaddr;
	struct sockaddr_in localaddr;
}

uint8_t CReporter_base::transBuff_[CReporter_base::TRANS_BUFF_LEN];
bool CReporter_base::isInitialized_ = false;


int CReporter_base::upload(uint32_t len, uint8_t* ip, uint16_t port)
{
	int ret = 0;
	socklen_t addrlen;
	if(socket_n < 0)
	{
		socket_n = socket(AF_INET, SOCK_DGRAM, 0);
		if (socket_n < 0)
		{
			printf("upload failed create client socket failure %d\n", errno);
			return -1;
		}

		localaddr.sin_family = AF_INET;
		localaddr.sin_port = HTONS(56789);
		localaddr.sin_addr.s_addr = HTONL(INADDR_ANY);
		addrlen              = sizeof(struct sockaddr_in);
		
	}
	
	destaddr.sin_family =  AF_INET;
	destaddr.sin_port = HTONS(port);
	destaddr.sin_addr.s_addr = HTONL(0xc0a8c005);//192.168.192.5///destaddr.sin_addr.s_addr = byteston(ip);
	addrlen = sizeof(struct sockaddr_in);
	ret = sendto(socket_n, transBuff_, len, 0, (struct sockaddr *)&destaddr, addrlen);
//	Console::Instance()->printf("rpt\r\n");
//	ret = sendto(socket_n, transBuff_, len, ip, port);
	if(ret < 0)
	{
		printf("Reporter upload failed\r\n");
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
