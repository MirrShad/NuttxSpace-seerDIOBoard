#include "ProtocolDeltaCanopen.h"
#include "Console.h"
#include "MessageTask.h"
namespace{
	int8_t msgCount = 0;
	const uint8_t INIT_MSG_COUNTS = 6;
	const uint8_t DELTA_CAN_POS_MODE_TARGET_SPEED_DATA[8] = {0x60, 0xff, 0x60};
	const uint8_t DELTA_CAN_ACTURAL_POS_DATA[8] = {0x43, 0x64, 0x60, 0x00, 0x00, 0x00, 0x00, 0x00};
	const uint8_t DELTA_POS_MODE_INIT_DATA[INIT_MSG_COUNTS][8] = {{0x2F, 0x60, 0x60, 0x00, 0x03, 0x00, 0x00, 0x00},
																									{0x2B, 0x40, 0x60, 0x00, 0x06, 0x00, 0x00, 0x00},
																									{0x2B, 0x40, 0x60, 0x00, 0x07, 0x00, 0x00, 0x00},
																									{0x2B, 0x40, 0x60, 0x00, 0x0F, 0x00, 0x00, 0x00},
																									{0x23, 0x83, 0x60, 0x00, 0xE8, 0x03, 0x00, 0x00},
																									{0x23, 0x84, 0x60, 0x00, 0xD0, 0x07, 0x00, 0x00}};
	const uint8_t DELTA_CAN_ERR_CODE_DATA[8] = {0x4B, 0x41, 0x60, 0x00};
}

CProtocolDeltaCANOpen::CProtocolDeltaCANOpen()
{
	isInitSend = false;
	for(int i = 0; i < Mileage::Instance()->num_of_wheel(); i++)
	{
		setMotorRxId(i, 0x601 + i);
		setMotorTxId(i, 0x581 + i);
	}
}

/**
  * @brief  encode
	* @param  idx: index of motor
	* @param  id: protocol id of motor
  * @retval None
  */
void CProtocolDeltaCANOpen::encode(uint8_t motor_idx, motorDriverCmdType ct, int32_t val, CanTxMsg& msg)
{
	seer_assert(motor_idx < Mileage::Instance()->num_of_wheel());
	msg.StdId = _motorRxId[motor_idx];
	msg.IDE = CAN_Id_Standard;
	msg.RTR = CAN_RTR_Data;
	msg.DLC = 8;

	if (false == isInitSend)
	{
		//POSITION MODE
		if(msgCount < Mileage::Instance()->num_of_wheel() * INIT_MSG_COUNTS)
		{
			memcpy(msg.Data, DELTA_POS_MODE_INIT_DATA[msgCount / Mileage::Instance()->num_of_wheel()], 8);
			msgCount++;
			Message::Instance()->postMsg("!!!!!! %x %x %x %x %x - %x" , msg.Data[0],
			msg.Data[1],
			msg.Data[2],
			msg.Data[3],
			msg.Data[4],
			msg.StdId);
		}else
		{
			msgCount = 0;
			isInitSend = true;
		}
	}
	
	if (isInitSend) {
		
		if (DRV_CMD_TARGET_SPEED == ct)
		{
			float encoderLines = Mileage::Instance()->encoderLines();
			int t = (val >= 0) ? 1 : -1;
			int32_t speedVal = (int32_t)(10.0f * val * 6.0f/(4.0f * encoderLines) + t * 0.5f);
			msg.Data[0] = 0x23;
			msg.Data[1] = 0xff;
			msg.Data[2] = 0x60;
			msg.Data[3] = 0x00;
			for(int i = 4; i < 8; i++)
			{
				msg.Data[i] = speedVal & 0xFF;
				speedVal >>= 8;
			}
		}else if (DRV_CMD_ACTURAL_POS == ct)
		{
			msg.Data[0] = 0x40;
			msg.Data[1] = 0x64;
			msg.Data[2] = 0x60;
			for(int i = 3; i < 8; i++)
			{
				msg.Data[i] = 0x00;
			}
		}else if (DRV_CMD_DRI_STATE == ct)
		{
			msg.Data[0] = 0x40;
			msg.Data[1] = 0x41;
			msg.Data[2] = 0x60;
			for(int i = 3; i < 8; i++)
			{
				msg.Data[i] = 0x00;
			}
		}
	}
}

