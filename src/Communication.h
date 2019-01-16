#ifndef _COMMUNICATION_PROTOCOL_H_
#define _COMMUNICATION_PROTOCOL_H_

#include <stdint.h>
#include <string.h>

//max length for command packet & config packet
#define PACKET_CHAR_LENGTH	135

//sensor num defines
#ifdef RUN_FOR_INT_BOARD
	#define MAX_DI_NUM				32
	#define MAX_INTBRD_DI_NUM		10
	#define INTBRD_DI_MSK			0x303FF
	#define MAX_DO_NUM				32
	#define MAX_INTBRD_DO_NUM		14
#else
	#define MAX_DI_NUM				32
	#define MAX_DO_NUM				32
#endif

#define SONIC_SENSER_NUM		24
#define MAGTRACK_NUM			2
//sensor num defines

/************************************************************************/
/*	                           control                                 */
/************************************************************************/
class CChassisControl
{
public:
	float X;				//forward + ,back -  
	float Y;				//forward + ,back - 
	float W;				//turn to right +, turn to left -
	void reset() {
		X = 0.0;
		Y = 0.0;
		W = 0.0;
	}
};

class CArmControl		//arm
{
public:
	float locA;			//temporarily define
	float locB;
	void reset() {
		locA = 0.0;
		locB = 0.0;
	}
	//...
};

class CControlData	
{
public:
	enum controlType		//control head
	{
		EChassisControl = 0,
		EArmControl,

		E_CONTROL_NUM_MAX = 0xFFFFFFFF
	};
	CControlData() {
		memset(m_control_cmd.m_buffer, 0, PACKET_CHAR_LENGTH);
	}

	int size() {
		if (m_control_type == EChassisControl) {
			return sizeof(controlType) + sizeof(CChassisControl);
		} 
		else if (m_control_type == EArmControl) {
			return sizeof(controlType) + sizeof(CArmControl);
		}
		return 0;
	}

	union controlCmd {
		CChassisControl m_chassis;
		CArmControl m_arm;
		char m_buffer[PACKET_CHAR_LENGTH];
	};

public:
	controlType m_control_type;
	controlCmd  m_control_cmd;
};

/************************************************************************/
/*                                report                                */
/************************************************************************/

class CReportBase			//data report
{
public:
	CReportBase():_bEnable(0){};
	void setEnable(bool en){_bEnable = (int32_t)en;}
	bool isEnable(){return (bool)_bEnable;}
private:
	int32_t _bEnable;
};

class CRFIDReport : public CReportBase
{
public:
	uint32_t tagStamp;		// hit new station ONLY when (tagStamp > 0) && (the value of tagStamp changed) 
	uint8_t vendorID;
	uint32_t tagID;
};

class CMagneticSenserReport : public CReportBase
{
public:
	uint8_t senser;
};

class COdometerReport : public CReportBase		//mileage data
{
public:
	COdometerReport(){}
	float mX;				//horizontally move, right +,left -
	float mY;				//vertically move,forward +,back -
	float mTheta;			//turn right +,turn left -
	uint32_t state1;
	uint32_t state2;
//	int32_t mStopSwitch;
//	uint32_t absEncoder1;
//	uint32_t absEncoder2;
};

typedef struct
{
	uint16_t address;
	uint16_t data;
}SonicData;

class CSonicReport : public CReportBase
{
public:
	uint16_t sonicNum;
	SonicData sonic[SONIC_SENSER_NUM];
};

class CGyroReport : public CReportBase
{
public:
	float yaw;
	float pitch;
	float roll;
	float tempr;		//temperature
	float ax;
	float ay;
	float az;
	float gx;
	float gy;
	float gz;
};

class CIntBrdReport : public CReportBase
{
public:
	
	float temDat;
	float humiDat;
	float voltage;
	uint32_t emergencyStop;
	uint32_t brkState;
	float current;
};

class CBatteryReport : public CReportBase	
{
public:
	float  current;
	float  voltage;
	float  maxTemperature;
	float  soc;
};

