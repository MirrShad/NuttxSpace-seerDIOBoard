#include "Gyro.h"
#include <pb_encode.h>
#include "message_imu.pb.h"

#include <errno.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

namespace{
	// version head
	const int16_t VERSION_HEAD = 0xA5A5;
	const int16_t VERSION_F103 = 0x01;
	const int16_t VERSION_F407 = 0x02;
	// version head

	const uint8_t GYRO_CMD_LENGTH = 6;
	const unsigned char set_zero_buffer[GYRO_CMD_LENGTH] = {0xA5,0x5A,0x04,0xEE,0xF2,0xAA};//只标零到ram里面
	const uint8_t GYRO_INIT_CMD_LENGTH = 9;
	const unsigned char gyro_init_data_buffer[GYRO_INIT_CMD_LENGTH] = {0xA5, 0x5A, 0x07, 0xB3, 0x01, 0x05, 0x07, 0xC7, 0xAA};		//to 50Hz
//	const unsigned char gyro_init_data_buffer[GYRO_INIT_CMD_LENGTH] = {0xA5, 0x5A, 0x07, 0xB3, 0x01, 0x07, 0x07, 0xC9, 0xAA}; //to 20Hz
	const unsigned char gyro_iap_cmd[GYRO_CMD_LENGTH] = {0xA5,0x5A,0x04,0x08,0x0c,0xAA};
	const unsigned char gyro_reset_cmd[GYRO_CMD_LENGTH] = {0xA5,0x5A,0x04,0x09,0x0d,0xAA};
	const unsigned char mpu6050_repower_cmd[GYRO_CMD_LENGTH] = {0xA5,0x5A,0x04,0x06,0x0A,0xAA};
	uint8_t gyro_set_LSB_cmd[10] = {0xA5, 0x5A, 0x08, 0x0E, 0x00, 0x00, 0x00, 0x00, 0x14, 0xAA};
	const uint8_t OMEGA_DOWNWARD_LEN = 10;
	uint8_t gyro_omega_downward[OMEGA_DOWNWARD_LEN] = {0xA5, 0x5A, 0x08, 0x0A, 0x00, 0x00, 0x00, 0x00, 0x12, 0xAA};

	volatile unsigned char rx_buffer[RX_BUFFER_SIZE]; //接收数据缓冲区
	volatile unsigned char rx_wr_index = 0; //缓冲写指针
	volatile unsigned char RC_Flag = 0;  //接收状态标志字节

//	Timer caliT(20,20);
	int cycle = 0;
	const int CALIBRATION_CYCLE = 400;// 50Hz , 1000 -> 20s
	const int CALIBRATION_NO_MOVE_CYCYE = 395;// 和上面这个值有联系的！！！！！！！！！！！！
	const int FINISH_BAN_CYCLE = 150;
	const int MANUAL_CALIBRATION_CYCLE = 100;
	bool finishBan = false;//陀螺仪标零后有一段结束屏蔽时间
	int finishBanCycle = 0;

	bool isManualCali = false;
	float beforeCaliYawRecord = 0;

	bool isAck = false;
	
	uint8_t iapHostIp[4];
	uint16_t iapHostPort;

	bool isImuReady = false;
	bool isMotionReady = false;

	char *buf;
}

int CGyroDevice::doInit()
{
	pthread_mutex_init(&gyro_mut,NULL);

	char *devpath;
	devpath = "/dev/ttyS1";
	buf = (char *)malloc(11520);
	if (buf == NULL)
    {
      fprintf(stderr, "GYRO ERROR: malloc failed: %d\n", errno);
      return 1;
    }
	usart_fd = open(devpath, O_RDWR);
	if (usart_fd < 0)
    {
      fprintf(stderr, "GYRO ERROR: open failed: %d\n", errno);
      return 1;
    }
	if(write(usart_fd,gyro_init_data_buffer,GYRO_INIT_CMD_LENGTH)<0)
    {
      fprintf(stderr, "GYRO ERROR: gyro write failed: %d\n", errno);
	  return 1;
    }
	bInited = true;
	return 0;
}

void CGyroDevice::rcvFrame()
{
	if(pthread_mutex_lock(&gyro_mut)!=0)
	{
		printf("ERROR Gyro pthread_mutex_lock failed\n");
	}

	ssize_t n = read(usart_fd, buf, 11520);
    if (n == 0)
    {
		printf("read 0 bytes\r\n");
    }
    else if (n < 0)
    {
        printf("read failed: %d\n", errno);
        fflush(stdout);
    }
    else
    {
		int i;
    	for (i = 0; i < (int)n; i++)
        {
          	//printf("get %d 0x%x \r\n", i, buf[i]);
	        Decode_frame(buf[i]);
        }
	}

	if ((pthread_mutex_unlock(&gyro_mut)) != 0)
    {
      printf("ERROR Gyro pthread_mutex_unlock failed\n");
    }
}

