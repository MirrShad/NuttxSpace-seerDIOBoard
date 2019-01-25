#include "Mileage.h"
#include <stdio.h>
//#include <math.h>
#include <pb_encode.h>
#include <pb_decode.h>
#include "message_chassisconfig.pb.h"
#include "ProtocolCanDriver.h"
#include "chassis.h"
#include "Utils.h"
#include <math.h>
#include "message_odometer.pb.h"
#include <sys/time.h>
#include "sntpSynchr.h"

#define PI 3.1415926

namespace{
	int stopCycle = 0;
	const int STOP_CYCLE_LIMIT = 20;
	float critical_delta = 1.1;
	const int ENCODER_MAX_DIFF = 2000000;
	struct timeval xTimeLast;
    	struct timeval xTimeCur;

}
int CMileage::doInit()
{
	reset();
	return 0;
}

/*void CMileage::resetEncoderLog()
{
	static_p.reset();
}
*/
void CMileage::reset()
{
	m_x = 0.0f;
	m_y = 0.0f;
	m_angle = 0.0f;
	//m_steerangle = 0.0f;
	firstIn = true;
}

int CMileage::doCalc(const iWheelPos& p)
{
	// 1.get current wheel positions : raw single counts
	uint8_t num_of_wheel = Mileage::Instance()->num_of_wheel();
	if (true == firstIn)
	{
		firstIn = false;
		for(int i = 0; i < num_of_wheel; i++)
			static_p.at(i) = p.at(i);
		return 1;
	}

	WheelPos wheelPos;		// delta_pos for short time
	bool is_wheel_rotate = false;
	for(int i = 0; i < num_of_wheel; i++)
	{
		//data filter.
		//give up encoder data with large noise.
		if(Utils::Fabs(p.at(i) - static_p.at(i)) > ENCODER_MAX_DIFF)
		{
			printf("encoder %d value jumps too large : %d !!!", i, p.at(i));
			for(int j = 0; j <num_of_wheel; j++)
				static_p.at(j) = p.at(j);
			return 2;
		}

		wheelPos.at(i) = p.at(i) - static_p.at(i);
		static_p.at(i) = p.at(i);
		#if 0
		Console::Instance()->printf("wheel%d: w = %f, p = %d, sp = %d\r\n",
			wheelPos.at(i), p.at(i), static_p.at(i));
		#endif

		if(Utils::Fabs(wheelPos.at(i)) > critical_delta)
		{
			is_wheel_rotate = true;
			m_stopped = 0;
			stopCycle = 0;
		}
	}
/*
	if(false == is_wheel_rotate)
	{
		stopCycle = (stopCycle >= STOP_CYCLE_LIMIT) ? STOP_CYCLE_LIMIT : (stopCycle + 1);
		if (stopCycle >= STOP_CYCLE_LIMIT)
			m_stopped = 1;
	}
*/
	// 2.do forward kinematics transformation
	PlanarPos planarPos;
	forwardKinematicsTrans(wheelPos, planarPos);

	float tmp_dx = planarPos.x * cos(m_angle) - planarPos.y * sin(m_angle);
	float tmp_dy = planarPos.x * sin(m_angle) + planarPos.y * cos(m_angle);
	float tmp_dtheta = planarPos.theta;
	
	m_delta_x = planarPos.x;
	m_delta_y = planarPos.y;
	m_delta_angle = tmp_dtheta;

	m_x += tmp_dx;
	m_y += tmp_dy;
	m_angle += tmp_dtheta;
	

	while (m_angle <= -PI) {
		m_angle += 2 * PI;
	}
	while (m_angle > PI) {
		m_angle -= 2 * PI;
	}
	//std::cout<<m_x<<" "<<m_angle*180/PI<<std::endl;
	return 0;
}
/*
void CMileage::getData(float& x, float& y, float& angle)
{
	x = m_x;					// m -> m
	y = m_y;					// m -> m
	angle = m_angle;			// rad -> rad
}
*/
void CMileage::forwardKinematicsTrans(const WheelPos& wheelPos, PlanarPos& planarPos)
{
	if(DOUBLE_DIFF == Mileage::Instance()->chassisType())
		forwardKinematicsTransDiff(wheelPos, planarPos);
/*
	else if(TRICYCLE == Mileage::Instance()->chassisType())
	{
		forwardKinematicsTransTricycle(wheelPos, planarPos);
	}

	else if(QUAD_MACNUM == Mileage::Instance()->chassisType())
	{
		forwardKinematicsTransMecanum(wheelPos, planarPos);
	}

	else
	{
		seer_assert(false);
	}*/
}

