#include "imu_module.hpp"

#include <stdio.h>
#include <iostream>
#include <unistd.h>

#include <sys/time.h>
#include <sys/select.h>
#include <time.h>

ImuModule imu;

void InitMPU9250()
{

  // Call imu.begin() to verify communication with and
  // initialize the MPU-9250 to it's default values.
  // Most functions return an error code - INV_SUCCESS (0)
  // indicates the IMU was present and successfully set up
  if (imu.MPU9250_DMP::begin() != INV_SUCCESS)
  {
    while (1)
    {
      std::cout<<"Unable to communicate with MPU-9250"<<std::endl;
	  std::cout<<"Check connections, and try again."<<std::endl;
	  sleep(5);
    }
  }

  // Use setSensors to turn on or off MPU-9250 sensors.
  // Any of the following defines can be combined:
  // INV_XYZ_GYRO, INV_XYZ_ACCEL, INV_XYZ_COMPASS,
  // INV_X_GYRO, INV_Y_GYRO, or INV_Z_GYRO
  // Enable all sensors:
  imu.setSensors(INV_XYZ_GYRO | INV_XYZ_ACCEL | INV_XYZ_COMPASS);

  // Use setGyroFSR() and setAccelFSR() to configure the
  // gyroscope and accelerometer full scale ranges.
  // Gyro options are +/- 250, 500, 1000, or 2000 dps
  imu.setGyroFSR(2000); // Set gyro to 2000 dps
  // Accel options are +/- 2, 4, 8, or 16 g
  imu.setAccelFSR(2); // Set accel to +/-2g
  // Note: the MPU-9250's magnetometer FSR is set at
  // +/- 4912 uT (micro-tesla's)

  // setLPF() can be used to set the digital low-pass filter
  // of the accelerometer and gyroscope.
  // Can be any of the following: 188, 98, 42, 20, 10, 5
  // (values are in Hz).
  imu.setLPF(5); // Set LPF corner frequency to 5Hz

  // The sample rate of the accel/gyro can be set using
  // setSampleRate. Acceptable values range from 4Hz to 1kHz
  imu.setSampleRate(50); // Set sample rate to 10Hz

  // Likewise, the compass (magnetometer) sample rate can be
  // set using the setCompassSampleRate() function.
  // This value can range between: 1-100Hz
  imu.setCompassSampleRate(50); // Set mag rate to 10Hz
}

void printIMUData(void);
void setTimer(int seconds, int mseconds);

int main(){
		// dataReady() checks to see if new accel/gyro data
	  // is available. It will return a boolean true or false
	  // (New magnetometer data cannot be checked, as the library
	  //  runs that sensor in single-conversion mode.)
    InitMPU9250();
    //imu.CollectDataAndCali();

    long i;
    imu.Madgwick::begin(50);
    for(i = 0 ; i < 10000; i++){
            setTimer(0, 200);
    }
	return 0;
}


void printIMUData(void)
{
  // After calling update() the ax, ay, az, gx, gy, gz, mx,
  // my, mz, time, and/or temerature class variables are all
  // updated. Access them by placing the object. in front:

  // Use the calcAccel, calcGyro, and calcMag functions to
  // convert the raw sensor readings (signed 16-bit values)
  // to their respective units.
  imu.CalMoveData();
  std::cout<<"Accel: "<<" X :"<<imu.move_data.accelX<<" Y :"<<imu.move_data.accelY<<" Z :"<<imu.move_data.accelZ<<std::endl;
  std::cout<<"Gyro: "<<" X :"<<imu.move_data.gyroX<<" Y :"<<imu.move_data.gyroY<<" Z :"<<imu.move_data.gyroZ<<std::endl;
  std::cout<<"Mag: " <<" X :"<<imu.move_data.magX<<" Y :"<<imu.move_data.magY<<" Z :"<<imu.move_data.magZ<<std::endl;
  std::cout<<"Time: "<<imu.time<<std::endl;
}

void setTimer(int seconds, int mseconds)
{
        struct timeval temp;

        temp.tv_sec = seconds;
        temp.tv_usec = mseconds;

        select(0, NULL, NULL, NULL, &temp);

        while(!imu.dataReady()){
            ;
        }
        imu.MPU9250_DMP::update(UPDATE_ACCEL | UPDATE_GYRO | UPDATE_COMPASS);
        imu.CalMoveData();
        imu.EstimationPose();
        std::cout<<"Roll: "<<imu.getRoll()<<std::endl;
        std::cout<<"Pitch: "<<imu.getPitch()<<std::endl;
        std::cout<<"Yaw: "<<imu.getYaw() <<std::endl;
        return ;
}

