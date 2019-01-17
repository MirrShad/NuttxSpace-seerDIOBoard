#ifndef _CHASSIS_H_
#define _CHASSIS_H_

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "Singleton.h"
#include "ProtocolCanDriver.h"

class CChassisDevice
{
public:
	int chassisCmdHandler(uint8_t* msg,uint16_t len);
	bool setProtocol(const uint8_t);
	CDriverCanProtocol* Protocol() const
	{
		return _driverProtocol;
	}
	int setStart();
private:
	uint8_t _protocol;
	CDriverCanProtocol* _driverProtocol;
};

typedef NormalSingleton<CChassisDevice> ChassisDevice;

#endif
