#ifndef _PROTOCOL_MOTEC_CAN_OPEN_H_
#define _PROTOCOL_MOTEC_CAN_OPEN_H_

#include "Singleton.h"
#include <stdint.h>
#include "stm32f4xx.h"
#include "ProtocolCanDriver.h"

class CProtocolMotecCANOpen
	:public CDriverCanProtocol
{
public:
	
	CProtocolMotecCANOpen();
	virtual ~CProtocolMotecCANOpen(){}	
		
	virtual int32_t getMotorIndex(const CanRxMsg&);	
	virtual void encode(uint8_t motor_idx, motorDriverCmdType ct, int32_t val, CanTxMsg&);
	virtual bool decode(motorDriverCmdType& ct, int32_t& val, const CanRxMsg&);
	virtual bool getInitMsg(CanTxMsg&);
	virtual void parseErrCode(uint8_t motorIdx, uint32_t errCode) const;
	
	virtual uint16_t calcRxID(uint16_t ID);
	virtual uint16_t calcTxID(uint16_t ID);
	
		
private:
	bool isInitSend;
};

typedef NormalSingleton<CProtocolMotecCANOpen> ProtocolMotecCANOpen;


#endif
