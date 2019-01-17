#ifndef _PROTOCOL_JINZHI_CAN_H_
#define _PROTOCOL_JINZHI_CAN_H_

#include "Singleton.h"
#include <stdint.h>
#include "stm32f4xx.h"
#include "ProtocolCanDriver.h"

class CProtocolMingZhiCanopen
	:public CDriverCanProtocol
{
public:
	enum CtrlModeEnum
	{
		SPEED_MODE = 0,
		POSITION_MODE
	};
	
	CProtocolMingZhiCanopen(CtrlModeEnum ctrlMode = SPEED_MODE);
	virtual ~CProtocolMingZhiCanopen(){}
		
	virtual int32_t getMotorIndex(const CanRxMsg&);	
	virtual void encode(uint8_t motor_idx, motorDriverCmdType ct, int32_t val, CanTxMsg&);
	virtual bool decode(motorDriverCmdType& ct, int32_t& val, const CanRxMsg&);
	virtual bool getInitMsg(CanTxMsg&);
	virtual void parseErrCode(uint8_t motorIdx, uint32_t errCode) const;
	
	virtual uint16_t calcRxID(uint16_t ID);
	virtual uint16_t calcTxID(uint16_t ID);
	void resetInitSend(void);
		
private:
	const CtrlModeEnum _ctrlMode;
};

typedef NormalSingleton<CProtocolMingZhiCanopen> ProtocolMingZhiCanopen;

#endif
