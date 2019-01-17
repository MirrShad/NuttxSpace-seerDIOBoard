#include "ProtocolJingZhiCanopen.h"
#include "Console.h"
#include "MessageTask.h"
#include "GlobalData.h"
namespace{
	const uint8_t JINZHI_TARGET_SPEED_DATA[8] = {0x60, 0x41, 0x23};
	const uint8_t JINZHI_ACTURAL_POS_DATA[8] = {0x43, 0x64, 0x60, 0x00};
	const uint8_t JINZHI_ERR_CODE_DATA[8] = {0x43, 0x02, 0x10, 0x00};
	//const uint8_t JINZHI_POS_MODE_INIT_DATA[8] = {0x2F, 0x60, 0x60, 0x00, 0x01};
	const uint8_t JINZHI_VEL_MODE_INIT_DATA[8] = {0x2F, 0x60, 0x60, 0x00, 0x03};
	//const uint8_t JINZHI_HEART_BEAT_DATA[8] = {0x60,0xA0,0xA0,0x00,0x50,0x50,0x50,0x50};
}


CProtocolJingZhiCanopen::CProtocolJingZhiCanopen(CtrlModeEnum ctrlMode)
	:_ctrlMode(ctrlMode)
{
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
void CProtocolJingZhiCanopen::encode(uint8_t motor_idx, motorDriverCmdType ct, int32_t val, CanTxMsg& msg)
{
	seer_assert(motor_idx < Mileage::Instance()->num_of_wheel());
	msg.StdId = _motorRxId[motor_idx];
	msg.IDE = CAN_Id_Standard;
	msg.RTR = CAN_RTR_Data;
	msg.DLC = 8;
	if (DRV_CMD_TARGET_SPEED == ct)
	{
		float encoderLines = Mileage::Instance()->encoderLines();
		//int t = (val >= 0) ? 1 : -1;
		//int32_t speedVal = (int32_t)(val / (4.0f * encoderLines)  + t * 0.5f);
		int32_t speedVal = val;
		msg.Data[0] = 0x23;
		msg.Data[1] = 0x41;
		msg.Data[2] = 0x23;
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
			val >>= 8;
		}
	}else if (DRV_CMD_DRI_STATE == ct)
	{
		msg.Data[0] = 0x40;
		msg.Data[1] = 0x02;
		msg.Data[2] = 0x10;
		for(int i = 3; i < 8; i++)
		{
			msg.Data[i] = 0x00;
			val >>= 8;
		}
	}
}

/**
  * @brief  decode
	* @param  idx: index of motor
	* @param  id: protocol id of motor
	* @retval false: driver state error
  */
bool CProtocolJingZhiCanopen::decode(motorDriverCmdType& ct, int32_t& val, const CanRxMsg& msg)
{
	uint32_t tempVal = 0;
	
	if(memcmp(JINZHI_TARGET_SPEED_DATA, msg.Data, 8) == 0)
	{
			ct = 	DRV_CMD_TARGET_SPEED;
			return true; //compare finished
	}
	
	/* check for COPLEY_CAN_ACTURAL_POS */
	for(int i = 0; i < 8; i++)
	{
		if((i < 4) && JINZHI_ACTURAL_POS_DATA[i] != msg.Data[i]) //compare bytes 0-3
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
	
	/* check for COPLEY_CAN_ERR_CODE */
	for(int i = 0; i < 8; i++)
	{
		if((i < 4) && JINZHI_ERR_CODE_DATA[i] != msg.Data[i]) //compare bytes 0-3
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
			return false; //compare finished
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
int32_t CProtocolJingZhiCanopen::getMotorIndex(const CanRxMsg& Msg)
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
bool CProtocolJingZhiCanopen::getInitMsg(CanTxMsg& msg)
{
	static int8_t msgCount = Mileage::Instance()->num_of_wheel();
	if(_ctrlMode == SPEED_MODE)
		return false;
	
	//POSITION MODE
	if(msgCount > 0)
	{
		msg.IDE = CAN_Id_Standard;
		msg.RTR = CAN_RTR_Data;
		msg.DLC = 8;
		memcpy(msg.Data, JINZHI_VEL_MODE_INIT_DATA, 8);
		msg.StdId = _motorRxId[msgCount - 1];
		msgCount--;
		return true;
	}else
	{
		msgCount = Mileage::Instance()->num_of_wheel();
		return false;
	}
}

/**
  * @brief  parse error code and print
	* @param  uint32_t error code
	* @retval None
  */
void CProtocolJingZhiCanopen::parseErrCode(uint8_t motorIdx, uint32_t errCode) const
{
	if((errCode & 0xFF) > 1)
	{
		Message::Instance()->postErrMsg(CODE_MOTO_ERRO_SEE_LOG,"motor%d has fault: 0x%x",motorIdx,errCode);
	}else
	{
		Message::Instance()->clearErr(CODE_MOTO_ERRO_SEE_LOG);
	}
}

/**
  * @brief  calculate motor Rx ID
  * @param  offset of motorID
  * @retval motor Rx ID
  */
uint16_t CProtocolJingZhiCanopen::calcRxID(uint16_t ID)
{
	return 0x600 + ID;
}

/**
  * @brief  calculate motor Tx ID
  * @param  offset of motorID
  * @retval motor Tx ID
  */
uint16_t CProtocolJingZhiCanopen::calcTxID(uint16_t ID)
{
	return 0x580 + ID;
}

//end of file
