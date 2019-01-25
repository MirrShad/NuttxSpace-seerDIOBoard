#ifndef SNTP_SYNCHR_H
#define SNTP_SYNCHR_H


#include "Singleton.h"
#include <sys/time.h>

class CSntpSynchr
{
	public:
		CSntpSynchr()
			//replyTimer_(this, &CSntpSynchr::replyTimerCallback, 200),
			//selfSynchrTimer_(this, &CSntpSynchr::selfSynchrTimerCallback, FAST_SYNC_PERIOD)
			{}
		uint64_t getNtpBaseOffset_ns() {return baseOffset_ns_;}

		uint64_t calcNtpTimeOffset(uint64_t t_int, uint64_t t_decimal);

		void doInit();

		enum{
			NTP_PORT = 123,
			NORMAL_SYNC_PERIOD = 30000,
			FAST_SYNC_PERIOD = 1000
		};

		int ntpQueryHandler(uint8_t* buff=(uint8_t*)0, uint16_t len=0);
		int ntpCalcHandler(uint8_t* buff, uint16_t len);
		void selfSynchrTimerCallback() { ntpQueryHandler(); }
		void replyTimerCallback();

		uint64_t getDeltaTime_ns() {return deltaTime_ns_;}
		//bool isWaitReplyTimeout();

	private:
		inline void startRecorder()
		{
			//if a request is already sent,
			//give up this request
			if(isWaitingForReply_)
				return;

			isWaitingForReply_ = true;
			//replyTimer_.enable();
			//BaseTimer::Instance()->getTime_ns(&startTime_ns_);
			gettimeofday(&startTime_ns_, NULL);
		}

		inline void stopRecorder()
		{
			struct timeval endTime_ns_;
			//BaseTimer::Instance()->getTime_ns(&endTime_ns_);
			gettimeofday(&endTime_ns_, NULL);
			midTime_ns_ = (((endTime_ns_.tv_sec - startTime_ns_.tv_sec) * 1000000000L \
			+ (endTime_ns_.tv_usec - startTime_ns_.tv_usec) * 1000L) >> 1);
			isWaitingForReply_ = false;
			//replyTimer_.disable();
		}

		struct timeval startTime_ns_;
		uint64_t midTime_ns_;
		uint64_t deltaTime_ns_;
		uint64_t baseOffset_ns_;
		bool isWaitingForReply_;
};

//class CSntpSynchrTask: public CTask
//{
//
//public:
//	CSntpSynchrTask():CTask(NAMECODE_SntpSynchrTask), selfSynchrTimer_(5000, 30000)
//	{
//
//	}
//	virtual int doRun();
//
//private:
//	Timer selfSynchrTimer_;
//
//
//};

typedef NormalSingleton < CSntpSynchr > SntpSynchr;
#endif
//end of file
