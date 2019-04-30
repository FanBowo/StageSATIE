#include "imu_module.hpp"

#include <stdio.h>
#include <iostream>
#include <unistd.h>
ImuModule imu;

void InitMPU9250()
{

  // Call imu.begin() to verify communication with and
  // initialize the MPU-9250 to it's default values.
  // Most functions return an error code - INV_SUCCESS (0)
  // indicates the IMU was present and successfully set up
  if (imu.begin() != INV_SUCCESS)
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

int main(){
		// dataReady() checks to see if new accel/gyro data
	  // is available. It will return a boolean true or false
	  // (New magnetometer data cannot be checked, as the library
	  //  runs that sensor in single-conversion mode.)
    InitMPU9250();
    imu.CollectDataAndCali();
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
  float accelX = imu.calcAccel(imu.ax);
  float accelY = imu.calcAccel(imu.ay);
  float accelZ = imu.calcAccel(imu.az);
  float gyroX = imu.calcGyro(imu.gx);
  float gyroY = imu.calcGyro(imu.gy);
  float gyroZ = imu.calcGyro(imu.gz);
  float magX = imu.calcMag(imu.mx);
  float magY = imu.calcMag(imu.my);
  float magZ = imu.calcMag(imu.mz);
  std::cout<<"Accel: "<<" X :"<<accelX<<" Y :"<<accelY<<" Z :"<<accelZ<<std::endl;
  std::cout<<"Gyro: "<<" X :"<<gyroX<<" Y :"<<gyroY<<" Z :"<<gyroZ<<std::endl;
  std::cout<<"Mag: " <<" X :"<<magX<<" Y :"<<magY<<" Z :"<<magZ<<std::endl;
  std::cout<<"Time: "<<imu.time<<std::endl;
}

