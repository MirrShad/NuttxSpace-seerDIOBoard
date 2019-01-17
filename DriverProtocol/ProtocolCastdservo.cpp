#include "ProtocolCastdservo.h"
#include "Console.h"

#define CASTD_QUIRY_CMD ((uint8_t)(0x01))
#define CASTD_WRITE_CMD ((uint8_t)(0x02))
#define CASTD_CTRL_CMD	((uint8_t)(0x03))
#define CASTD_QUIRY_POS ((uint8_t)(0x01))
#define CASTD_QUIRY_STATE ((uint8_t)(0x00))
#define CASTD_WRITE_VEL ((uint8_t)(0x08)) //ask!!!

enum CastdservoStatusEnum
{
	STATUS_StandBy		=	0x00000000,
	STATUS_Running		=	0x00000001,
	STATUS_OffLine		=	0x00000002,
	STATUS_FindOPoint	=	0x00002000,
	STATUS_OPointOK 	=	0x00004000,
	STATUS_OPointTrig	= 0x00008000,
	STATUS_PosiEnd 		=	0x00040000,
	STATUS_PosiErr 		=	0x00080000,
	STATUS_Emergency	=	0x00100000,
	STATUS_DrvAlarm 	=	0x00200000, 
	STATUS_SeriousErr = 0x00800000 
};

enum CastdservoReportEnum
{
	MsgReport_StandBy  		= 0x01,
	MsgReport_Running 		= 0x02,
	MsgReport_OffLine 		= 0x03,
	MsgReport_OPointTrig 	= 0x07,
	MsgReport_PositionEnd	= 0x06,
	MsgReport_OPointErr 	= 0x82,
	MsgReport_PositionErr = 0x83,
	MsgReport_Emergency 	= 0x84,
	MsgReport_SeriousErr 	= 0x85,
	MsgReport_DrvAlarm 		= 0x86
};

/**
  * @brief  Constructor
  * @param  None
  * @retval None
  */
CProtocolCastdservo::CProtocolCastdservo()
{
	for(int i = 0; i < Mileage::Instance()->num_of_wheel(); i++)
	{
		setMotorRxId(i, i + 1);
		
		setMotorTxId(i, 0xFE);
	}
}

/**
  * @brief  parse Motor Index from can message
  * @param  Msg get from CAN
  * @retval ret >= 0, Motor index
  *  		ret == -1, parse failed
  */
int32_t CProtocolCastdservo::getMotorIndex(const CanRxMsg& Msg)
{
	for(int i = 0; i < CMileage::MAX_WHEEL_NUM; i++)
	{
		if(Msg.Data[2] == _motorRxId[i])
			return i;
	}
	return -1;
}

/**
  * @brief  encode
  * @param  idx: index of motor
  * @param  id: protocol id of motor
  * @retval None
  */
void CProtocolCastdservo::encode(uint8_t motor_idx, motorDriverCmdType ct, int32_t val, CanTxMsg& msg)
{
	seer_assert(motor_idx < Mileage::Instance()->num_of_wheel());
	msg.StdId = _motorRxId[motor_idx];
	msg.IDE = CAN_Id_Standard;
	msg.RTR = CAN_RTR_Data;
	msg.DLC = 8;
	
	if(DRV_CMD_TARGET_SPEED == ct)
	{
		float encoderLines = Mileage::Instance()->encoderLines();
		val = (int32_t)(val*6000.0f/(4.0f * encoderLines) + 0.5f);
		
		msg.Data[0] = CASTD_CTRL_CMD;
		msg.Data[1] = CASTD_WRITE_VEL;//TODO ask castdservo for speed cmd
	}else if(DRV_CMD_ACTURAL_POS == ct)
	{
		msg.Data[0] = CASTD_QUIRY_CMD;
		msg.Data[1] = CASTD_QUIRY_POS;
	}else if(DRV_CMD_DRI_STATE == ct)
	{
		msg.Data[0] = CASTD_QUIRY_CMD;
		msg.Data[1] = CASTD_QUIRY_STATE;//TODO ask castdservo for speed cmd
	}
	
	msg.Data[2] = 0x00;
	msg.Data[3] = (val & 0xff);
	msg.Data[4] = ((val >> 8)& 0xff);
	msg.Data[5] = ((val >> 16)& 0xff);
	msg.Data[6] = ((val >> 24)& 0xff);
	msg.Data[7] = 0x00;
}

/**
  * @brief  decode
	* @param  idx: index of motor
	* @param  id: protocol id of motor
	* @retval false: driver state error
  */
bool CProtocolCastdservo::decode(motorDriverCmdType& ct, int32_t& val, const CanRxMsg& msg)
{
	val = 0;
	for(int i = 6; i > 2; i--)
	{
		val <<= 8;
		val += msg.Data[i];
	}
	if(CASTD_CTRL_CMD == msg.Data[0] && CASTD_WRITE_VEL == msg.Data[1])
	{
		ct = DRV_CMD_TARGET_SPEED;
	}else if(CASTD_QUIRY_CMD == msg.Data[0] && CASTD_QUIRY_POS == msg.Data[1])
	{
		ct = DRV_CMD_ACTURAL_POS;
	}else if(CASTD_QUIRY_CMD == msg.Data[0] && CASTD_QUIRY_STATE == msg.Data[1])
	{
		ct = DRV_CMD_DRI_STATE;
		if((val & (STATUS_PosiErr | STATUS_Emergency | STATUS_DrvAlarm | STATUS_SeriousErr)) > 0)
		return false;
	}else
	{
		ct = DRV_CMD_UNDEFINED;
	}
	
	if((msg.Data[7] & 0x80) > 0)
		return false;	
	
	else 
		return true;
}

/**
  * @brief  parse error code and print
	* @param  uint32_t error code
	* @retval None
  */
typedef struct 
{
	const char* errStr;
	uint32_t errCode;
}enum_str;
enum_str castedServoErrTable[11] = 
{
	{ "STATUS_StandBy",			0x00000000 },
	{ "STATUS_Running",			0x00000001 },
	{ "STATUS_OffLine",			0x00000002 },
	{ "STATUS_FindOPoint",		0x00002000 },
	{ "STATUS_OPointOK",		0x00004000 },
	{ "STATUS_OPointTrig",		0x00008000 },
	{ "STATUS_PosiEnd",			0x00040000 },
	{ "STATUS_PosiErr",			0x00080000 },
	{ "STATUS_Emergency",		0x00100000 },
	{ "STATUS_DrvAlarm",		0x00200000 },
	{ "STATUS_SeriousErr",		0x00800000 },	
};
void CProtocolCastdservo::parseErrCode(uint8_t motorIdx, uint32_t errCode) const
{
	for(int i = 0; i < 11; i++)
	{
		if(errCode & castedServoErrTable[i].errCode)
		{
				Console::Instance()->printf("motor%d %s.\r\n", motorIdx, castedServoErrTable[i].errStr);
		}
	}	
}

/**
  * @brief  calculate motor Rx ID
  * @param  offset of motorID
  * @retval motor Rx ID
  */
uint16_t CProtocolCastdservo::calcRxID(uint16_t ID)
{
	return 0x000 + ID;
}

/**
  * @brief  calculate motor Tx ID
  * @param  offset of motorID
  * @retval motor Tx ID
  */
uint16_t CProtocolCastdservo::calcTxID(uint16_t ID)
{
	return 0x010 + ID;
}

//end of file
