#ifndef _FW_MILEAGE_H_
#define _FW_MILEAGE_H_

#include "Singleton.h"
#include <stdint.h>
#include <string.h>

#define helf_PI	1.570796f


enum chassisTypeEnum
{
	UNDEFINED = 0,
	DOUBLE_DIFF = 1,
	TRICYCLE = 2,
	QUAD_MACNUM = 3,
	SINGLE_STEER_PREC = 4,
	SINGLE_STEER_AFTER = 5,
	DOUBLE_STEER = 6,
	QUAD_STEER = 7,
	QUAD_DIFF = 8,
};

struct PlanarVel {
	float vx;
	float vy;
	float w;
	float steer_angle;
};

struct PlanarPos {
	float x;
	float y;
	float theta;
	float steer_angle;
};
class CConfigTask;
class CMileage
{
	friend class CConfigTask;
public:
	enum{MAX_WHEEL_NUM = 4};
	
	template <typename T>
	class CWheel_PhyParam
	{
		public:
		CWheel_PhyParam()
		{
			reset();
		}
		T& at(uint16_t idx) 
		{
			//seer_assert(idx < MAX_WHEEL_NUM);
			return m_w[idx];
		}
		
		const T& at(uint16_t idx) const
		{
			//seer_assert(idx < MAX_WHEEL_NUM);
			return m_w[idx];
		}
		
		T& carLikeSteerAngle()
		{
			return m_w[0];
		}
		
		const T& carLikeSteerAngle() const
		{
			return m_w[0];
		}
		
		T& carLikeDriveWheel()
		{
			return m_w[1];
		}
		
		const T& carLikeDriveWheel() const
		{
			return m_w[1];
		}
	
		
		void reset() {memset(m_w, 0, MAX_WHEEL_NUM*sizeof(T));}
		private:
			T m_w[MAX_WHEEL_NUM];
			
	};
	
	typedef CWheel_PhyParam<float> WheelVel;
	typedef CWheel_PhyParam<float> WheelPos;
	typedef CWheel_PhyParam<int32_t> iWheelVel;
	typedef CWheel_PhyParam<int32_t> iWheelPos;
	 
	CMileage():	m_x(0.0),m_y(0.0),m_angle(0.0),firstIn(true),
				m_chassisType(DOUBLE_DIFF),m_e(500),m_r(100),
				m_vel_reduce_scale(2000),m_pos_reduce_rate(200),
				m_motor_max_speed_rpm(0),m_motor_max_speed(0),m_inverse(1),m_stopped(true)
				{
					for(int i = 0; i < CMileage::MAX_WHEEL_NUM; i++)
					{
						m_scale[i] = 20000;
					}
					
					m_delta_x = 0;
					m_delta_y = 0;
					m_delta_angle = 0;
					
					m_max_steer_angle = 90.0f;
					m_min_steer_angle = -90.0f;
					m_reductionRatio = 15;
					
					for (int i = 0; i < 4; i++)
						m_inv_one[i] = 1;
				}
	virtual ~CMileage(){}	
		
 	int doInit();
	//int doCalc(const iWheelPos& wheelpos);
	//int updateGlobalOdo(WheelPos delta_pos);
		
	//void initialzeParam(float e, float r, int32_t encoderLines, float reductionRatio, int32_t motorMaxSpeedRpm, int32_t inverse);
	//void initialzeParam4cp(const float& e, const float (&r)[4], const int32_t& encoderLines, const float& reductionRatio, const int32_t& motorMaxSpeedRpm, const int32_t& inverse);
	int initializeParam(uint8_t* msg, uint16_t len);

	void reset();
	//void resetEncoderLog();
	//void getData(float& x, float& y, float& angle);
	bool isStopped() const {return m_stopped;};
	int32_t encoderLines(){return m_encoder_lines;}
	int32_t getMotorMaxSpeedRpm(){return m_motor_max_speed_rpm;}
	float globalSteerAngle() { return m_steerangle; } 

	/// Forward Kinematics
	//void forwardKinematicsTrans(const WheelPos& wheelPos, PlanarPos& planarPos);

