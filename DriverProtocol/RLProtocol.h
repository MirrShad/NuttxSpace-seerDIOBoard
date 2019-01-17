#ifndef __ROCKET_LAUNCHER_PROTOCOL_H__
#define __ROCKET_LAUNCHER_PROTOCOL_H__

#include "Singleton.h"

#include "stdint.h"
#include "ProtocolCanDriver.h"

class CProtocolRL
	:public CDriverCanProtocol
{
public:
	CProtocolRL(){}
	virtual ~CProtocolRL(){}	
	
	virtual int32_t getMotorIndex(const CanRxMsg&);	
	virtual void encode(uint8_t motor_idx, motorDriverCmdType ct, int32_t val, CanTxMsg&);
	virtual bool decode(motorDriverCmdType& ct, int32_t& val, const CanRxMsg&);
	virtual void parseErrCode(uint8_t motorIdx, uint32_t) const;

	virtual bool isEmsPressed(uint32_t errCode) const;
		
	virtual uint16_t calcRxID(uint16_t ID);
	virtual uint16_t calcTxID(uint16_t ID);

private:
	void int2byte(int32_t val, uint8_t& a, uint8_t& b, uint8_t& c, uint8_t& d);
	void byte2int(uint8_t a, uint8_t b, uint8_t c, uint8_t d, int32_t& val);
};

typedef NormalSingleton<CProtocolRL> ProtocolRL;

#endif