class CIOReport : public CReportBase
{
	enum DItype
	{
		e_collision = 0		
		// add by yourself
	};
	enum DOtype 
	{
		e_warning = 0,
		e_brake
		// add by yourself
	};
	
private:
	uint32_t DI;
	uint32_t DO;

public:
/*#if DSP_SIDE
	// pos start from 0.
#ifdef RUN_FOR_INT_BOARD
	void setDI(uint16_t pos, uint16_t data){ DI = (data > 0) ? (DI | (1ul << (pos+MAX_INTBRD_DI_NUM))):(DI & ~(1ul << (pos+MAX_INTBRD_DI_NUM))); }
	int32_t getIntBrdDo() { return DO; }
	void setIntBrdDI(uint32_t intbrddist) {DI &= (~INTBRD_DI_MSK),DI |= intbrddist;}																					//
#else
	void setDI(uint16_t pos, uint16_t data) { DI = (data > 0) ? (DI | (1ul << pos)):(DI & ~(1ul << pos)); }
#endif

	void setDO(uint16_t pos, uint16_t data){ DO = (data > 0) ? (DO | (1ul << pos)):(DO & ~(1ul << pos)); }
	void setDO(uint32_t data){ DO=data; }
#endif*/
	// return 1 or 0. if pos >= MAX_NUM then return -1
	int32_t getDI(uint16_t pos){return (pos < MAX_DI_NUM) ? (int32_t)((DI >> pos) & 1ul) : (-1);}																									//to do???
	int32_t getDO(uint16_t pos){return (pos < MAX_DO_NUM) ? (int32_t)((DO >> pos) & 1ul) : (-1);}//to do???
	uint32_t getDI() { return DI; }
  uint32_t getDO() { return DO; }
};

class CReportData
{
public:
	enum enumType	//config head
	{
		EOdometer = 0,
		EBattery,
		EDigiInput,
		EDigiOutput,
		EGyro,
		ESonic,
		ERFID,
		EMagnetSensor,
		EIntBrd,
		EEncoder,
		EEkfPos,
		EDeveloperReserve,
		ENamedDigitalInput,
		EPgvDmt,
		EFork,
		EAstern,
		ELift,
	};

	int32_t					packetCount;
	int32_t					packetTimestamp;
	COdometerReport			odometerDat;
	CBatteryReport			batteryDat;
	CIOReport				ioDat;
	CGyroReport				gyroDat;
	CSonicReport			sonicDat;
	CRFIDReport				RFIDDat;
	CMagneticSenserReport	magnetDat[MAGTRACK_NUM];
	CIntBrdReport			IntBrdDat;
};

/************************************************************************/
/*                                config                                */
/************************************************************************/
//config only when system initial
class CDeviceDemandConfig
{
public:
	int32_t bChassisExist;
	int32_t bBatteryExist;
	int32_t bGyroExist;
	int32_t bDmxLedExist;
	int32_t bSonicExist;
	int32_t bRFIDExist;
	int32_t bMagneticSenserExist;
	int32_t bIntBrdExist;
};

class CChassisStructConfig{
public:
	float E;					// Distance from left to right
	float R;					// Radius of the single wheel
	int32_t encoderLines;
	float reductionRatio;
	int32_t motorMaxSpeedRpm;
	int32_t Inverse;
};

class CChassisStructConfig4cp{
public:
	float E;
	float R[4];
	int32_t encoderLines;
	float reductionRatio;
	int32_t motorMaxSpeedRpm;
	int32_t Inverse;
};

// for set DO
class CDigitalOutput{
public:
	uint16_t pos;
	uint16_t data;
};

class CConfigData{
public:
	enum configType	//config head
	{
		// PC -> DIOBoard
		EDeviceDemandConfig = 0,
		EChassisStructConfig,
		EChargeControl,
		EGyroCalibration,
		EGyroAutoCaliSwitch,
		ESonicReconfiguration,
		EHostError,
		EInApplicationProgram,
		EChassisBrakeSwitch,
		EResetDIOBoard,
		ESetDO,
		EGetUid,
		EUnlock,
		EGetMainVersion,
		EGetFullVersion,
		EPrintDebugInfo,
		ESetDefaultDO,//0x10
		ESetDriverPowerDoPos,
		ELaunchEthCANTool,
		EPrintBaseTime,
		EGetMcuNtpTime,
		EGetGyroVersion,

