#include "ProtocolKincoCanOpen.h"
#include "Console.h"
#include "MessageTask.h"
namespace{
	const uint8_t INIT_MSG_COUNTS = 4;
	const uint8_t KINCO_CAN_POS_MODE_TARGET_SPEED_DATA[8] = {0x60, 0xff, 0x60};
	const uint8_t KINCO_CAN_ACTURAL_POS_DATA[8] = {0x43, 0x63, 0x60, 0x00, 0x00, 0x00, 0x00, 0x00};
	const uint8_t KINCO_POS_MODE_INIT_DATA[INIT_MSG_COUNTS][8] = {  {0x2F, 0x60, 0x60, 0x00, 0x03, 0x00, 0x00, 0x00},
																	{0x2B, 0x40, 0x60, 0x00, 0x0F, 0x00, 0x00, 0x00},
																	{0x2B, 0x40, 0x60, 0x00, 0x2F, 0x00, 0x00, 0x00},
																	{0x2B, 0x40, 0x60, 0x00, 0x3F, 0x00, 0x00, 0x00}};
	const uint8_t KINCO_CAN_ERR_CODE_DATA[8] = {0x4B, 0x41, 0x60, 0x00};
}

CProtocolKincoCANOpen::CProtocolKincoCANOpen()
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
void CProtocolKincoCANOpen::encode(uint8_t motor_idx, motorDriverCmdType ct, int32_t val, CanTxMsg& msg)
{
	seer_assert(motor_idx < Mileage::Instance()->num_of_wheel());
	msg.StdId = _motorRxId[motor_idx];
	msg.IDE = CAN_Id_Standard;
	msg.RTR = CAN_RTR_Data;
	msg.DLC = 8;
	if (DRV_CMD_TARGET_SPEED == ct)
	{
		//todo: unit:dec!!!!!!!!
		float encoderLines = Mileage::Instance()->encoderLines();
		int t = (val >= 0) ? 1 : -1;
		//speedVal = rpm*512*encodeslines/1875
		int32_t speedVal = (int32_t)(val * 1.6384 + t * 0.5f);
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
		msg.Data[1] = 0x63;
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

/**
  * @brief  decode
	* @param  idx: index of motor
	* @param  id: protocol id of motor
	* @retval false: driver state error
  */
bool CProtocolKincoCANOpen::decode(motorDriverCmdType& ct, int32_t& val, const CanRxMsg& msg)
{
	uint32_t tempVal = 0;
	
	if(memcmp(KINCO_CAN_POS_MODE_TARGET_SPEED_DATA, msg.Data, 3) == 0 )
	{
			ct = 	DRV_CMD_TARGET_SPEED;
			return true; //compare finished
	}
	
	/* check for KINCO_CAN_ACTURAL_POS */
	for(int i = 0; i < 8; i++)
	{
		if((i < 4) && KINCO_CAN_ACTURAL_POS_DATA[i] != msg.Data[i]) //compare bytes 0-3
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
	
	/* check for KINCO_CAN_ERR_CODE */
	for(int i = 0; i < 8; i++)
	{
		if((i < 4) && KINCO_CAN_ERR_CODE_DATA[i] != msg.Data[i]) //compare bytes 0-3
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
int32_t CProtocolKincoCANOpen::getMotorIndex(const CanRxMsg& Msg)
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
bool CProtocolKincoCANOpen::getInitMsg(CanTxMsg& msg)
{
	static int8_t msgCount = (Mileage::Instance()->num_of_wheel()*INIT_MSG_COUNTS);

	//POSITION MODE
	if(msgCount > 0)
	{
		msg.IDE = CAN_Id_Standard;
		msg.RTR = CAN_RTR_Data;
		msg.DLC = 8;
		int8_t currentMsg = (Mileage::Instance()->num_of_wheel()*INIT_MSG_COUNTS)-msgCount;
		memcpy(msg.Data, KINCO_POS_MODE_INIT_DATA[currentMsg%INIT_MSG_COUNTS], 8);
		msg.StdId = _motorRxId[uint8_t(currentMsg/INIT_MSG_COUNTS)];
		msgCount--;
		return true;
	}else
	{
		msgCount = Mileage::Instance()->num_of_wheel()*INIT_MSG_COUNTS;
		return false;
	}
}

/**
  * @brief  parse error code and print
  * @param  uint32_t error code
  * @retval None
  */
void CProtocolKincoCANOpen::parseErrCode(uint8_t motorIdx, uint32_t errCode) const
{
	//Console::Instance()->printf("motor%d error code: 0x%X. DELTA parse rule undefined.\r\n", motorIdx, errCode);
	if((errCode & 0x08) > 1)
	{
		Message::Instance()->postErrMsg(CODE_MOTO_ERRO_SEE_LOG, "motor%d has fault",motorIdx);
	}
}
/**
  * @brief  calculate motor Rx ID
  * @param  offset of motorID
  * @retval motor Rx ID
  */
uint16_t CProtocolKincoCANOpen::calcRxID(uint16_t ID)
{
	return 0x600 + ID;
}

/**
  * @brief  calculate motor Tx ID
  * @param  offset of motorID
  * @retval motor Tx ID
  */
uint16_t CProtocolKincoCANOpen::calcTxID(uint16_t ID)
{
	return 0x580 + ID;
}

