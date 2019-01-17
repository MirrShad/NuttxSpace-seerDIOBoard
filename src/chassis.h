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
private:
	uint8_t _protocol;
	CDriverCanProtocol* _driverProtocol;
	sem_t openSem;
};

typedef NormalSingleton<CChassisDevice> ChassisDevice;

#endif
