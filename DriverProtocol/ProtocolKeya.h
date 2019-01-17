#ifndef _PROTOCOL_KEYA_H_
#define _PROTOCOL_KEYA_H_

#include "Singleton.h"
#include <stdint.h>
#include "stm32f4xx.h"
#include "ProtocolCanDriver.h"

class CProtocolKeya
	:public CDriverCanProtocol
{
public:
	CProtocolKeya();
	virtual ~CProtocolKeya(){}	
		
	virtual int32_t getMotorIndex(const CanRxMsg&);	
	virtual void encode(uint8_t motor_idx, motorDriverCmdType ct, int32_t val, CanTxMsg&);
	virtual bool decode(motorDriverCmdType& ct, int32_t& val, const CanRxMsg&);
		
	virtual uint16_t calcRxID(uint16_t ID);
	virtual uint16_t calcTxID(uint16_t ID);
};

#endif
