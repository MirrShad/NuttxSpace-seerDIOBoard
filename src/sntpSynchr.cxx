#include "sntpSynchr.h"

#include <stdlib.h>
#include "callbackhandler.h"
#include <math.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "udp.h"
#include <errno.h>
#include <stdio.h>
#include <stdint.h>
#include "Communication.h"
#include <string.h>
#include <pthread.h>
namespace{
	const uint8_t NTP_REQUEST_MSG_LEN = 48;
	uint8_t NTP_REQUEST_MSG[NTP_REQUEST_MSG_LEN] = {0x23};
	const uint16_t NTP_PORT = 123;
	const uint64_t M_1900_TO_1970 = 2208988800000000000ull;
	int socket_n = -1;
	sockaddr_in destaddr;
};

uint64_t CSntpSynchr::calcNtpTimeOffset(uint64_t t_integ, uint64_t t_decimal)
{
	uint64_t stampTime_ns_ = (t_integ * 1000000000ull + (t_decimal * 1000000000ull >> 32));
//	Console::Instance()->printf("stamptime = %lld, midstamp = %lld\r\n", stampTime_ns_/1000000000ull, midTime_ns_/1000000000ull);
	return (stampTime_ns_ - midTime_ns_);
}

int CSntpSynchr::ntpQueryHandler(uint8_t* buff, uint16_t len)
{
	if(isWaitingForReply_)
		return -1;
	
//	if(socket_n < 0)
//	{
//		sockaddr_in localaddr;
//	
//		localaddr.sin_family = AF_INET;
//		localaddr.sin_port = htons(1232);
//		localaddr.sin_addr.s_addr = INADDR_ANY;
//		
//		socket_n = net::socket(AF_INET, SOCK_DGRAM, 0);
//		net::bind(socket_n, (sockaddr *)&localaddr,  sizeof(localaddr));
//		Console::Instance()->printf("Init NTP client socket: [%d]\r\n", socket_n);
//	}

	startRecorder();
//#ifndef LAN8720DEVE
//	sendto(CW5500Device::DISPATCH_SKT, NTP_REQUEST_MSG, NTP_REQUEST_MSG_LEN, CmdSocket::Instance()->rbkIP(), NTP_PORT);
//#endif	
	int ret = 0;
	socklen_t addrlen;
	if(socket_n < 0)
	{
	  	destaddr.sin_family             = AF_INET;
    	destaddr.sin_port               = HTONS(NTP_PORT);
  	  	destaddr.sin_addr.s_addr        = HTONL(0xc0a8c005);//192.168.192.5

  	  	socket_n                        = socket(PF_INET, SOCK_DGRAM, 0);
  	  	if (socket_n < 0)
    	{
    		fprintf(stderr, "ERROR: socket() failed: %d\n", errno);
      	    return 1;
    	}
	}

	addrlen                       = sizeof(struct sockaddr_in);
	sendto(socket_n, NTP_REQUEST_MSG, NTP_REQUEST_MSG_LEN, MSG_DONTWAIT, (sockaddr *)&destaddr, sizeof(addrlen));

	return 0;
}

int CSntpSynchr::ntpCalcHandler(uint8_t* buff, uint16_t len)
{
	stopRecorder();

	if(len != NTP_REQUEST_MSG_LEN - 4)
	{
		printf("Length mismatch\r\n");
		return -1;
	}

	uint64_t t_integ1 = 0;
	uint64_t t_integ2 = 0;

	for(int i = 0; i < 8; i++)
	{
		t_integ1 <<= 8;
		t_integ1 += buff[NTP_REQUEST_MSG_LEN - 4 - 16 + i];

		t_integ2 <<= 8;
		t_integ2 += buff[NTP_REQUEST_MSG_LEN - 4 - 8 + i];
	}

	if(fabs((int64_t)(t_integ1 - t_integ2)) > 0xFFFFF)
	{
		printf("int1 != int2, in1 - in2 = %lld\r\n", abs((int64_t)(t_integ1 - t_integ2)));
	}

	uint64_t tmpNtpTimeOffset_ns = calcNtpTimeOffset( t_integ1 >> 32, t_integ1 & 0xFFFFFFFF);

	tmpNtpTimeOffset_ns -= M_1900_TO_1970;

	printf("ntpBaseOffset = %lld\r\n",tmpNtpTimeOffset_ns);
	//Message::Instance()->clearErr(CODE_NTP_SERV_NO_RESP);
	baseOffset_ns_ = tmpNtpTimeOffset_ns;

	//selfSynchrTimer_.setPeriod(NORMAL_SYNC_PERIOD);
	return 0;
}

//bool CSntpSynchr::isWaitReplyTimeout()
//{
//	if(isWaitingForReply_ && replyTimer_.isAbsoluteTimeUp())
//	{
//		isWaitingForReply_ = false;
//		return true;
//	}
//	return false;
//}

void CSntpSynchr::doInit()
{
	//selfSynchrTimer_.enable();
}

void CSntpSynchr::replyTimerCallback()
{
	stopRecorder();
	//selfSynchrTimer_.setPeriod(FAST_SYNC_PERIOD); //when NTP server loss, synchronize faster
	//Message::Instance()->postErrMsg(CODE_NTP_SERV_NO_RESP,"NTP server loss");
}

/*********************************************************
****      this part is for CSntpSynchrTask        *****
*********************************************************/
//int CSntpSynchrTask::doRun()
//{
////	if(selfSynchrTimer_.isAbsoluteTimeUp())
////	{
////		SntpSynchr::Instance()->ntpQueryHandler(NULL, 0);
////	}
////
////	if(SntpSynchr::Instance()->isWaitReplyTimeout())
////	{
////		Message::Instance()->postErrMsg(CODE_NTP_SERV_NO_RESP,"NTP server loss\r\n");
//////		if(Message::Instance()->checkErrOccur(CODE_CONT_NET_DOWN))
//////		{
//////			W5500Device::Instance()->w5500RstCommandHandler();
//////		}
////
////	}
//	return 0;
//}

//endof