		// DIOBoard -> PC
		EReboot = 0x100,
		EChassisStructConfig4cp,

		// keep alive
		EKeepAlive = 0x200,

		E_CONFIG_NUM_MAX = 0xFFFFFFFF
	};

	//config type
	CConfigData(){
		memset(m_config_cmd.m_buffer,0,PACKET_CHAR_LENGTH);

	}

	int size() {
		if (m_config_type == EDeviceDemandConfig) {
			return sizeof(configType) + sizeof(CDeviceDemandConfig);
		} else if (m_config_type == EChassisStructConfig) {
			return sizeof(configType) + sizeof(CChassisStructConfig);
		} else if (m_config_type == EChassisStructConfig4cp) {
			return sizeof(configType) + sizeof(CChassisStructConfig4cp);
		} else if (m_config_type == ESetDO) {
			return sizeof(configType) + sizeof(CDigitalOutput);
		}else  {
			return sizeof(configType) + sizeof(uint32_t);
		}
	}

public:
	union configCmd{
		CDeviceDemandConfig m_device_demand;
		CChassisStructConfig chassisStructConfig;
		CChassisStructConfig4cp chassisStructConfig4cp;

		// use 0xFFFFFFFF & 0x0
		uint32_t chargeControl;			// charge:				0xFFFFFFFF
		uint32_t gyroCalibration;		// do calibration:		0xFFFFFFFF
		uint32_t gyroAutoCaliSwitch;	// auto calibration:	0xFFFFFFFF
		uint32_t sonicReconfiguration;	// reconfigure sonic:	0xFFFFFFFF
		uint32_t hostError;				// there is an error:	0xFFFFFFFF
		uint32_t iapTrigger;			// download program:	0xFFFFFFFF
		uint32_t chassisBrakeSwitch;	// enable brake			0xFFFFFFFF
		uint32_t resetDIOBoard;			// reset DIO board 		0xFFFFFFFF
		CDigitalOutput digitalOutput;	// set DO state

		uint32_t keepAlive;				// alive:				0xFFFFFFFF
		
		uint32_t data;

		char m_buffer[PACKET_CHAR_LENGTH];
	};

	configType m_config_type;
	configCmd  m_config_cmd;
};

class CConfigReport
{
public:
	union UConfigReportData{

		uint32_t data;

		char m_buffer[PACKET_CHAR_LENGTH];
		};
	
	CConfigReport()
	{
		m_config_type = CConfigData::E_CONFIG_NUM_MAX;
		resetDat();
	}

	void resetDat(){
		memset(m_config_report.m_buffer,0,PACKET_CHAR_LENGTH);
	}

	int size() {

		int s = sizeof(CConfigData::configType);
		if (CConfigData::EKeepAlive == m_config_type)
		{
			s += sizeof(uint32_t);
		}
		else if (CConfigData::EGetUid == m_config_type)
		{
			s += PACKET_CHAR_LENGTH;
		}
		else if (CConfigData::EGetMainVersion == m_config_type)
		{
			s += PACKET_CHAR_LENGTH;
		}
		else if (CConfigData::EGetFullVersion == m_config_type)
		{
			s += PACKET_CHAR_LENGTH;
		}
		else if(CConfigData::EGetMcuNtpTime == m_config_type)
		{
			s += sizeof(uint64_t);
		}
		else if(CConfigData::EGetGyroVersion == m_config_type)
		{
			s += PACKET_CHAR_LENGTH;
		}
		return s;
	}

public:
	CConfigData::configType m_config_type;
	UConfigReportData m_config_report;
};

#endif	//_COMMUNICATION_PROTOCOL_H_
