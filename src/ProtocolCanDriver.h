#ifndef _PROTOCOL_CAN_DRIVER_H_
#define _PROTOCOL_CAN_DRIVER_H_

#include "Singleton.h"
#include <stdint.h>
#include "Mileage.h"
#include <stdio.h>

class CDriverCanProtocol
{
	public:
		enum DriverCanProtocolEnum
		{
			Protocol_RL = 1,
			Protocol_CanOpen,
			Protocol_Castdservo,
			Protocol_Keya,
			Protocol_CanOpenPosition,
			Protocol_HLS,
			Protocol_DeltaCanopen,
			Protocol_KincoCanopen,
			Protocol_HlsCanopen,
			Protocol_Curtis,
			Protocol_MotecCanopen,
			Protocol_JingZhiCanopen,
			Protocol_MingZhiCanopen
		};
		
		CDriverCanProtocol(){};
			
		uint32_t getMotorTxId(uint8_t idx)
		{
			if(idx >= Mileage::Instance()->num_of_wheel()) printf("Error in get Motor tx id\r\n");
			return _motorTxId[idx];
		}
		
		uint32_t getMotorRxId(uint8_t idx)
		{
			if(idx >= Mileage::Instance()->num_of_wheel()) printf("Error in get Motor rx id\r\n");
			return _motorRxId[idx];
		}
		
		/**
		* @brief  set motor rx id
		* @param  idx: index of motor
		* @param  id: protocol id of motor
		* @retval None
		*/
		void setMotorTxId(uint8_t idx, uint32_t id)
		{
			if(idx >= Mileage::Instance()->num_of_wheel()) printf("Error in set Motor tx id\r\n");
			_motorTxId[idx] = id;
		}
		
		/**
		* @brief  set motor tx id
		* @param  idx: index of motor
		* @param  id: protocol id of motor
		* @retval None
		*/
		void setMotorRxId(uint8_t idx, uint32_t id)
		{
			if(idx >= Mileage::Instance()->num_of_wheel()) printf("Error in get Motor rx id\r\n");
			_motorRxId[idx] = id;
		}	
		
		void setMotorInverse(uint8_t idx, bool inverse)
		{
			if(idx >= Mileage::Instance()->num_of_wheel()) printf("Error in set Motor inverse\r\n");
			_inverse[idx] = inverse;
		}
		
		bool getMotorInverse(uint8_t idx)
		{
			if(idx >= Mileage::Instance()->num_of_wheel()) printf("Error in get Motor inverse\r\n");
			return _inverse[idx];
		}
		
		//virtual bool getInitMsg(CanTxMsg&) {return false;}
		
		virtual ~CDriverCanProtocol(){};
		///virtual int32_t getMotorIndex(const CanRxMsg&) = 0; 
		
		//virtual void encode(uint8_t motor_idx, motorDriverCmdType ct, int32_t val, CanTxMsg&) = 0;
		//virtual bool decode(motorDriverCmdType& ct, int32_t& val, const CanRxMsg&) = 0;
		/*virtual void parseErrCode(uint8_t motorIdx, uint32_t errCode) const 
			{Console::Instance()->printf("motor%d error:0x%08X\r\n", motorIdx, errCode);}*/

		virtual bool isEmsPressed(uint32_t errCode) const {return false;}
		
		virtual uint16_t calcRxID(uint16_t ID) = 0;
		virtual uint16_t calcTxID(uint16_t ID) = 0;
	
	protected:
		uint32_t _motorTxId[CMileage::MAX_WHEEL_NUM];
		uint32_t _motorRxId[CMileage::MAX_WHEEL_NUM];
		bool _inverse[CMileage::MAX_WHEEL_NUM];
	
};
												
#endif
//end of file