int CGyroDevice::gyroCmdDispatcher(uint8_t* msg,uint16_t len)
{
	if(pthread_mutex_lock(&gyro_mut)!=0)
	{
		printf("ERROR Gyro pthread_mutex_lock failed\n");
	}

	printf("gyro receive cmd\r\n");

	if ((pthread_mutex_unlock(&gyro_mut)) != 0)
    {
      printf("ERROR Gyro pthread_mutex_unlock failed\n");
    }
}

void CGyroDevice::Decode_frame(unsigned char data){
	if(data==0xa5)
	{
		RC_Flag|=b_uart_head; //如果接收到A5 置位帧头标专位
		rx_buffer[rx_wr_index++]=data; //保存这个字节.
	}
	else if(data==0x5a)
	{
		if(RC_Flag&b_uart_head) //如果上一个字节是A5 那么认定 这个是帧起始字节
		{
			rx_wr_index=0;  //重置 缓冲区指针
			RC_Flag&=~b_rx_over; //这个帧才刚刚开始收
		}
		else //上一个字节不是A5
			rx_buffer[rx_wr_index++]=data;
		RC_Flag&=~b_uart_head; //清帧头标志
	}
	else
	{
		rx_buffer[rx_wr_index++]=data;
		RC_Flag&=~b_uart_head;
		if(rx_wr_index==rx_buffer[0]) //收够了字节数.
		{
			RC_Flag|=b_rx_over; //置位 接收完整的一帧数据
			UART2_CommandRoute(); //立即提取数据。
		}
	}

	if(rx_wr_index==RX_BUFFER_SIZE) 
		rx_wr_index--;
}

void CGyroDevice::UART2_CommandRoute(void)
{
	if(RC_Flag&b_rx_over)
	{  //已经接收完一帧?
		RC_Flag&=~b_rx_over; //清标志先
		if(Sum_check())
		{
			//timeOut.reset();
			//GlobalData::RptDat.gyroDat.setEnable(true);
			/*if(Message::Instance()->checkErrOccur(CODE_IMU_ERRO))
			{
				Message::Instance()->clearErr(CODE_IMU_ERRO);
				printf("gyro error cleared\r\n");
			}*/
			//校验通过
			if(rx_buffer[1]==0xA1)
			{ //UART2_ReportIMU 的数据
				//setTimestampe();
				UART2_Get_IMU();	//取数据
				isImuReady = true;
				if (isImuReady && isMotionReady)report();
				//falldownDetect2();
			}
			else if(rx_buffer[1]==0xA2)
			{ //UART2_ReportMotion 的数据
				UART2_Get_Motion();	 //取数据
				isMotionReady = true;
				if(ax == prev_ax && ay == prev_ay && az == prev_az)
					++accFreezeCounter_;
				else
					accFreezeCounter_ = 0;

				if(gx == prev_gx && gy == prev_gy && gz == prev_gz)
					++gyroFreezeCounter_;
				else
					gyroFreezeCounter_ = 0;

				prev_ax = ax;//加速度计
				prev_ay = ay;
				prev_az = az;
				prev_gx = gx;//陀螺仪
				prev_gy = gy;
				prev_gz = gz;

				if(accFreezeCounter_ > 100 || gyroFreezeCounter_ > 500)
				{
					printf("IMU data freezed: accCnt = %d, gyroCnt = %d",
						accFreezeCounter_, gyroFreezeCounter_);
					printf("a(x,y,z) = (%d, %d, %d); g(x,y,z) = (%d, %d, %d); yaw = %f",
						ax, ay, az, gx, gy, gz, yaw);
					rstTxPowerOff();
				}
			}
			else if(rx_buffer[1] == gyro_init_data_buffer[3])
			{
				printf("Gyro init success...");
			}else if(rx_buffer[1] == 0x38)
			{
//				Console::Instance()->printf("Gyro downward rotate\r\n");
			} else if (rx_buffer[1] == 0x39)
			{
				m_set_lsb_flag = true;
			}
			else if(rx_buffer[1] == 0x0B)
			{
				if (true == finishBan)
				{
					isAck = true;
					beforeCaliYawRecord = yaw;
					// Message::Instance()->postMsg("None block calibration ACK");
				}else
				{
					// Message::Instance()->postMsg("Give up gyro cali ack");
				}
			}
			else
			{
				//Message::Instance()->postMsg("Unknown rxbuffer[1] = 0x%02X", rx_buffer[1]);
			}

		}//校验是否通过?
		else
		{
			printf("gyro decode sum check failed...");
		}

	}
}

unsigned char CGyroDevice::Sum_check(void)
{
	unsigned char i;
	unsigned int checksum=0;
	for(i=0;i<rx_buffer[0]-2;i++)
		checksum+=rx_buffer[i];
	if((checksum%256)==rx_buffer[rx_buffer[0]-2])
		return(0x01); //Checksum successful
	else
		return(0x00); //Checksum error
}

