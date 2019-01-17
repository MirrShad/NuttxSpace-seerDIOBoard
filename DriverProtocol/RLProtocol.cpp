#include "RLProtocol.h"
#include "Console.h"
#include "MessageTask.h"
#include "GlobalData.h"

namespace RLDriver{
	enum CmdType{
		RL_CMD_IDLE					= 0x00,
		RL_TARGET_SPEED			= 0x10
	};

	enum RetType{
		RL_ACTURAL_POS			= 0x01,
		RL_DRI_STATE				= 0x10,
	};
}

void CProtocolRL::int2byte(int32_t val, uint8_t& a, uint8_t& b, uint8_t& c, uint8_t& d)
{
	uint32_t temp;
	memcpy(&temp,&val,sizeof(uint32_t));
	a = uint8_t((temp >> 24) & 0xFF);
	b = uint8_t((temp >> 16) & 0xFF);
	c = uint8_t((temp >> 8)  & 0xFF);
	d = uint8_t(temp		   & 0xFF);
}

void CProtocolRL::byte2int(uint8_t a, uint8_t b, uint8_t c, uint8_t d, int32_t& val)
{
	uint32_t temp =	((uint32_t(a) << 24) & 0xFF000000)	|
					((uint32_t(b) << 16) & 0x00FF0000)	|
					((uint32_t(c) << 8) & 0x0000FF00)		|
					(uint32_t(d) & 0x000000FF);
	memcpy(&val,&temp,sizeof(uint32_t));
}

/**
  * @brief  parse Motor Index from can message
  * @param  Msg get from CAN
  * @retval ret >= 0, Motor index
	*  				ret == -1, parse failed
  */
