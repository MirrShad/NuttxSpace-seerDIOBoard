#include "chassis.h"
#include "message_navigation.pb.h"
#include <pb_decode.h>
#include "ProtocolCopleyCAN.h"
#include "Mileage.h"
#include "ChassisCmdType.h"
#include <nuttx/can/seer_can.h>
#include <fcntl.h>
#include <errno.h>

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

int CChassisDevice::doInit()
{
	while(!Mileage::Instance()->isInited())
		sleep(1);

	can1_fd = open("/dev/can1", O_RDWR);
    if (can1_fd < 0)
    {
      int errcode = errno;
      printf("chassis: ERROR: Failed to open %s: %d\n",
             "/dev/can1", errcode);
      return 1;
    }
	FAR struct can_msg_s TxMessage;
	printf("chassis get init message\r\n");
	while(_driverProtocol->getInitMsg(TxMessage))
	{
		//printf("send init msg id 0x%x, dlc %x, data 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x",TxMessage.cm_hdr.ch_id,TxMessage.cm_hdr.ch_dlc,TxMessage.cm_data[0],TxMessage.cm_data[1],TxMessage.cm_data[2],TxMessage.cm_data[3],TxMessage.cm_data[4],TxMessage.cm_data[5],TxMessage.cm_data[6],TxMessage.cm_data[7]);
		write(can1_fd, &TxMessage, 1);
	}
	/*
	FAR struct can_msg_s testMsg;
	testMsg.cm_hdr.ch_id = 0x702;
	testMsg.cm_hdr.ch_rtr = 1;
	testMsg.cm_hdr.ch_extid = 1;
	testMsg.cm_hdr.ch_dlc = 4;
	testMsg.cm_data[0] = 0x11;
	testMsg.cm_data[1] = 0x22;
	testMsg.cm_data[2] = 0x33;
	testMsg.cm_data[3] = 0x44;
	write(fd, &testMsg, 1);*/
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
		_driverProtocol = new CProtocolCopleyCAN(CProtocolCopleyCAN::POSITION_MODE);
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

int CChassisDevice::waitCmdRet(motorDriverCmdType cmdType)
{
	FAR struct can_msg_s RxMessage;
	int status;
	int i;
	
	for(i = 0; i < Mileage::Instance()->num_of_wheel(); i++)
	{
		do{
			//printf("wait again \r\n");
			status = read(can1_fd, &RxMessage, 1);
			if(status == 0)
			{
				//printf("get Msg ID:0x%x Data:0x%x 0x%x 0x%x 0x%x\r\n",RxMessage.cm_hdr.ch_id,RxMessage.cm_data[0],RxMessage.cm_data[1],RxMessage.cm_data[2],RxMessage.cm_data[3]);
				int temp = 0;
				if(RxMessage.cm_hdr.ch_id == 0x581)
					temp = 0;
				else if(RxMessage.cm_hdr.ch_id == 0x582)
					temp = 1;
				_driverProtocol->decode(_retTyp[temp], _retVal[temp], RxMessage);
				if(_retTyp[temp] == cmdType)
				{
					break;
				}

			}
			else if(status==EINTR && bNewSpeedCmd)
				;//printf("interrupt by an speed cmd ignore it\r\n");
			else
			{
				printf("Error: chassis wait QueryRet get errno %d\r\n",-1*status);
				return -1;
			}
		}while(true);
	}
	//printf("get full query odo response\r\n");
}

void CChassisDevice::report()
{
	for(int i = 0; i < Mileage::Instance()->num_of_wheel(); i++)
	{
		Mileage::Instance()->setPos(i, getRetVal(i));
	}
	CMileage::iWheelPos TempWheelPos;		
	Mileage::Instance()->getPositions(TempWheelPos);
	if (0 == Mileage::Instance()->doCalc(TempWheelPos))
	{	
		//printf("chassis odo report\r\n");
		Mileage::Instance()->report();
	}	
}

int CChassisDevice::waitCmd()
{
	int value;
	int status = sem_getvalue(&cmdSem, &value);
	if(status<0){
		printf("Error: chassis waitCmd can not get semophore value\r\n");
	}
	status = sem_wait(&cmdSem);
	if(status!=0)
	{
		if(errno==EINTR && bOdoCmd);
		else
		{
			printf("Error: chassis waitCmd can not wait semophore value errno %d\r\n",errno);
			return -1;
		}
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
		printf("Error: chassis new speed cmd can not get semophore value\r\n");
	}
	bNewSpeedCmd = true;
	status = sem_post(&cmdSem);
	if(status!=0){
		printf("Error: chassis new speed cmd can not post semophore value\r\n");
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
		FAR struct can_msg_s TxMessage;
		_driverProtocol->encode(i, DRV_CMD_TARGET_SPEED, Mileage::Instance()->getVel(i), TxMessage);
		//printf("send speed down\r\n");
		write(can1_fd, &TxMessage, 1);
	}
}

void CChassisDevice::sendQueryCmd()
{
	int i;
	for(i = 0; i < Mileage::Instance()->num_of_wheel(); i++)
	{
		FAR struct can_msg_s TxMessage;
		_driverProtocol->encode(i, DRV_CMD_ACTURAL_POS, Mileage::Instance()->getVel(i), TxMessage);
		//printf("send speed down\r\n");
		write(can1_fd, &TxMessage, 1);
	}
}

void CChassisDevice::queryOdoCmd()
{
	bOdoCmd = true;
}