void CGyroDevice::UART2_Get_IMU(void)
{
	int16_t temp;

	temp = 0;
	temp = rx_buffer[2];
	temp <<= 8;
	temp |= rx_buffer[3];
	if(temp&0x8000){
		temp = 0-(temp&0x7fff);
	}else temp = (temp&0x7fff);
	yaw=(float)temp / 10.0f; //偏航角

	temp = 0;
	temp = rx_buffer[4];
	temp <<= 8;
	temp |= rx_buffer[5];
	if(temp&0x8000){
		temp = 0-(temp&0x7fff);
	}else temp = (temp&0x7fff);
	pitch=(float)temp / 10.0f;//俯仰

	temp = 0;
	temp = rx_buffer[6];
	temp <<= 8;
	temp |= rx_buffer[7];
	if(temp&0x8000){
		temp = 0-(temp&0x7fff);
	}else temp = (temp&0x7fff);
	roll=(float)temp / 10.0f;//滚转

	temp = 0;
	temp = rx_buffer[8];
	temp <<= 8;
	temp |= rx_buffer[9];

	if (VERSION_HEAD == temp)
	{
		temp = 0;
		temp = rx_buffer[10];

		if (VERSION_F103 == temp)
		{
			version[0] = 103;
		}
		else if (VERSION_F407 == temp)
		{
			version[0] = 407;
		}
		else {
			version[0] = 0;
		}

		if (0 != version[0])
		{
			version[1] = rx_buffer[11];
			version[2] = rx_buffer[12];
			version[3] = rx_buffer[13];
		}
	}
}

void CGyroDevice::UART2_Get_Motion(void)
{
	int16_t temp;

	temp = 0;
	temp = rx_buffer[2];
	temp <<= 8;
	temp |= rx_buffer[3];
	if(temp&0x8000){
		temp = 0-(temp&0x7fff);
	}else temp = (temp&0x7fff);
	ax=temp;//加速度计 X轴的ADC值

	temp = 0;
	temp = rx_buffer[4];
	temp <<= 8;
	temp |= rx_buffer[5];
	if(temp&0x8000){
		temp = 0-(temp&0x7fff);
	}else temp = (temp&0x7fff);
	ay=temp;//加速度计 Y轴的ADC值

	temp = 0;
	temp = rx_buffer[6];
	temp <<= 8;
	temp |= rx_buffer[7];
	if(temp&0x8000){
		temp = 0-(temp&0x7fff);
	}else temp = (temp&0x7fff);
	az=temp;//加速度计 Z轴的ADC值

	temp = 0;
	temp = rx_buffer[8];
	temp <<= 8;
	temp |= rx_buffer[9];
	if(temp&0x8000){
		temp = 0-(temp&0x7fff);
	}else temp = (temp&0x7fff);
	gx=temp;//陀螺仪 X轴的ADC值

	temp = 0;
	temp = rx_buffer[10];
	temp <<= 8;
	temp |= rx_buffer[11];
	if(temp&0x8000){
		temp = 0-(temp&0x7fff);
	}else temp = (temp&0x7fff);
	gy=temp;//陀螺仪 Y轴的ADC值

	temp = 0;
	temp = rx_buffer[12];
	temp <<= 8;
	temp |= rx_buffer[13];
	if(temp&0x8000){
		temp = 0-(temp&0x7fff);
	}else temp = (temp&0x7fff);
	gz=temp;//陀螺仪 Z轴的ADC值

	temp = 0;
	temp = rx_buffer[14];
	temp <<= 8;
	temp |= rx_buffer[15];
	if(temp&0x8000){
		temp = 0-(temp&0x7fff);
	}else temp = (temp&0x7fff);
	off_gx_=temp;//magnetic X轴的ADC值
	
	temp = 0;
	temp = rx_buffer[16];
	temp <<= 8;
	temp |= rx_buffer[17];
	if(temp&0x8000){
		temp = 0-(temp&0x7fff);
	}else temp = (temp&0x7fff);
	off_gy_=temp;//magnetic Y轴的ADC值
	
	temp = 0;
	temp = rx_buffer[18];
	temp <<= 8;
	temp |= rx_buffer[19];
	if(temp&0x8000){
		temp = 0-(temp&0x7fff);
	}else temp = (temp&0x7fff);
	off_gz_=temp;//magnetic Z轴的ADC值
}

void CGyroDevice::rstTxPowerOff()
{
	// SRC-2000 re-power gyro mpu6050
	//while(0 != gyroUsart.get_BytesInTxFifo()){}
	//gyroUsart.send_Array((uint8_t*)mpu6050_repower_cmd, GYRO_CMD_LENGTH);	
	printf("send repower command to gyro!!");
}

void CGyroDevice::rstTxPowerOn()
{
	printf("Gyro poweron");
	//Message::Instance()->postMsg("Gyro poweron");
}

void CGyroDevice::report()
{
	rbk_protocol_Message_IMU pbMsg = rbk_protocol_Message_IMU_init_zero;
	
	pbMsg.acc_x = -9.8f*ax/16384.0f;
	pbMsg.acc_y = 9.8f*ay/16384.0f;
	pbMsg.acc_z = 9.8f*az/16384.0f;
	printf("gyro get Message 2: %d,%d,%d\r\n",ax,ay,az);
}