/// Inverse Kinematics
void CMileage::inverseKinematicsTrans(const PlanarVel& planarVel, WheelVel& wheelVel) {

	
	inverseKinematicsTransDiff(planarVel, wheelVel);


	//confirm there is no wheel overspeed
	bool overspeedFlag = false;
	float maxSpeed = 0.0f;
	for(int i = 0; i < num_of_wheel(); i++)
	{
		float tempVel = wheelVel.at(i)>0?wheelVel.at(i):(-1*wheelVel.at(i));
		if(tempVel > m_motor_max_speed)
		{
			if(tempVel > maxSpeed)
			{
				maxSpeed = tempVel;
			}
			overspeedFlag = true;
		}

	}
#define ENABLE_SPEED_LIMIT 1
	if(overspeedFlag)
	{
		printf("Overspeed !!!!!\r\n");
		//Message::Instance()->postErrMsg(CODE_MOTO_OVER,"Error in Mileage.Motor speed overflow.");
#if ENABLE_SPEED_LIMIT
		for(int i = 0; i < num_of_wheel(); i++)
		{
			wheelVel.at(i) *= m_motor_max_speed / maxSpeed;
		}
#endif
	}

	/*else if (true == Message::Instance()->checkErrOccur(CODE_MOTO_OVER))
		Message::Instance()->clearErr(CODE_MOTO_OVER);*/

}


/// Forward Kinematics - Diff
void CMileage::forwardKinematicsTransDiff(const WheelPos& wheelPos, PlanarPos& planarPos)
{
	float l_wheel_pos = wheelPos.at(0) * m_scale[0]/ m_pos_reduce_rate;
	float r_wheel_pos = wheelPos.at(1) * m_scale[1]/ m_pos_reduce_rate;

	planarPos.x = (r_wheel_pos + l_wheel_pos) * 0.5f;
	planarPos.y = 0.0f;
	planarPos.theta = (r_wheel_pos - l_wheel_pos) / m_e;

	return ;
}

