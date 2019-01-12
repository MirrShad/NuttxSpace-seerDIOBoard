#ifndef _FW_GYRO_H_
#define _FW_GYRO_H_


#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

//uart reicer flag
#define b_uart_head  0x80  //收到A5 头 标志位
#define b_rx_over    0x40  //收到完整的帧标志
// USART Receiver buffer
#define RX_BUFFER_SIZE 50 //接收缓冲区字节数

class CGyroDevice
{
public:
  void Decode_frame(unsigned char date);
	void rstTxPowerOff();
	void rstTxPowerOn();
private:
	//解算后的角度值
	float yaw;  //偏航角
	float pitch;//俯仰
	float roll; //滚转
	float alt;  //高度
	float tempr;//温度
	float press;//气压

	float baseYaw;
	
	//ADC值
	int16_t ax;//加速度计
	int16_t ay;
	int16_t az;
	int16_t gx;//陀螺仪
	int16_t gy;
	int16_t gz;
	
	int16_t off_gx_;
	int16_t off_gy_;
	int16_t off_gz_;
	
	//ADC值
	int16_t prev_ax;//加速度计
	int16_t prev_ay;
	int16_t prev_az;
	int16_t prev_gx;//陀螺仪
	int16_t prev_gy;
	int16_t prev_gz;
	
	uint16_t gyroFreezeCounter_;
	uint16_t accFreezeCounter_;
	
	uint8_t reCaliCounter_;
	enum E_InstallType
	{
		E_UNKNOW = 0,
		E_X_AXIS = 1,
		E_Y_AXIS = 2,
		E_Z_AXIS = 3
	};
	
	E_InstallType IMU_installType_;

	uint16_t ReceivedChar;
	bool isFirstIn;
	//Timer timeOut;
	bool iapFlag_;

	float m_lsb;
	bool m_set_lsb_flag;


  unsigned char rx_buffer[RX_BUFFER_SIZE]; //接收数据缓冲区
  unsigned char rx_wr_index = 0; //缓冲写指针
  unsigned char RC_Flag = 0;  //接收状态标志字节
  unsigned char Sum_check(void);
  void UART2_Get_Motion(void);
  void UART2_Get_IMU(void);
  void UART2_CommandRoute(void);
  uint16_t version[4];

  void report();
};

#endif