	/// Inverse Kinematics
	//void inverseKinematicsTrans(const PlanarVel& planarVel, WheelVel& wheelVel);

	uint8_t num_of_wheel();
	chassisTypeEnum chassisType() {return m_chassisType;}
	
	//int odoCommandHandler(uint8_t* argv, uint16_t argc);
	
	//void forwardKinematicsTransCarLikeSteerWheelOdo_SI(const WheelPos& wheelPos, PlanarPos& planarPos);

	//void forwardKinematicsTransCarLikeFixedWheelOdo_SI(const WheelPos& wheelPos, PlanarPos& planarPos);

	//void inverseKinematicsTransCarLikeSteerWheelDrive_SI(const PlanarVel& planarVel, WheelVel& wheelVel);

	//void inverseKinematicsTransCarLikeFixedWheelDrive_SI(const PlanarVel& planarVel, WheelVel& wheelVel);
	
	//float driveWheelRawSpeedToPhyOdo(void* rawSpeed);
	
	void setChassisType(chassisTypeEnum type) {m_chassisType = type;}

	uint64_t timestamp_ms;
	uint64_t m_timestamp_ns = 0;
	
	//void setSteerVel(int16_t rawVel);
	
public:
	//void getPositions(iWheelPos& p_wheelpos);
	void setPos(uint8_t idx, int32_t pos)
	{m_pos.at(idx) = pos;}
	
	float steer_wheel_r(void)
	{
		return m_scale[0];
	}
	
	float steer_wheel_ve_r(void)
	{
		return m_reductionRatio;
	}

	float get_fork_car_rotate(void)
	{
		return car_rotate;
	}

	//void setVelocities(const WheelVel& v_wheelvel);
	int32_t getVel(uint8_t idx){return m_vel.at(idx);}
	//virtual int report(void* arg = NULL);

private:
/*
	/// Forward Kinematics - Diff
	void forwardKinematicsTransDiff(const WheelPos& wheelPos, PlanarPos& planarPos);

	/// Inverse Kinematics - Diff
	void inverseKinematicsTransDiff(const PlanarVel& planarVel, WheelVel& wheelVel);

	//Tricycle
	void forwardKinematicsTransTricycle(const WheelPos& wheelPos, PlanarPos& planarPos);

	void inverseKinematicsTransTricycle(const PlanarVel& planarVel, WheelVel& wheelVel);

	void forwardKinematicsTransMecanum(const WheelPos& wheelPos, PlanarPos& planarPos);

	void inverseKinematicsTransMecanum(const PlanarVel& planarVel, WheelVel& wheelVel);
*/	
	float m_x;
	float m_y;
	float m_angle;
	float m_steerangle;
	float m_steer_vel;

	iWheelPos static_p;
	bool  firstIn;

private:	//params
	//chassisType
	chassisTypeEnum m_chassisType; 
	
	float steerAngleVal_RawToPhy(int32_t rawVal) { return (m_steer_angle_phy_dived_raw * rawVal + m_min_steer_angle_raw); }
	
	int32_t steerAngleVal_PhyToRaw(float phyVal) { return ((phyVal - m_min_steer_angle_raw) / m_steer_angle_phy_dived_raw); }

	/// Distance from left to right
	float m_e;

	/// Radius of the single wheel
	float m_r;

	/// Arc length
	float m_scale[4];

	// velocity reduce scale
	float m_vel_reduce_scale;

	// position reduce rate
	float m_pos_reduce_rate;
	
	int32_t m_motor_max_speed_rpm;
	float m_motor_max_speed;
	int32_t m_encoder_lines;
	
	float m_reductionRatio;
	
	int32_t m_min_steer_angle_raw;
	
	float m_max_steer_angle;
	float m_min_steer_angle;

	float m_steer_angle_phy_dived_raw;

private:
	// for position & velocity transfer
	iWheelVel m_vel;
	iWheelPos m_pos;
	int32_t m_inverse;
	int32_t m_inv_one[4];
	bool m_stopped;
	float m_delta_x;
	float m_delta_y;
	float m_delta_angle;
	float car_rotate;
};
typedef NormalSingleton<CMileage> Mileage;
#endif