/// Inverse Kinematics - Diff
void CMileage::inverseKinematicsTransDiff(const PlanarVel& planarVel, WheelVel& wheelVel)
{
	const float cur_scale = m_vel_reduce_scale;

	const float transtionalSpeed = planarVel.vx;	// m/s
	const float rotationalSpeed = planarVel.w; // rad/s

	float l_wheel_vel = (2 * transtionalSpeed - m_e * rotationalSpeed) * 0.5f;	// m/s
	float r_wheel_vel = (2 * transtionalSpeed + m_e * rotationalSpeed) * 0.5f;	// m/s

	// calc wheel velocity
	wheelVel.at(0) = cur_scale * l_wheel_vel / (m_scale[0]);
	wheelVel.at(1) = cur_scale * r_wheel_vel / (m_scale[1]);

	return ;
}
/*
//	alpha = (pi/6)
//  S <=> sin(alpha)
//  C <=> cos(alpha)
//
//  --                                 -- -  -	 -  -
//  | -.5/(S+1)   -.5/(S+1)   1/(S+1)   | |V0|   |Vx|
//  | .5/C        -.5/C       0         | |V1| = |Vy|
//  | .5/L/(S+1)  .5/L/(S+1)  S/L/(S+1) | |V2|   |W |
//  --                                 -- -  -   -  -
//
void CMileage::forwardKinematicsTransTricycle(const WheelPos& wheelPos, PlanarPos& planarPos)
{
	float w_pos[3];
	const float L = m_e;
	// alpha = pi/6
	const float inv_cos_k = 1.1547005384f;					// 1/(cos(alpha))
	const	float inv_sin_k1 = 0.66666666666f;				// 1/(sin(alpha) + 1)
	const float sin_k = 0.5f;								// sin(alpha)
	for(int i = 0; i < num_of_wheel(); i++)
	{
		//calc from motor axis pos to wheel axis pos
		w_pos[i] = wheelPos.at(i) * m_scale[i]/ m_pos_reduce_rate;
	}
	planarPos.y 		= -0.5f*inv_sin_k1*w_pos[0] 	- 0.5f*inv_sin_k1*w_pos[1]		+ inv_sin_k1*w_pos[2];
	planarPos.x 		= 0.5f*inv_cos_k*w_pos[0] 		- 0.5f*inv_cos_k*w_pos[1];
	planarPos.theta 	= -0.5f*inv_sin_k1/L*w_pos[0] 	- 0.5f*inv_sin_k1/L*w_pos[1]	- sin_k*inv_sin_k1/L*w_pos[2];

	return;
}

void CMileage::inverseKinematicsTransTricycle(const PlanarVel& planarVel, WheelVel& wheelVel)
{
	const float cur_scale = m_vel_reduce_scale;
	const float chassis_radius = m_e;

	//alpha = pi/6
	const float SIN_ALPHA = 0.5f;							//sin(alpha)
	const float COS_ALPHA = 0.86602540378443f;				//cos(alpha)

	float wheel_vel[3];

	//calc the line velocity of wheel
	wheel_vel[0] = (COS_ALPHA* planarVel.vx - SIN_ALPHA*planarVel.vy - chassis_radius*planarVel.w);
	wheel_vel[1] = (-COS_ALPHA* planarVel.vx - SIN_ALPHA*planarVel.vy - chassis_radius*planarVel.w);
	wheel_vel[2] = (planarVel.vy - chassis_radius*planarVel.w);

	for(int i = 0; i < num_of_wheel(); i++)
	{
		//calc the angular velocity of motor axis
		wheelVel.at(i) = cur_scale * wheel_vel[i] / (m_scale[i]);
	}
}

//	从下往上看是O型的时候如下：
//                         ↑ x (0°)
//                    a/2  |
//                 --------|--------
//             1 | |       |       | | 0
//               | |       |       | |
//                 |      z|       |
// (90°) y <------[-]------*------[-]-------- (-90°)
//                 |       |(0,0,0)|
//             2 | | b/2   |       | | 3
//               | |       |       | |
//                 --------|--------
//                         |
//                         |
// 注意：从下往上看是X型的时候，需要左右ID对调（0,1对调，2,3对调）
// 可以理解为车子正反面对调了
void CMileage::forwardKinematicsTransMecanum(const WheelPos& wheelPos, PlanarPos& planarPos)
{
	float w_pos[4];

	float param_AplusB = m_e / 2.0f;

	for(int i = 0; i < num_of_wheel(); i++)
	{
		//calc from motor axis pos to wheel axis pos
		w_pos[i] = wheelPos.at(i) * m_scale[i]/ m_pos_reduce_rate;
	}

	planarPos.x 	= (w_pos[0] + w_pos[1] + w_pos[2] + w_pos[3]) / 4;
	planarPos.y 	= (w_pos[0] - w_pos[1] + w_pos[2] - w_pos[3]) / 4;
	planarPos.theta = (w_pos[0] - w_pos[1] - w_pos[2] + w_pos[3]) / ( 4 * param_AplusB);

	return;

}

void CMileage::inverseKinematicsTransMecanum(const PlanarVel& planarVel, WheelVel& wheelVel)
{
	float param_AplusB = m_e / 2.0f;

	float param_w_multipled_aplusb = planarVel.w * param_AplusB;

	float wheel_vel[4];

	//calc the line velocity of wheel
	wheel_vel[0] = planarVel.vx + planarVel.vy + param_w_multipled_aplusb;
	wheel_vel[1] = planarVel.vx - planarVel.vy - param_w_multipled_aplusb;
	wheel_vel[2] = planarVel.vx + planarVel.vy - param_w_multipled_aplusb;
	wheel_vel[3] = planarVel.vx - planarVel.vy + param_w_multipled_aplusb;

	for(int i = 0; i < num_of_wheel(); i++)
	{
		//calc the angular velocity of motor axis
		wheelVel.at(i) = m_vel_reduce_scale * wheel_vel[i] / (m_scale[i]);
	}
}

//vs = vx / cos(steer_angle)
void CMileage::inverseKinematicsTransCarLikeFixedWheelDrive_SI(const PlanarVel& planarVel, WheelVel& wheelVel)
{

}

void CMileage::inverseKinematicsTransCarLikeSteerWheelDrive_SI(const PlanarVel& planarVel, WheelVel& wheelVel)
{
	wheelVel.carLikeSteerAngle() = planarVel.steer_angle;

#define CMD_CHECK 1

	if(planarVel.steer_angle < (PI / 3) && planarVel.steer_angle > (PI / -3))
	{
		wheelVel.carLikeDriveWheel() = planarVel.vx / cos(planarVel.steer_angle);
#if CMD_CHECK
		float predictOmega = wheelVel.carLikeDriveWheel() / m_e * sin(planarVel.steer_angle);
		if(fabs(predictOmega - planarVel.w) > 0.1f)
		{
			Console::Instance()->printf("Dangerous! cmd: vx = %f, sa = %f, w = %f, predw = %f\r\n",
				planarVel.vx, planarVel.steer_angle, planarVel.w, predictOmega);
//			wheelVel.carLikeDriveWheel() = 0;
		}	
		
#endif
	} else
	{
		wheelVel.carLikeDriveWheel() = planarVel.w * m_e / sin(planarVel.steer_angle);
#if CMD_CHECK
		float predictVx = wheelVel.carLikeDriveWheel() * cos(planarVel.steer_angle);
		if(fabs(predictVx - planarVel.vx) > 0.1f)
		{
			Console::Instance()->printf("Dangerous! cmd: vx = %f, sa = %f, w = %f, predvx = %f\r\n",
				planarVel.vx, planarVel.steer_angle, planarVel.w, predictVx);
			wheelVel.carLikeDriveWheel() = 0;
		}
#endif
	}
	
	//confirm there is no wheel overspeed
	bool overspeedFlag = false;
	if (wheelVel.carLikeDriveWheel() > m_motor_max_speed)
	{
		overspeedFlag = true;
	}
	
	if (overspeedFlag)
	{
		wheelVel.carLikeDriveWheel() = m_motor_max_speed;
		Message::Instance()->postErrMsg(CODE_MOTO_OVER, "Error in Mileage.Motor speed overflow.");
	} else if (true == Message::Instance()->checkErrOccur(CODE_MOTO_OVER))
		Message::Instance()->clearErr(CODE_MOTO_OVER);
	
	//confirm there is no wheel angle over
	int overangleFlag = 0;
	if (wheelVel.carLikeSteerAngle() > m_max_steer_angle)
	{
		overangleFlag = 1;
	} else if (wheelVel.carLikeSteerAngle() < m_min_steer_angle)
	{
		overangleFlag = 2;
	}
	
	if (1 == overangleFlag)
	{
		Message::Instance()->postErrMsg(CODE_STEE_ANGL_OVER, "Error in Mileage. steer angle overflow angle = %f.", wheelVel.carLikeSteerAngle());
		wheelVel.carLikeSteerAngle() = m_max_steer_angle;
	} else if (2 == overangleFlag)
	{
		Message::Instance()->postErrMsg(CODE_STEE_ANGL_OVER, "Error in Mileage. steer angle overflow angle = %f.", wheelVel.carLikeSteerAngle());
		wheelVel.carLikeSteerAngle() = m_min_steer_angle;
	} else {
		Message::Instance()->clearErr(CODE_STEE_ANGL_OVER);
	}
}

void CMileage::forwardKinematicsTransCarLikeFixedWheelOdo_SI(const WheelPos& deltaOdoAbsAngle, PlanarPos& planarPos)
{
	planarPos.x = deltaOdoAbsAngle.carLikeDriveWheel();
	planarPos.y = 0;
	planarPos.theta = deltaOdoAbsAngle.carLikeDriveWheel() / m_e * tan(deltaOdoAbsAngle.carLikeSteerAngle());
}

void CMileage::forwardKinematicsTransCarLikeSteerWheelOdo_SI(const WheelPos& deltaOdoAbsAngle, PlanarPos& planarPos)
{
	planarPos.x = deltaOdoAbsAngle.carLikeDriveWheel() * cos(deltaOdoAbsAngle.carLikeSteerAngle());
	planarPos.y = 0;
	planarPos.theta = deltaOdoAbsAngle.carLikeDriveWheel() / m_e * sin(deltaOdoAbsAngle.carLikeSteerAngle());
	#if 0
	static int cnt = 0;
	if(++cnt > 100)
	{
		cnt = 0;
		Console::Instance()->printf("delta: vsdt = %fm, x = %f m, theta = %f rad\r\n",
			deltaOdoAbsAngle.carLikeDriveWheel(), planarPos.x, planarPos.theta);
	}
	#endif
}

void CMileage::initialzeParam(float e, float r, int32_t encoderLines, float reductionRatio, int32_t motorMaxSpeedRpm, int32_t inverse) { //reductionRatio should be float
	m_e = e/1000.0f;		// m
	m_r = r/1000.0f;		// m
//	float m_rr[4] = {0.1615, m_r, 0.07, 0.07};
#if 0
	for(int i = 0; i < CMileage::MAX_WHEEL_NUM; i++)
	{
		m_scale[i] = 2 * PI * m_rr[i];	// m
	}
#else
	for(int i = 0; i < CMileage::MAX_WHEEL_NUM; i++)
	{
		m_scale[i] = 2 * PI * m_r;	// m
	}
#endif


	m_vel_reduce_scale = encoderLines * 4 * reductionRatio * 10;
	m_pos_reduce_rate = encoderLines * 4 * reductionRatio;
	m_inverse = inverse;
	m_motor_max_speed_rpm = motorMaxSpeedRpm;
	m_motor_max_speed = 1.0 * motorMaxSpeedRpm / 60 * 4 * encoderLines * 10;
	m_encoder_lines = encoderLines;
	if (m_pos_reduce_rate < 10000) critical_delta = 0.1f;
	else if (m_pos_reduce_rate < 150000)critical_delta = 1.1f;
	else if (m_pos_reduce_rate < 350000)critical_delta = 2.1f;
	else critical_delta = 3.1f;
	return;
}

void CMileage::initialzeParam4cp(const float& e, const float (&r)[4], const int32_t& encoderLines, const float& reductionRatio, const int32_t& motorMaxSpeedRpm, const int32_t& inverse) {
	// transformation parameters
	m_e = e/1000.0f;		// m
	for(int i = 0; i < CMileage::MAX_WHEEL_NUM; i++)
	{
		m_scale[i] = 2.0f * PI * r[i] / 1000.0f;	// m
	}

	m_vel_reduce_scale = encoderLines * 4 * reductionRatio * 10;
	m_reductionRatio = reductionRatio;
	m_pos_reduce_rate = encoderLines * 4 * reductionRatio;
	m_inverse = inverse;
	m_motor_max_speed_rpm = motorMaxSpeedRpm;
	m_motor_max_speed = 1.0 * motorMaxSpeedRpm / 60 * 4 * encoderLines * 10;
	m_encoder_lines = encoderLines;
	if (m_pos_reduce_rate < 10000) critical_delta = 0.1f;
	else if (m_pos_reduce_rate < 150000)critical_delta = 1.1f;
	else if (m_pos_reduce_rate < 350000)critical_delta = 2.1f;
	else critical_delta = 3.1f;
	return;
}
*/
int CMileage::initializeParam(uint8_t* pbData, uint16_t len)
{
	printf("initialize Param\r\n");
	
	this->doInit();
	bool status;
	{
		rbk_protocol_Message_ChassisConfig pbMsg = rbk_protocol_Message_ChassisConfig_init_zero;
		pb_istream_t stream = pb_istream_from_buffer(pbData, len);
		status = pb_decode(&stream, rbk_protocol_Message_ChassisConfig_fields, &pbMsg);

		if (!status)
		{
			printf("Mileage Decoding failed: \n");
			return -1;
		}

		this->m_e = pbMsg.E / 1000;
		m_motor_max_speed_rpm = pbMsg.motorMaxSpeedRpm;
		m_encoder_lines = pbMsg.encoderLines;
		m_motor_max_speed = 1.0 * m_motor_max_speed_rpm / 60 * 4 * m_encoder_lines * 10;
		m_vel_reduce_scale = pbMsg.encoderLines * 4 * pbMsg.reductionRatio * 10;
		m_pos_reduce_rate = pbMsg.encoderLines * 4 * pbMsg.reductionRatio;
		int8_t tempInverse = pbMsg.Inverse[0] ? -1 : 1;
		m_inverse = tempInverse;

		if (m_pos_reduce_rate < 10000) critical_delta = 0.1f;
		else if (m_pos_reduce_rate < 150000) critical_delta = 1.1f;
		else if (m_pos_reduce_rate < 350000) critical_delta = 2.1f;
		else critical_delta = 3.1f;
		
		if (pbMsg.driveID_count < num_of_wheel())
		{
			printf("wheel config error!\n");
		}
		
		if (SINGLE_STEER_PREC != m_chassisType)
		{
			for (int i = 0; i < pbMsg.driveID_count; i++)
			{
				//ChassisDevice::Instance()->Protocol()->calcRxID(1);
				ChassisDevice::Instance()->Protocol()->setMotorRxId(i, \
					ChassisDevice::Instance()->Protocol()->calcRxID(pbMsg.driveID[i]));
				ChassisDevice::Instance()->Protocol()->setMotorTxId(i, \
					ChassisDevice::Instance()->Protocol()->calcTxID(pbMsg.driveID[i]));
				ChassisDevice::Instance()->Protocol()->setMotorInverse(i, pbMsg.Inverse[i]);
				int8_t temp = pbMsg.Inverse[i] ? -1 : 1;
				m_scale[i] = 2.0f * PI * pbMsg.R[i] / 1000.0f * temp;
				m_inv_one[i]= temp;
			//	Console::Instance()->printf("Config driver id %d\r\n", pbMsg.driveID[i]);
			}
			m_inverse = 1;
		}
		m_param_inited = true;
	}
	return 0;
}

