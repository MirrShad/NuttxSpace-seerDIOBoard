#include "chassis.h"
#include "message_navigation.pb.h"
#include <pb_decode.h>
#include "ProtocolCopleyCAN.h"
#include "Mileage.h"
#include "ChassisCmdType.h"

CChassisDevice::CChassisDevice()
{
	_driverProtocol = NULL;
	currentCmd = 0;
	bNewSpeedCmd = false;
	bOdoCmd = false;
	bStateCmd = false;
	sem_init(&openSem,0,0);
	sem_init(&cmdSem,0,0);
	ori_global_X = 0;
	ori_global_Y = 0;
	ori_global_W = 0;
}

int CChassisDevice::chassisCmdHandler(uint8_t* pbData,uint16_t len)
{
    bool status;
    rbk_protocol_Message_NavSpeed pbMsg = rbk_protocol_Message_NavSpeed_init_zero;
    pb_istream_t stream = pb_istream_from_buffer(pbData,len);
    status = pb_decode(&stream,rbk_protocol_Message_NavSpeed_fields,&pbMsg);
    if(!status)
        printf("Decode fail in Chassis.cxx\r\n");   
	ori_global_X = pbMsg.x;
	ori_global_Y = pbMsg.y;
	ori_global_W = pbMsg.rotate;
	newSpeedCmd();
	return 0;
}

bool CChassisDevice::setProtocol(const uint8_t proto_type)
{
    if (_protocol == proto_type)
	{
		return true;
	}
	_protocol = proto_type;
	
	if(_driverProtocol != NULL)
	{
		delete _driverProtocol;
		_driverProtocol = NULL;
	}

	if (CDriverCanProtocol::Protocol_RL == proto_type)
	{
		//_driverProtocol = new CProtocolRL;
	}	
	else if (CDriverCanProtocol::Protocol_CanOpen == proto_type)
	{
		_driverProtocol = new CProtocolCopleyCAN;
	}
	else if (CDriverCanProtocol::Protocol_Castdservo == proto_type)
	{
		//_driverProtocol = new CProtocolCastdservo;
	}
	else if (CDriverCanProtocol::Protocol_Keya == proto_type)
	{
		//_driverProtocol = new CProtocolKeya;
	}else if (CDriverCanProtocol::Protocol_CanOpenPosition == proto_type)
	{
		//_driverProtocol = new CProtocolCopleyCAN(CProtocolCopleyCAN::POSITION_MODE);
	}
	else if (CDriverCanProtocol::Protocol_HLS == proto_type)
	{
		//_driverProtocol = new CProtocolHLS();
	}
	else if (CDriverCanProtocol::Protocol_DeltaCanopen == proto_type)
	{
		//_driverProtocol = new CProtocolDeltaCANOpen();
	}
	else if (CDriverCanProtocol::Protocol_KincoCanopen == proto_type)
	{
		//_driverProtocol = new CProtocolKincoCANOpen();
	}
	else if(CDriverCanProtocol::Protocol_HlsCanopen == proto_type)
	{
		//_driverProtocol = new CProtocolHlsCANOpen();
	}else if(CDriverCanProtocol::Protocol_MotecCanopen == proto_type)
	{
		//_driverProtocol = new CProtocolMotecCANOpen();
	}else if(CDriverCanProtocol::Protocol_JingZhiCanopen == proto_type)
	{
		//_driverProtocol = new CProtocolJingZhiCanopen();
	}else if(CDriverCanProtocol::Protocol_MingZhiCanopen == proto_type)
	{
		//_driverProtocol = new CProtocolMingZhiCanopen();
	}
	
	else 
		return false;
	
	return true;

}

int CChassisDevice::setStart()
{
	int value;
	int status = sem_getvalue(&openSem, &value);
	if(status<0){
		printf("Error: chassis setStart can not get semophore value\r\n");
	}
	status = sem_post(&openSem);
	if(status!=0){
		printf("Error: chassis setStart can not post semophore value\r\n");
	}
    return 0;
}

void CChassisDevice::waitStart()
{
	int value;
	int status = sem_getvalue(&openSem, &value);
	if(status<0){
		printf("Error: chassis waitStart can not get semophore value\r\n");
	}
	status = sem_wait(&openSem);
	if(status!=0){
		printf("Error: chassis waitStart can not wait semophore value\r\n");
	}
	return;
}

int CChassisDevice::waitCmd()
{
	int value;
	int status = sem_getvalue(&cmdSem, &value);
	if(status<0){
		printf("Error: chassis waitCmd can not get semophore value\r\n");
	}
	status = sem_wait(&cmdSem);
	if(status!=0){
		printf("Error: chassis waitCmd can not wait semophore value\r\n");
	}
	if(bNewSpeedCmd)
	{
		bNewSpeedCmd = false;
		currentCmd = 1;
	}else if(bOdoCmd)
	{
		bOdoCmd = false;
		currentCmd = 2;
	}else if(bStateCmd)
	{
		bStateCmd = false;
		currentCmd = 3;
	}
	return currentCmd;
}

void CChassisDevice::newSpeedCmd()
{
	int value;
	int status = sem_getvalue(&cmdSem, &value);
	if(status<0){
		printf("Error: chassis setStart can not get semophore value\r\n");
	}
	bNewSpeedCmd = true;
	status = sem_post(&cmdSem);
	if(status!=0){
		printf("Error: chassis setStart can not post semophore value\r\n");
	}
}

void CChassisDevice::sendSpeedCmd()
{
	CMileage::WheelVel wheelVel;
	PlanarVel planarVel;
	planarVel.vx = ori_global_X;
	planarVel.vy = ori_global_Y;
	planarVel.w = ori_global_W;
	Mileage::Instance()->inverseKinematicsTrans(planarVel, wheelVel);
	Mileage::Instance()->setVelocities(wheelVel);
	int i;
	for(i = 0; i < Mileage::Instance()->num_of_wheel(); i++)
	{
		struct can_msg_s TxMessage;
		_driverProtocol->encode(i, DRV_CMD_TARGET_SPEED, Mileage::Instance()->getVel(i), TxMessage);
		printf("send speed down\r\n");
		//_canBaseRouter.putMsg(TxMessage);
	}
}