int32_t CProtocolRL::getMotorIndex(const CanRxMsg& Msg)
{
	for(int i = 0; i < CMileage::MAX_WHEEL_NUM; i++)
	{
		if(Msg.StdId == _motorTxId[i])
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
void CProtocolRL::encode(uint8_t motor_idx, motorDriverCmdType ct, int32_t val, CanTxMsg& msg)
{
	seer_assert(motor_idx < Mileage::Instance()->num_of_wheel());
	msg.StdId = _motorRxId[motor_idx];
	msg.IDE = CAN_Id_Standard;
	msg.RTR = CAN_RTR_Data;
	msg.DLC = 6;
	
	int2byte(val,msg.Data[0],msg.Data[1],msg.Data[2],msg.Data[3]);
	if(DRV_CMD_TARGET_SPEED == ct)
	{
		msg.Data[5] = RLDriver::RL_TARGET_SPEED;
		msg.Data[4] = RLDriver::RL_DRI_STATE;
	}else if(DRV_CMD_ACTURAL_POS == ct)
	{
		msg.Data[5] = RLDriver::RL_CMD_IDLE;
		msg.Data[4] = RLDriver::RL_ACTURAL_POS;
	}else if(DRV_CMD_DRI_STATE == ct)
	{
		msg.Data[5] = RLDriver::RL_CMD_IDLE;
		msg.Data[4] = RLDriver::RL_DRI_STATE;
	}
}

/**
  * @brief  decode
	* @param  idx: index of motor
	* @param  id: protocol id of motor
	* @retval false: driver state error
  */
bool CProtocolRL::decode(motorDriverCmdType& ct, int32_t& val, const CanRxMsg& msg)
{

	if(msg.Data[4] == RLDriver::RL_ACTURAL_POS)
		ct = DRV_CMD_ACTURAL_POS;
	else if(msg.Data[4] == RLDriver::RL_DRI_STATE)
		ct = DRV_CMD_DRI_STATE;
	else
		ct = DRV_CMD_UNDEFINED;
	
	byte2int(msg.Data[0],msg.Data[1],msg.Data[2],msg.Data[3],val);
	if (msg.Data[5] & 0x80)
	{
		return true;//error
	}
	return false;//no error
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

const uint8_t RL_ERR_NUM = 22;
const uint8_t FAULT_HARDSTOP_IDX = 11;
const uint32_t FAULT_ENCO_HALL_MISMATCH_IDX = 21;
const uint32_t FAULT_CURR_SUM_NOT_ZERO_IDX = 17;
const uint32_t FAULT_HALL_ERROR_IDX = 16;
const uint32_t FAULT_DC_BUS_LOW_15_IDX = 19;

enum_str RLErrTable[RL_ERR_NUM] = 
{
	{ "DRV_FAULT_GVDD_OV_BIT",			0x00000001 },
	{ "DRV_FAULT_FETLC_OC_BIT",			0x00000002 },
	{ "DRV_FAULT_FETLB_OC_BIT",			0x00000004 },
	{ "DRV_FAULT_FETLA_OC_BIT",			0x00000008 },
	
	{ "DRV_FAULT_FETHC_OC_BIT",			0x00000010 },
	{ "DRV_FAULT_FETHB_OC_BIT",			0x00000020 },
	{ "DRV_FAULT_FETHA_OC_BIT",			0x00000040 },
	{ "DRV_FAULT_OTW_BIT",				0x00000080 },
	
	{ "DRV_FAULT_PVDD_UV_BIT",			0x00000100 },
	{ "DRV_FAULT_GVDD_UV_BIT",			0x00000200 },
	{ "DRV_FAULT_BIT",					0x00000400 },
	{ "FAULT_HARDSTOP_BIT",				0x00000800 },
	
	{ "FAULT_FLASH_DATA_DISABLE",		0x00001000 },
	{ "FAULT_POSOFFSET_BIT",			0x00002000 },
	{ "FAULT_DRV8301_NO_CON_BIT",		0x00004000 },
	{ "FAULT_HALL_ERROR",				0x00008000 },
	
	{ "FAULT_HALL_VALUE_ERROR",			0x00010000 },
	{ "FAULT_CURRENT_SUM_NOT_ZERO",		0x00020000 },
	{ "FAULT_LOCK_BY_ANY_FAULT",		0x00000000 },//ignore lock by any fault
	{ "FAULT_DC_BUS_LOW",				0x00080000 },
	
	{ "FAULT_RUN_OUT_OF_CONTROL",		0x00100000  },
	{ "FAULT_ENCO_HALL_MISMATCHED",		0x00200000  }
};
void CProtocolRL::parseErrCode(uint8_t motorIdx, uint32_t errCode) const
{

	if(isEmsPressed(errCode))
	{
		GlobalControl::setChassisEmergencyButtonOn();
	}else
	{
		GlobalControl::setChassisEmergencyButtonOff();
	}
		
	errCode &= ~(RLErrTable[FAULT_HARDSTOP_IDX].errCode);
	bool needSetErr = false;
	
	if(errCode & (1ul << FAULT_ENCO_HALL_MISMATCH_IDX))
	{
		Message::Instance()->postErrMsg(CODE_MOTO_HALL_ENCO_MISM);
	}

	if(errCode & (1ul << FAULT_HALL_ERROR_IDX))
	{
		Message::Instance()->postErrMsg(CODE_MOTO_HALL_ERRO);
	}

	if(errCode & (1ul << FAULT_DC_BUS_LOW_15_IDX))
	{
		Message::Instance()->postErrMsg(CODE_MOTO_DC_BUS_LOW_15);
	}
	
	if(errCode & (1ul << FAULT_CURR_SUM_NOT_ZERO_IDX))
	{
		Message::Instance()->postErrMsg(CODE_MOTO_CURR_SUM_NOT_ZERO);
	}
	
	for(int i = 0; i < RL_ERR_NUM; i++)
	{
		if((RLErrTable[i].errCode & errCode) > 0)
		{
			needSetErr = true;
			Message::Instance()->postMsg("motor[%d] error: %s", motorIdx, RLErrTable[i].errStr);
		}
	}
	
	if(needSetErr)
		Message::Instance()->postErrMsg(CODE_MOTO_ERRO_SEE_LOG);
	else
		Message::Instance()->clearErr(CODE_MOTO_ERRO_SEE_LOG);
}

bool CProtocolRL::isEmsPressed(uint32_t errCode) const
{
	if ((errCode & RLErrTable[FAULT_HARDSTOP_IDX].errCode) > 0)
	{
		return true;
	}
	return false;
}

/**
  * @brief  calculate motor Rx ID
  * @param  offset of motorID
  * @retval motor Rx ID
  */
uint16_t CProtocolRL::calcRxID(uint16_t ID)
{
	if (ID < 1)
	{
		Message::Instance()->postMsg("motor ID config error, ID = %d", ID);
		ID = 1;
	}
	return 2 * ID;
}

/**
  * @brief  calculate motor Tx ID
  * @param  offset of motorID
  * @retval motor Tx ID
  */
uint16_t CProtocolRL::calcTxID(uint16_t ID)
{
	if (ID < 1)
	{
		Message::Instance()->postMsg("motor ID config error, ID = %d", ID);
		ID = 1;
	}
	return 2 * ID - 1;
}

//end of file