void CMileage::getPositions(iWheelPos& p_wheelpos)
{
	if(2 == num_of_wheel())
	{
		p_wheelpos.at(0) = -1 * m_inverse * m_pos.at(0);
		p_wheelpos.at(1) = m_inverse * m_pos.at(1);
	}
	else if(3 == num_of_wheel())
	{
		p_wheelpos.at(0) = m_inverse * m_pos.at(0);
		p_wheelpos.at(1) = m_inverse * m_pos.at(1);
		p_wheelpos.at(2) = m_inverse * m_pos.at(2);
	}
	else if(4 == num_of_wheel())
	{
		p_wheelpos.at(0) = m_inverse * m_pos.at(0);
		p_wheelpos.at(1) = -1 * m_inverse * m_pos.at(1);
		p_wheelpos.at(2) = -1 * m_inverse * m_pos.at(2);
		p_wheelpos.at(3) = m_inverse * m_pos.at(3);
	}

}

void CMileage::setVelocities(const WheelVel& v_wheelvel)
{
	WheelVel temp_wheelvel;
	for(int i = 0; i < num_of_wheel(); i++)
	{
		if(v_wheelvel.at(i) < 0)
			temp_wheelvel.at(i) = v_wheelvel.at(i) - 0.5f;
		else if(v_wheelvel.at(i) > 0)
			temp_wheelvel.at(i) = v_wheelvel.at(i) + 0.5f;
		else
			temp_wheelvel.at(i) = 0;
	}

	if(2 == num_of_wheel())
	{
		m_vel.at(0) = temp_wheelvel.at(0) * (-1) * m_inverse;
		m_vel.at(1) = temp_wheelvel.at(1) * m_inverse;
	}
	else if(3 == num_of_wheel())
	{
		m_vel.at(0) = temp_wheelvel.at(0) * m_inverse;
		m_vel.at(1) = temp_wheelvel.at(1) * m_inverse;
		m_vel.at(2) = temp_wheelvel.at(2) * m_inverse;
	}
	else if(4 == num_of_wheel())
	{
		m_vel.at(0) = temp_wheelvel.at(0) * m_inverse;
		m_vel.at(1) = -1 * temp_wheelvel.at(1) * m_inverse;
		m_vel.at(2) = -1 * temp_wheelvel.at(2) * m_inverse;
		m_vel.at(3) = temp_wheelvel.at(3) * m_inverse;
	}
}

