#include "ProtocolHLS.h"
#include "Console.h"
#include "MessageTask.h"

#define HLS_SPEED_COMMAND_ADDR	0x00
#define HLS_QUERY_POS_ADDR			0x1D
#define HLS_QUERY_STATE_ADDR		0x5B

namespace{

	enum HlsFuncEnum
	{
		E_RPT_UNDEFINED	= 0x00,

		E_SEND_COMMAND_WORD = 0x28,
		E_RPT_COMMAND_WORD = 0x29,

		E_SEND_QUERY_WORD = 0x0A,
		E_RPT_QUERY_WORD = 0x0B,

		E_SEND_QUERY_2WORDS = 0x0D,
		E_RPT_QUERY_2WORDS = 0x0E
	};

}



/**
  * @brief  Constructor
	* @param  None
  * @retval None
  */
CProtocolHLS::CProtocolHLS()
{
	for(int i = 0; i < Mileage::Instance()->num_of_wheel(); i++)
	{
		setMotorRxId(i, 2 * i);		
		setMotorTxId(i, 2 * i - 1);
	}
}

/**
  * @brief  parse Motor Index from can message
  * @param  Msg get from CAN
  * @retval ret >= 0, Motor index
	*  				ret == -1, parse failed
  */
int32_t CProtocolHLS::getMotorIndex(const CanRxMsg& Msg)
{
	for(int i = 0; i < CMileage::MAX_WHEEL_NUM; i++)
	{
		// will be ok too
		// if(Msg.Data[0] == _motorRxId[i] - 0x700)
		// 	return i;
		
		// will be ok too
		if((Msg.StdId == _motorTxId[i]) && (Msg.StdId == 0x700 + Msg.Data[0]))
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
void CProtocolHLS::encode(uint8_t motor_idx, motorDriverCmdType ct, int32_t val, CanTxMsg& msg)
{
	seer_assert(motor_idx < Mileage::Instance()->num_of_wheel());
	msg.StdId = _motorRxId[motor_idx];
	msg.IDE = CAN_Id_Standard;
	msg.RTR = CAN_RTR_Data;
	
	if(DRV_CMD_TARGET_SPEED == ct)
	{
		msg.DLC = 6;
		float encoderLines = Mileage::Instance()->encoderLines();
		int t = (val >= 0) ? 1 : -1;
		int16_t speedVal = (int16_t)(val * 6.0f/(4.0f * encoderLines) + t * 0.5f);
		
		msg.Data[0] = _motorTxId[motor_idx] & 0xFF;
		msg.Data[1] = E_SEND_COMMAND_WORD;
		msg.Data[2] = 0x00;
		msg.Data[3] = HLS_SPEED_COMMAND_ADDR;
		msg.Data[4] = ((speedVal >> 8)& 0xff);
		msg.Data[5] = (speedVal & 0xff);
		seer_assert(false);
	}else if(DRV_CMD_ACTURAL_POS == ct)
	{
		msg.DLC = 4;
		msg.Data[0] = _motorTxId[motor_idx] & 0xFF;
		msg.Data[1] = E_SEND_QUERY_2WORDS;
		msg.Data[2] = 0x00;
		msg.Data[3] = HLS_QUERY_POS_ADDR;
		seer_assert(false);
	}else if(DRV_CMD_DRI_STATE == ct)
	{
		msg.DLC = 4;
		msg.Data[0] = _motorTxId[motor_idx] & 0xFF;
		msg.Data[1] = E_SEND_QUERY_WORD;
		msg.Data[2] = 0x00;
		msg.Data[3] = HLS_QUERY_STATE_ADDR;
		seer_assert(false);
	}
}

/**
  * @brief  decode
	* @param  idx: index of motor
	* @param  id: protocol id of motor
	* @retval false: driver state error
  */
bool CProtocolHLS::decode(motorDriverCmdType& ct, int32_t& val, const CanRxMsg& msg)
{
	if(E_RPT_COMMAND_WORD == msg.Data[1] && HLS_SPEED_COMMAND_ADDR == msg.Data[3])
	{
		ct = DRV_CMD_TARGET_SPEED;
	}else if(E_RPT_QUERY_2WORDS == msg.Data[1] && HLS_QUERY_POS_ADDR == msg.Data[3])
	{
		ct = DRV_CMD_ACTURAL_POS;
		uint8_t tempValP[4] = {0,0,0,0};
		tempValP[0] = msg.Data[5];
		tempValP[1] = msg.Data[4];
		tempValP[2] = msg.Data[7];
		tempValP[3] = msg.Data[6];
		val = *(reinterpret_cast<int32_t*>(tempValP));
	}else if(E_RPT_QUERY_WORD == msg.Data[1] && HLS_QUERY_STATE_ADDR == msg.Data[3])
	{
		ct = DRV_CMD_DRI_STATE;
		uint8_t tempValP[2] = {0,0};
		tempValP[0] = msg.Data[5];
		tempValP[1] = msg.Data[4];
		val = (int32_t)(*(reinterpret_cast<uint16_t*>(tempValP)));
	}else
	{
		ct = DRV_CMD_UNDEFINED;
	}
	
	return true;
}

/**
  * @brief  parse error code and print
	* @param  uint32_t error code
	* @retval None
  */
void CProtocolHLS::parseErrCode(uint8_t motorIdx, uint32_t errCode) const
{
	if ((errCode & 0x10) > 0)
	{
		Message::Instance()->postErrMsg(CODE_MOTO_ERRO_SEE_LOG, "driver%d gets a warning code.", motorIdx);
	}
	else{
		Message::Instance()->clearErr(CODE_MOTO_ERRO_SEE_LOG);
	}
}

/**
  * @brief  calculate motor Rx ID
  * @param  offset of motorID
  * @retval motor Rx ID
  */
uint16_t CProtocolHLS::calcRxID(uint16_t ID)
{
	return 0x100 + ID;
}

/**
  * @brief  calculate motor Tx ID
  * @param  offset of motorID
  * @retval motor Tx ID
  */
uint16_t CProtocolHLS::calcTxID(uint16_t ID)
{
	return 0x700 + ID;
}


//end of file
