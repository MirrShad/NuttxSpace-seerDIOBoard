#include "ProtocolKeya.h"
#include "Console.h"

/**
  * @brief  Constructor
	* @param  None
  * @retval None
  */
CProtocolKeya::CProtocolKeya()
{
	for(int i = 0; i < Mileage::Instance()->num_of_wheel(); i++)
	{
		setMotorRxId(i, 0x601 + (i >> 1));
		
		setMotorTxId(i, 0x581 + (i >> 1));
	}
}

/**
  * @brief  encode
	* @param  idx: index of motor
	* @param  id: protocol id of motor
  * @retval None
  */
void CProtocolKeya::encode(uint8_t motor_idx, motorDriverCmdType ct, int32_t val, CanTxMsg& msg)
{
	seer_assert(motor_idx < Mileage::Instance()->num_of_wheel());
	msg.StdId = _motorRxId[motor_idx];
	msg.IDE = CAN_Id_Standard;
	msg.RTR = CAN_RTR_Data;
	msg.DLC = 8;
	
	msg.Data[3] = 1 + (motor_idx & 1); //subIndex of CanOpen
	
	if (DRV_CMD_TARGET_SPEED == ct)
	{
		msg.Data[0] = 0x23;//set motor param
		msg.Data[1] = 0x00;//main index 0x2000
		msg.Data[2] = 0x20;//main index
		
		float encoderLines = Mileage::Instance()->encoderLines();
		val = (int32_t)(val*6.0f/(4.0f * encoderLines) + 0.5f);
		
		for(int i = 4; i < 6; i++)
		{
			msg.Data[i] = val & 0xFF;
			val >>= 8;
		}
		msg.Data[6] = 0x00;
		msg.Data[7] = 0x00;
	}else if (DRV_CMD_ACTURAL_POS == ct)
	{
		msg.Data[0] = 0x40;//get motor param
		msg.Data[1] = 0x04;//main index 0x2104
		msg.Data[2] = 0x21;//main index
		for(int i = 4; i < 8; i++)
		{
			msg.Data[i] = 0x00;
			val >>= 8;
		}
	}else if (DRV_CMD_DRI_STATE == ct)
	{
		msg.Data[0] = 0x40;//get motor param
		msg.Data[1] = 0x12;//main index 0x2112
		msg.Data[2] = 0x21;//main index
		for(int i = 4; i < 8; i++)
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
bool CProtocolKeya::decode(motorDriverCmdType& ct, int32_t& val, const CanRxMsg& msg)
{
	val = 0;
	for(int i = 7; i > 3; i--)
	{
		val <<= 8;
		val += msg.Data[i];
	}
	if(msg.Data[1] == 0x00 && msg.Data[2] == 0x20)
		ct = DRV_CMD_TARGET_SPEED;
	else if(msg.Data[1] == 0x04 && msg.Data[2] == 0x21)
	{
		ct = DRV_CMD_ACTURAL_POS;
		
	}
	else if(msg.Data[1] == 0x12 && msg.Data[2] == 0x21)
	{
		ct = DRV_CMD_DRI_STATE;
		val &= 0xFFFFFF;
	}
	
	
	return true;
}

/**
  * @brief  parse Motor Index from can message
  * @param  Msg get from CAN
  * @retval ret >= 0, Motor index
	*  				ret == -1, parse failed
  */
int32_t CProtocolKeya::getMotorIndex(const CanRxMsg& Msg)
{
	
	uint8_t subIdxCount = 0;
	for(int i = 0; i < CMileage::MAX_WHEEL_NUM; i++)
	{
		if(Msg.StdId == _motorTxId[i])
		{
			if(Msg.Data[3] == ++subIdxCount)
				return i;
		}
	}

	seer_assert(false);
	return 0;
}

/**
  * @brief  calculate motor Rx ID
  * @param  offset of motorID
  * @retval motor Rx ID
  */
uint16_t CProtocolKeya::calcRxID(uint16_t ID)
{
	return 0x600 + ID;
}
	
/**
  * @brief  calculate motor Tx ID
  * @param  offset of motorID
  * @retval motor Tx ID
  */
uint16_t CProtocolKeya::calcTxID(uint16_t ID)
{
	return 0x580 + ID;
}

//end of file