/**
  * @brief  decode
	* @param  idx: index of motor
	* @param  id: protocol id of motor
	* @retval false: driver state error
  */
bool CProtocolDeltaCANOpen::decode(motorDriverCmdType& ct, int32_t& val, const CanRxMsg& msg)
{
	if (false == isInitSend)
	{
		ct = DRV_CMD_TARGET_SPEED;
		val = 0;
		return true;
	}
	uint32_t tempVal = 0;
	
	if(memcmp(DELTA_CAN_POS_MODE_TARGET_SPEED_DATA, msg.Data, 8) == 0)
	{
			ct = 	DRV_CMD_TARGET_SPEED;
			return true; //compare finished
	}
	
	/* check for DELTA_CAN_ACTURAL_POS */
	for(int i = 0; i < 8; i++)
	{
		if((i < 4) && DELTA_CAN_ACTURAL_POS_DATA[i] != msg.Data[i]) //compare bytes 0-3
			break;	//give up compare and jump to next for loop
		else if(i >= 4)	//copy bytes 4-7
		{
			tempVal >>= 8;
			tempVal += ((uint32_t)msg.Data[i] << 24);
		}
		if(i == 7)
		{
			ct = DRV_CMD_ACTURAL_POS;
			val = tempVal;
			return true; //compare finished
		}
	}
	
	/* check for DELTA_CAN_ERR_CODE */
	for(int i = 0; i < 8; i++)
	{
		if((i < 4) && DELTA_CAN_ERR_CODE_DATA[i] != msg.Data[i]) //compare bytes 0-3
			break;	//give up compare and jump to next for loop
		else if(i >= 4)	//copy bytes 4-7
		{
			tempVal >>= 8;
			tempVal += ((uint32_t)msg.Data[i] << 24);
		}
		if(i == 7)
		{
			ct = DRV_CMD_DRI_STATE;//use driver state to mark error code
			val = tempVal;
			return true; //compare finished
		}
	}
	
	ct = DRV_CMD_UNDEFINED;
	return true;
}

/**
  * @brief  parse Motor Index from can message
  * @param  Msg get from CAN
  * @retval ret >= 0, Motor index
	*  				ret == -1, parse failed
  */
int32_t CProtocolDeltaCANOpen::getMotorIndex(const CanRxMsg& Msg)
{
	for(int i = 0; i < CMileage::MAX_WHEEL_NUM; i++)
	{
		if(Msg.StdId == _motorTxId[i])
			return i;
	}
	return -1;
}

/**
  * @brief  getInitMessage
	* @param  InitMsg: index of motor
  * @retval still message
  */
bool CProtocolDeltaCANOpen::getInitMsg(CanTxMsg& msg)
{
	msgCount = 0;
	isInitSend = false;
	return false;
}

/**
  * @brief  parse error code and print
	* @param  uint32_t error code
	* @retval None
  */
void CProtocolDeltaCANOpen::parseErrCode(uint8_t motorIdx, uint32_t errCode) const
{
	//Console::Instance()->printf("motor%d error code: 0x%X. DELTA parse rule undefined.\r\n", motorIdx, errCode);
	if((errCode & 0x08) > 1)
	{
		Message::Instance()->postErrMsg(CODE_MOTO_ERRO_SEE_LOG,"motor%d has fault",motorIdx);
	}
}

/**
  * @brief  calculate motor Rx ID
  * @param  offset of motorID
  * @retval motor Rx ID
  */
uint16_t CProtocolDeltaCANOpen::calcRxID(uint16_t ID)
{
	return 0x600 + ID;
}

/**
  * @brief  calculate motor Tx ID
  * @param  offset of motorID
  * @retval motor Tx ID
  */
uint16_t CProtocolDeltaCANOpen::calcTxID(uint16_t ID)
{
	return 0x580 + ID;
}

//end of file
