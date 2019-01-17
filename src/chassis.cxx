#include "chassis.h"
#include "message_navigation.pb.h"
#include <pb_decode.h>

int CChassisDevice::chassisCmdHandler(uint8_t* pbData,uint16_t len)
{
    bool status;
    rbk_protocol_Message_NavSpeed pbMsg = rbk_protocol_Message_NavSpeed_init_zero;
    pb_istream_t stream = pb_istream_from_buffer(pbData,len);
    status = pb_decode(&stream,rbk_protocol_Message_NavSpeed_fields,&pbMsg);
    if(!status)
        printf("Decode fail in Chassis.cxx\r\n");   
}

bool CChassisDevice::setProtocol(const uint8_t)
{

}

int CChassisDevice::setStart()
{
    return 0;
}
