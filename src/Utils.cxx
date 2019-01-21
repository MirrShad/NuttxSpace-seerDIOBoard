#include "Utils.h"

namespace Utils{
	namespace Math{
		float PI = 3.1415926;
		float Epsilon = 0.00001;
	}

	float Fabs(float v)
	{
		return v >= 0 ? v : (-1.0 * v);
	}

	float Normalize(float angle){
		float tmp2pi = 2*Math::PI;
		if( Fabs(angle) > tmp2pi ) { // 取余
			angle = angle - long(angle / tmp2pi) * tmp2pi;
		}

		while( angle > Math::PI ) {
			angle -= tmp2pi;
		}
		while( angle <= -Math::PI ) {
			angle += tmp2pi;
		}
		return angle;
	}

	float Deg2Rad(float angle)
	{
		return angle * Math::PI / 180;
	}

	uint16_t checkCRC8(uint16_t *ptr,uint32_t len)
	{
		uint16_t crc = 0;
		while(len--)
		{
			crc ^= *ptr++;
			for(int i = 0;i < 8;i++)
			{
				if(crc & 0x01)
				{
					crc = (crc >> 1) ^ 0x8C;
				}else crc >>= 1;
			}
		}
		return crc;
	}

	uint16_t checkCRC8_2(uint16_t* data,int datalen)         // computes crc value
	{
		uint16_t c;
		uint16_t CRC = 0;
		for (int j = 0; j < datalen; j++)
		{
			c = data[j];
			CRC ^= c;
			for(int i = 0; i < 8; i++)
				if(CRC & 0x80)
					CRC = (CRC << 1) ^ 0x07;
				else
					CRC <<= 1;
			CRC &= 0xff;
		}
		return CRC;
	};
}
