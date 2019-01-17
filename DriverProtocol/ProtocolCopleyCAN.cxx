#include "ProtocolCopleyCAN.h"
#include "ProtocolCanDriver.h"
namespace{
	const uint8_t COPLEY_CAN_VEL_MODE_TARGET_SPEED_DATA[8] = {0x60, 0x41, 0x23};
	const uint8_t COPLEY_CAN_POS_MODE_TARGET_SPEED_DATA[8] = {0x60, 0xff, 0x60};
	const uint8_t COPLEY_CAN_ACTURAL_POS_DATA[8] = {0x43, 0x64, 0x60, 0x00};
	//	const uint8_t COPLEY_CAN_ERR_CODE_DATA[8] = {0x43, 0x83, 0x21, 0x00}; //TODO: chenxx confirm
	const uint8_t COPLEY_CAN_ERR_CODE_DATA[8] = {0x43, 0x02, 0x10, 0x00};
	const uint8_t COPLEY_POS_MODE_INIT_DATA[8] = {0x2F, 0x60, 0x60, 0x00, 0x03};
}


CProtocolCopleyCAN::CProtocolCopleyCAN(CtrlModeEnum ctrlMode)
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
void CProtocolCopleyCAN::encode(uint8_t motor_idx, motorDriverCmdType ct, int32_t val, struct can_msg_s& msg)
{
	//seer_assert(motor_idx < Mileage::Instance()->num_of_wheel());
	msg.cm_hdr.ch_id = _motorRxId[motor_idx];
	msg.cm_hdr.ch_extid = 1;
	msg.cm_hdr.ch_rtr = 1;
	msg.cm_hdr.ch_dlc = 8;
	if (DRV_CMD_TARGET_SPEED == ct)
	{
		if(SPEED_MODE == _ctrlMode)
		{
			msg.cm_data[0] = 0x23;
			msg.cm_data[1] = 0x41;
			msg.cm_data[2] = 0x23;
			msg.cm_data[3] = 0x00;
		}else if(POSITION_MODE == _ctrlMode)
		{
			msg.cm_data[0] = 0x23;
			msg.cm_data[1] = 0xff;
			msg.cm_data[2] = 0x60;
			msg.cm_data[3] = 0x00;
		}
		for(int i = 4; i < 8; i++)
		{
			msg.cm_data[i] = val & 0xFF;
			val >>= 8;
		}
	}else if (DRV_CMD_ACTURAL_POS == ct)
	{
		msg.cm_data[0] = 0x40;
		msg.cm_data[1] = 0x64;
		msg.cm_data[2] = 0x60;
		for(int i = 3; i < 8; i++)
		{
			msg.cm_data[i] = 0x00;
			val >>= 8;
		}
	}else if (DRV_CMD_DRI_STATE == ct)
	{
		msg.cm_data[0] = 0x40;
		msg.cm_data[1] = 0x02;
		msg.cm_data[2] = 0x10;
		for(int i = 3; i < 8; i++)
		{
			msg.cm_data[i] = 0x00;
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
bool CProtocolCopleyCAN::decode(motorDriverCmdType& ct, int32_t& val, const struct can_msg_s& msg)
{
	uint32_t tempVal = 0;
	
	if(memcmp(COPLEY_CAN_POS_MODE_TARGET_SPEED_DATA, msg.cm_data, 8) == 0 || 
			memcmp(COPLEY_CAN_VEL_MODE_TARGET_SPEED_DATA, msg.cm_data, 8) == 0 )
	{
			ct = 	DRV_CMD_TARGET_SPEED;
			return true; //compare finished
	}
	
	/* check for COPLEY_CAN_ACTURAL_POS */
	for(int i = 0; i < 8; i++)
	{
		if((i < 4) && COPLEY_CAN_ACTURAL_POS_DATA[i] != msg.cm_data[i]) //compare bytes 0-3
			break;	//give up compare and jump to next for loop
		else if(i >= 4)	//copy bytes 4-7
		{
			tempVal >>= 8;
			tempVal += ((uint32_t)msg.cm_data[i] << 24);
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
		if((i < 4) && COPLEY_CAN_ERR_CODE_DATA[i] != msg.cm_data[i]) //compare bytes 0-3
			break;	//give up compare and jump to next for loop
		else if(i >= 4)	//copy bytes 4-7
		{
			tempVal >>= 8;
			tempVal += ((uint32_t)msg.cm_data[i] << 24);
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
int32_t CProtocolCopleyCAN::getMotorIndex(const struct can_msg_s& Msg)
{
	for(int i = 0; i < CMileage::MAX_WHEEL_NUM; i++)
	{
		if(Msg.cm_hdr.ch_id == _motorTxId[i])
			return i;
	}
	return -1;
}

/**
  * @brief  getInitMessage
	* @param  InitMsg: index of motor
  * @retval still message
  */
bool CProtocolCopleyCAN::getInitMsg(struct can_msg_s& msg)
{
	static int8_t msgCount = Mileage::Instance()->num_of_wheel();
	if(_ctrlMode == SPEED_MODE)
		return false;
	
	//POSITION MODE
	if(msgCount > 0)
	{
		msg.cm_hdr.ch_extid = 1;
		msg.cm_hdr.ch_rtr = 1;
		msg.cm_hdr.ch_dlc = 8;
		memcpy(msg.cm_data, COPLEY_POS_MODE_INIT_DATA, 8);
		msg.cm_hdr.ch_id = _motorRxId[msgCount - 1];
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
void CProtocolCopleyCAN::parseErrCode(uint8_t motorIdx, uint32_t errCode) const
{
	/*
	if ((errCode & 0x800) > 0)
	{
		GlobalControl::setChassisEmergencyButtonOn();
	}
	else
	{
		GlobalControl::setChassisEmergencyButtonOff();
	}
	if (errCode > 0)
	{
		Console::Instance()->printf("motor%d error code: 0x%X. Copley parse rule undefined.\r\n", motorIdx, errCode);
	}*/
}

/**
  * @brief  calculate motor Rx ID
  * @param  offset of motorID
  * @retval motor Rx ID
  */
uint16_t CProtocolCopleyCAN::calcRxID(uint16_t ID)
{
	return 0x600 + ID;
}

/**
  * @brief  calculate motor Tx ID
  * @param  offset of motorID
  * @retval motor Tx ID
  */
uint16_t CProtocolCopleyCAN::calcTxID(uint16_t ID)
{
	return 0x580 + ID;
}

//end of file
