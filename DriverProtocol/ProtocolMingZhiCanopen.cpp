#include "ProtocolMingZhiCanopen.h"
#include "Console.h"
#include "MessageTask.h"
#include "GlobalData.h"
namespace{
	const uint8_t MINGZHI_TARGET_SPEED_DATA[8] = {0x60, 0xff, 0x60};
	const uint8_t MINGZHI_ACTURAL_POS_DATA[8] = {0x43, 0x0a, 0x70, 0x00};
	const uint8_t MINGZHI_ERR_CODE_DATA[8] = {0x4B, 0x41, 0x60, 0x00};
	const uint8_t MINGZHI_INIT_MSG_COUNTS = 5;
	bool isInitSend;
	static int8_t msgCount = (Mileage::Instance()->num_of_wheel() * MINGZHI_INIT_MSG_COUNTS);
	const uint8_t MINGZHI_VEL_MODE_INIT_DATA[MINGZHI_INIT_MSG_COUNTS][8] = {{0x2B, 0x40, 0x60, 0x00, 0x06, 0x00, 0x00, 0x00},
																			{0x2B, 0x40, 0x60, 0x00, 0x07, 0x00, 0x00, 0x00},
																			{0x2B, 0x40, 0x60, 0x00, 0x0F, 0x00, 0x00, 0x00},
																			{0x2F, 0x60, 0x60, 0x00, 0x03, 0x00, 0x00, 0x00},
																			{0x40, 0x41, 0x60, 0x00, 0x00, 0x00, 0x00, 0x00}
																			};
}

CProtocolMingZhiCanopen::CProtocolMingZhiCanopen(CtrlModeEnum ctrlMode)
	:_ctrlMode(ctrlMode)
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
void CProtocolMingZhiCanopen::encode(uint8_t motor_idx, motorDriverCmdType ct, int32_t val, CanTxMsg& msg)
{
	
	
	seer_assert(motor_idx < Mileage::Instance()->num_of_wheel());	
	msg.StdId = _motorRxId[motor_idx];
	msg.IDE = CAN_Id_Standard;
	msg.RTR = CAN_RTR_Data;
	msg.DLC = 8;
	if (DRV_CMD_TARGET_SPEED == ct)
	{
		//unit:rps!!!!!!!!
		float encoderLines = Mileage::Instance()->encoderLines();
		int t = (val >= 0) ? 1 : -1;
		//unit:rps
		int32_t speedVal = (int32_t)(val /(4.0f * encoderLines) * 24.0f + t * 0.5f);
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
		msg.Data[0] = 0x43;
		msg.Data[1] = 0x0a;
		msg.Data[2] = 0x70;
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
	
	if (false == isInitSend)
	{
		static int count = 0;
		if (count >= 1)
		{
			int8_t currentMsg = (Mileage::Instance()->num_of_wheel() * MINGZHI_INIT_MSG_COUNTS) - msgCount;
			memcpy(msg.Data, MINGZHI_VEL_MODE_INIT_DATA[currentMsg % MINGZHI_INIT_MSG_COUNTS], 8);
			msg.StdId = _motorRxId[(uint8_t)(currentMsg / MINGZHI_INIT_MSG_COUNTS)];
			Console::Instance()->printf("msgCount: %d.\r\n", msgCount);
			count = 0;
			msgCount--;
			if(msgCount == 0)
				isInitSend = true;
		} else {
			count++;
			msg.StdId = 0x610;
			memcpy(msg.Data, MINGZHI_VEL_MODE_INIT_DATA[0], 8);
		}
		
	}
}

/**
  * @brief  decode
	* @param  idx: index of motor
	* @param  id: protocol id of motor
	* @retval false: driver state error
  */
bool CProtocolMingZhiCanopen::decode(motorDriverCmdType& ct, int32_t& val, const CanRxMsg& msg)
{
	if (false == isInitSend)
	{
		ct = DRV_CMD_TARGET_SPEED;
		val = 0;
		return true;
	}
	
	uint32_t tempVal = 0;
	
	if(memcmp(MINGZHI_TARGET_SPEED_DATA, msg.Data, 3) == 0)
	{
			ct = DRV_CMD_TARGET_SPEED;
			return true; //compare finished
	}
	
	/* check for COPLEY_CAN_ACTURAL_POS */
	for(int i = 0; i < 8; i++)
	{
		if((i < 4) && MINGZHI_ACTURAL_POS_DATA[i] != msg.Data[i]) //compare bytes 0-3
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
		if((i < 4) && MINGZHI_ERR_CODE_DATA[i] != msg.Data[i]) //compare bytes 0-3
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
			
			if (((val & 0x07) != 0x07) && true == isInitSend)
			{
			//	ChassisDriver::Instance()->setOpen(true);
				Console::Instance()->printf("mingzhi reconfig\r\n");
				isInitSend = false;
				msgCount = (Mileage::Instance()->num_of_wheel() * MINGZHI_INIT_MSG_COUNTS);
			}
			
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
int32_t CProtocolMingZhiCanopen::getMotorIndex(const CanRxMsg& Msg)
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
bool CProtocolMingZhiCanopen::getInitMsg(CanTxMsg& msg)
{
	return false;
}

/**
  * @brief  parse error code and print
	* @param  uint32_t error code
	* @retval None
  */
void CProtocolMingZhiCanopen::parseErrCode(uint8_t motorIdx, uint32_t errCode) const
{
	/*
	if ((errCode & 0x20) > 0)
	{
		GlobalControl::setChassisEmergencyButtonOn();
	}
	else
	{
		GlobalControl::setChassisEmergencyButtonOff();
	}*/
	
	if (((errCode & 0x07) != 0x07) && true == isInitSend)
	{
	//	ChassisDriver::Instance()->setOpen(true);
		Console::Instance()->printf("mingzhi reconfig\r\n");
		isInitSend = false;
		msgCount = (Mileage::Instance()->num_of_wheel() * MINGZHI_INIT_MSG_COUNTS);
	}
	
	if ((errCode & 0x07) != 0x07)
	{
		Console::Instance()->printf("motor%d error code: 0x%X.\r\n", motorIdx, errCode);
	}
	//Console::Instance()->printf("motor%d error code: 0x%X. Copley parse rule undefined.\r\n", motorIdx, errCode);
}

/**
  * @brief  calculate motor Rx ID
  * @param  offset of motorID
  * @retval motor Rx ID
  */
uint16_t CProtocolMingZhiCanopen::calcRxID(uint16_t ID)
{
	return 0x600 + ID;
}

/**
  * @brief  calculate motor Tx ID
  * @param  offset of motorID
  * @retval motor Tx ID
  */
uint16_t CProtocolMingZhiCanopen::calcTxID(uint16_t ID)
{
	return 0x580 + ID;
}

//end of file
