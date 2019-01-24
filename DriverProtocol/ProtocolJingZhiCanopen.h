#ifndef _PROTOCOL_JINGZHI_CAN_H_
#define _PROTOCOL_JINGZHI_CAN_H_

#include "Singleton.h"
#include <stdint.h>
#include "ProtocolCanDriver.h"
#include <nuttx/can/seer_can.h>
#include "ChassisCmdType.h"

class CProtocolJingZhiCanopen
	:public CDriverCanProtocol
{
public:
	enum CtrlModeEnum
	{
		SPEED_MODE = 0,
		POSITION_MODE
	};
	
	CProtocolJingZhiCanopen(CtrlModeEnum ctrlMode = SPEED_MODE);
	virtual ~CProtocolJingZhiCanopen(){}
		
	virtual int32_t getMotorIndex(const struct can_msg_s&);	
	virtual void encode(uint8_t motor_idx, motorDriverCmdType ct, int32_t val, struct can_msg_s&);
	virtual bool decode(motorDriverCmdType& ct, int32_t& val, const struct can_msg_s&);
	virtual bool getInitMsg(struct can_msg_s& msg);
	virtual void parseErrCode(uint8_t motorIdx, uint32_t errCode) const;
		
	virtual uint16_t calcRxID(uint16_t ID);
	virtual uint16_t calcTxID(uint16_t ID);
		
private:
	const CtrlModeEnum _ctrlMode;
};

typedef NormalSingleton<CProtocolJingZhiCanopen> ProtocolJingZhiCanopen;

#endif
