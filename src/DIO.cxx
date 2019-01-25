#include "DIO.h"
#include <pb_encode.h>
#include "message_controller.pb.h"

#include <errno.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "Utils.h"
#include <pb_encode.h>

void CDIODevice::report()
{
	reportLock();
	uint32_t msgEnum = CReportData::EGyro;
	memcpy(CReporter_base::transBuff_, &msgEnum, sizeof(uint32_t));

	rbk_protocol_Message_Controller pbMsg = rbk_protocol_Message_Controller_init_zero;
	
	pb_ostream_t stream = pb_ostream_from_buffer(
		CReporter_base::transBuff_ + sizeof(uint32_t),
		CReporter_base::TRANS_BUFF_LEN - sizeof(uint32_t));

	pbMsg.temp = 0;
	pbMsg.humi = 0;
	pbMsg.voltage = 0;
	pbMsg.emc = false;
    pbMsg.driverEmc = false;
    pbMsg.brake = false;
    pbMsg.manualCharge = false;
    pbMsg.autoCharge = false;
    pbMsg.electric = false;

	bool status;
	status = pb_encode(&stream, rbk_protocol_Message_Controller_fields, &pbMsg);
	uint16_t pbMsgLen = stream.bytes_written;
	if (!status)
	{
			printf("Gyro Encoding failed: %s\n", PB_GET_ERROR(&stream));
	}

	uploadRbk(pbMsgLen + sizeof(uint32_t));
	reportUnlock();
	return ;
}
