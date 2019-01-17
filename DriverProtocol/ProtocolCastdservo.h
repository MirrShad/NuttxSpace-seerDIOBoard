#ifndef _PROTOCOL_CASTDSERVO_H_
#define _PROTOCOL_CASTDSERVO_H_

#include "Singleton.h"
#include <stdint.h>
#include "Mileage.h"
#include "stm32f4xx.h"
#include "ChassisDriver.h"
#include "ProtocolCanDriver.h"
#include "Console.h"
												
class CProtocolCastdservo
	:public CDriverCanProtocol
{
public:
	CProtocolCastdservo();

	virtual int32_t getMotorIndex(const CanRxMsg&);	
	virtual void encode(uint8_t motor_idx, motorDriverCmdType ct, int32_t val, CanTxMsg&);
	virtual bool decode(motorDriverCmdType& ct, int32_t& val, const CanRxMsg&);
	virtual void parseErrCode(uint8_t motorIdx, uint32_t errCode) const;
	
	virtual uint16_t calcRxID(uint16_t ID);
	virtual uint16_t calcTxID(uint16_t ID);
	
};

typedef NormalSingleton<CProtocolCastdservo> ProtocolCastdservo;

#endif
//end of file