uint8_t CMileage::num_of_wheel()
{
	if (DOUBLE_DIFF == m_chassisType)
		return 2;
	else if (TRICYCLE == m_chassisType)
		return 3;
	else if (QUAD_MACNUM == m_chassisType)
		return 4;
	else if (SINGLE_STEER_PREC == m_chassisType)
		return 1;
	else if (SINGLE_STEER_AFTER == m_chassisType)
		return 1;
	else if (DOUBLE_STEER == m_chassisType)
		return 2;
	else if (QUAD_STEER == m_chassisType)
		return 4;
	else if (QUAD_DIFF == m_chassisType)
		return 4;
	else
		return 0;
}

int CMileage::report(void* arg)
{
	reportLock();
	static uint64_t prev_time = 1;

	uint32_t msgEnum = CReportData::EOdometer;
	memcpy(CReporter_base::transBuff_, &msgEnum, sizeof(uint32_t));

	_rbk_protocol_Message_Odometer pbMsg = rbk_protocol_Message_Odometer_init_zero;

	pb_ostream_t stream = pb_ostream_from_buffer(
		CReporter_base::transBuff_ + sizeof(uint32_t),
		CReporter_base::TRANS_BUFF_LEN - sizeof(uint32_t));

	pbMsg.encoder_count = num_of_wheel();
	for(int i = 0; i < pbMsg.encoder_count; i++)
	{
		pbMsg.encoder[i] = static_p.at(i) * m_inv_one[i];
		// Console::Instance()->printf("encoder: %d, inverse: %d, origin: %d\r\n", pbMsg.encoder[i], m_inv_one[i],static_p.at(i));
	}

	pbMsg.x = m_x;
	pbMsg.y = m_y;
	pbMsg.angle = m_angle;
	
	pbMsg.steer_angle = m_steerangle;
	struct timeval currentTime;
	gettimeofday(&currentTime, NULL);
	timestamp_nsec_ = (currentTime.tv_sec - currentTime.tv_sec) * 1000000000L + \
        (currentTime.tv_usec - currentTime.tv_usec) * 1000L;
	pbMsg.header.data_nsec = timestamp_nsec_ + SntpSynchr::Instance()->getNtpBaseOffset_ns();
	pbMsg.is_stop = m_stopped;
	pbMsg.spin_angle = 0;//LiftRotationDevice::Instance()->getCurrentRad();
	uint64_t deltatime = 10000000;//timestamp_nsec_ - prev_time;      	
	gettimeofday(&xTimeCur, NULL);    
	if(xTimeLast.tv_usec == 0 && xTimeLast.tv_sec==0);
	else
		deltatime = (xTimeCur.tv_sec - xTimeLast.tv_sec) * 1000000000l + \
        (xTimeCur.tv_usec - xTimeLast.tv_usec) * 1000L;
	if (deltatime > 0) 
	{
		if (SINGLE_STEER_PREC == m_chassisType) 
			pbMsg.vel_x = m_steer_vel;
		else 
			pbMsg.vel_x = m_delta_x / (deltatime / 1000000000.0f);
		pbMsg.vel_y = m_delta_y / (deltatime / 1000000000.0f);
		car_rotate = m_delta_angle / (deltatime / 1000000000.0f);
		pbMsg.vel_rotate = car_rotate;
	} else
	{
		printf("odometer deltaT = 0");
	}

	if (fabs(pbMsg.vel_x) > 3)
	{
		printf("vel_x is too large, vel_x = %f", pbMsg.vel_x);
	}
	if (fabs(pbMsg.vel_y) > 3)
	{
		printf("vel_y is too large, vel_y = %f", pbMsg.vel_y);
	}
	if (fabs(pbMsg.vel_rotate) > PI) 
	{
		printf("vel_rotate is too large, vel_rotate = %f, m_delta_angle = %f, deltatime = %llu", \
			pbMsg.vel_rotate, m_delta_angle, deltatime);
	}

//	RobotPosEKF::Instance()->setNewOdomData(pbMsg);

	bool status;
	status = pb_encode(&stream, rbk_protocol_Message_Odometer_fields, &pbMsg);
	uint16_t pbMsgLen = stream.bytes_written;
	if (!status)
	{
		printf("Mileage Encoding failed: %s\r\n", PB_GET_ERROR(&stream));
	}

	uploadRbk(pbMsgLen + sizeof(uint32_t));
	reportUnlock();
	prev_time = timestamp_nsec_;
	return 0;

}
/*
int CMileage::odoCommandHandler(uint8_t* argv, uint16_t argc)
{
	if(sizeof(uint32_t) != argc)
		return -1;

	uint32_t cmd = *(uint32_t*)argv;
	switch (cmd)
	{
		case 0:
		{
			reset();
			Console::Instance()->printf("X: %.4fm, Y: %.4fm, angle: %.4frad, sa = %.4frad, stop: %d\r\n",
				m_x, m_y, m_angle, m_steerangle, m_stopped);
			break;
		}
		case 1:
		{
			Console::Instance()->printf("X: %.4fm, Y: %.4fm, angle: %.4frad, sa = %.4frad, stop: %d\r\n",
				m_x, m_y, m_angle, m_steerangle, m_stopped);
			break;
		}
		default:
			break;
	}
	return 0;
}

int CMileage::updateGlobalOdo(WheelPos deltaOdoAbsAngle)
{

	bool is_wheel_rotate = false;

	//TODO: confirm which value is real stop!
	if(fabs(deltaOdoAbsAngle.carLikeDriveWheel()) > 0
		|| fabs(deltaOdoAbsAngle.carLikeSteerAngle() - m_steerangle) > 0.0026f)
	{
		is_wheel_rotate = true;
		m_stopped = 0;
		stopCycle = 0;
	}

	if(false == is_wheel_rotate)
	{
		stopCycle = (stopCycle >= STOP_CYCLE_LIMIT) ? STOP_CYCLE_LIMIT : (stopCycle + 1);
		if (stopCycle >= STOP_CYCLE_LIMIT)
			m_stopped = 1;
	}

	// 2.do forward kinematics transformation
	PlanarPos planarPos;

	if(SINGLE_STEER_AFTER == m_chassisType)
	{
		forwardKinematicsTransCarLikeFixedWheelOdo_SI(deltaOdoAbsAngle, planarPos);
	}else if(SINGLE_STEER_PREC == m_chassisType)
	{
		forwardKinematicsTransCarLikeSteerWheelOdo_SI(deltaOdoAbsAngle, planarPos);
	}else
	{
		Console::Instance()->printf("Unsupport chassis type: %d\r\n", m_chassisType);
	}


	float tmp_dx = planarPos.x * cos(m_angle) - planarPos.y * sin(m_angle);
	float tmp_dy = planarPos.x * sin(m_angle) + planarPos.y * cos(m_angle);
	float tmp_dtheta = planarPos.theta;
	
	m_delta_x = planarPos.x;
	m_delta_y = planarPos.y;
	m_delta_angle = tmp_dtheta;
	
#if 0
	static int cnt = 0;
	if(++cnt > 100)
	{
		cnt = 0;
		Console::Instance()->printf("delta: x = %f m, y = %f m, theta = %f rad, stopped: %d\r\n",
			tmp_dx, tmp_dy , tmp_dtheta, m_stopped);
//		Console::Instance()->printf("phy delta x = %f m, actual angle = %f rad\r\n",
//			deltaOdoAbsAngle.carLikeDriveWheel(), deltaOdoAbsAngle.carLikeSteerAngle());
	}
#endif
	m_x += tmp_dx;
	m_y += tmp_dy;
	m_angle += tmp_dtheta;
	m_steerangle = deltaOdoAbsAngle.carLikeSteerAngle();

	while (m_angle <= -PI) {
		m_angle += 2 * PI;
	}
	
	while (m_angle > PI) {
		m_angle -= 2 * PI;
	}

	return 0;
}

void CMileage::setSteerVel(int16_t rawVel)
{
	//m_r = 0.115f;
	//m_vel_reduce_scale = 27.84;
	m_steer_vel = (float)rawVel * (m_scale[0] / 60 / m_reductionRatio);
}

float CMileage::driveWheelRawSpeedToPhyOdo(void* rawSpeed)
{
	uint64_t PDO_PERIOD = timestamp_nsec_ - m_timestamp_ns;
	m_timestamp_ns = timestamp_nsec_;
	int16_t rawVal = *(int16_t*)rawSpeed;
//	float phySpeed = (float)rawVal * 0.1125f / 184.0f;
	float phySpeed = (float)rawVal * steer_wheel_r() / 60 / steer_wheel_ve_r();
	float phyOdo = PDO_PERIOD * phySpeed / 1000000000;
	return phyOdo;
}
*/
//end of file
