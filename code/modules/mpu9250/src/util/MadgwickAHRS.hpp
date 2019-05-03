//=====================================================================================================
// MadgwickAHRS.h
//=====================================================================================================
//
// Implementation of Madgwick's IMU and AHRS algorithms.
// See: http://www.x-io.co.uk/node/8#open_source_ahrs_and_imu_algorithms
//
// Date			Author          Notes
// 29/09/2011	SOH Madgwick    Initial release
// 02/10/2011	SOH Madgwick	Optimised for reduced CPU load
//
//=====================================================================================================
#ifndef MadgwickAHRS_h
#define MadgwickAHRS_h

//----------------------------------------------------------------------------------------------------
// Variable declaration
class Madgwick_cpp{
	private :
	 static float invSqrt(float x);
	 float beta, q0, q1, q2, q3,sampleFreq;	
	 
	 float roll, pitch, yaw;
	 
	 char anglesComputed;
	 void computeAngles();
	 
	 public:
	 
	Madgwick_cpp();
	void begin (float sampleFrequency);
    void update(float gx, float gy, float gz, float ax, float ay, float az, float mx, float my, float mz);
    void updateIMU(float gx, float gy, float gz, float ax, float ay, float az);
	
	float getRoll(){
		if(!anglesComputed)computeAngles();
			return roll * 57.29578f;
	}
	
	float getPitch(){
		if(!anglesComputed)computeAngles();
			return pitch * 57.29578f;
	}
	
	float getYaw(){
		if(!anglesComputed)computeAngles();
			return yaw * 57.29578f;
	}
	
};

//---------------------------------------------------------------------------------------------------
// Function declarations


#endif
//=====================================================================================================
// End of file
//=====================================================================================================
