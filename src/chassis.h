#ifndef _CHASSIS_H_
#define _CHASSIS_H_

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "Singleton.h"

class CChassisDevice
{
public:
	int chassisCmdHandler(uint8_t* msg,uint16_t len);
};

typedef NormalSingleton<CChassisDevice> ChassisDevice;

#endif
