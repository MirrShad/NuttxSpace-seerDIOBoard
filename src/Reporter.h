#ifndef _FW_REPORTER_H_
#define _FW_REPORTER_H_

#include <stdint.h>
#include "Communication.h"
#include <string.h>
#include <pthread.h>

class CReporter_base
{
public:
	CReporter_base(){}
	enum {TRANS_BUFF_LEN = 300};
	/*inline void setTimestampe()
	{
		BaseTimer::Instance()->getTime_ns(&timestamp_nsec_);
	}*/
	
	void reportLock();
	void reportUnlock();
	int upload(uint32_t len, uint8_t* ip, uint16_t port);
	int uploadRbk(uint32_t len);
//	int reply(uint32_t len);
	
protected:
	uint64_t timestamp_nsec_;
	static uint8_t transBuff_[TRANS_BUFF_LEN];

private: 
	static bool isInitialized_;
	static int socket_n;
	static pthread_mutex_t reporter_mut;
};


#endif
