#ifndef _FW_UTILS_H_
#define _FW_UTILS_H_
#include "stdint.h"

namespace Utils{
	namespace Math{
		extern float Epsilon;
	}
	float Normalize(float angle);
	float Deg2Rad(float angle);
	float Fabs(float v);
	uint16_t checkCRC8(uint16_t *ptr,uint32_t len);	//crc8
	uint16_t checkCRC8_2(uint16_t* data,int datalen);
}

#endif
