#ifndef _CHASSIS_H_
#define _CHASSIS_H_

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

#include "Singleton.h"
#include "ProtocolCanDriver.h"

class CChassisDevice
{
public:
	CChassisDevice();
	int chassisCmdHandler(uint8_t* msg,uint16_t len);
	bool setProtocol(const uint8_t);
	CDriverCanProtocol* Protocol() const
	{
		return _driverProtocol;
	}
	int setStart();
	void waitStart();
	void newSpeedCmd();
	void queryOdoCmd();
	int waitCmd();
	void sendSpeedCmd();
	void sendQueryCmd();
	int doInit();
private:
	int can1_fd;
	uint8_t _protocol;
	CDriverCanProtocol* _driverProtocol;
	sem_t openSem;
	sem_t cmdSem;
	bool bNewSpeedCmd;
	bool bOdoCmd;
	bool bStateCmd;
	int currentCmd;//0:null 1:speed 2:odo 3:state
	float ori_global_X;
	float ori_global_Y;
	float ori_global_W;
};

typedef NormalSingleton<CChassisDevice> ChassisDevice;

#endif
