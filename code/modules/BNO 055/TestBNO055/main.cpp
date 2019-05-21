#include <iostream>
#include <fstream>
//#include <Wire.h>
#include "Adafruit_Sensor.h"
#include "Adafruit_BNO055.h"
#include "BNO055_Cali.h"
#include "imumaths.h"
#include "LinuxClk.h"
#define delay(a) Linux_delay_ms(a)
/* This driver reads raw data from the BNO055

   Connections
   ===========
   Connect SCL to analog 5
   Connect SDA to analog 4
   Connect VDD to 3.3V DC
   Connect GROUND to common ground

   History
   =======
   2015/MAR/03  - First release (KTOWN)
*/

/* Set the delay between fresh samples */
#define BNO055_SAMPLERATE_DELAY_MS (100)
#define CaliMagneDataNUM (3000)


void CaliMagneDataCollectAndCali();
BNO055_Cali bno = BNO055_Cali();

/**************************************************************************/
/*
    Arduino setup function (automatically called at startup)
*/
/**************************************************************************/
void setup(void)
{
  //Serial.begin(9600);
  //Serial.println("Orientation Sensor Raw Data Test"); Serial.println("");
  std::cout<<"Orientation Sensor Raw Data Test"<<std::endl;

  /* Initialise the sensor */

  if(!bno.begin(BNO055_Cali::OPERATION_MODE_NDOF))
  {
    /* There was a problem detecting the BNO055 ... check your connections */
    //Serial.print("Ooops, no BNO055 detected ... Check your wiring or I2C ADDR!");
    std::cout<<"Ooops, no BNO055 detected ... Check your wiring or I2C ADDR!"<<std::endl;

    while(1);
  }

  delay(1000);
  #ifdef Debug
  if(bno.bMagnBiasGeted){
    std::cout<<"Rewrite magn bias"<<std::endl;
    bno.setMode(BNO055_Cali::OPERATION_MODE_CONFIG);
    bno.setSensorOffsets(bno.OffsetToWriteToRegister);
    bno.setMode(BNO055_Cali::OPERATION_MODE_NDOF);
    delay(1000);
  }
  #endif // Debug
  /* Display the current temperature */
  int8_t temp = bno.getTemp();
  std::cout<<"Current Temperature: "<<temp<<" C"<<std::endl;
//  Serial.print("Current Temperature: ");
//  Serial.print(temp);
//  Serial.println(" C");
//  Serial.println("");

  bno.setExtCrystalUse(true);
    std::cout<<"Calibration status values: 0=uncalibrated, 3=fully calibrated"<<std::endl;
  //Serial.println("Calibration status values: 0=uncalibrated, 3=fully calibrated");
}
#ifdef Dehug
std::ofstream Magnedata,Oriendata;
#endif // Dehug
void loop(void)
{
  // Possible vector values can be:
  // - VECTOR_ACCELEROMETER - m/s^2
  // - VECTOR_MAGNETOMETER  - uT
  // - VECTOR_GYROSCOPE     - rad/s
  // - VECTOR_EULER         - degrees
  // - VECTOR_LINEARACCEL   - m/s^2
  // - VECTOR_GRAVITY       - m/s^2
  sensors_event_t event;
  bno.getEvent(& event);
  std::cout<<"Orientation :"<<(float)event.orientation.x<<\
                       " "<<(float)event.orientation.y<<\
                       " "<<(float)event.orientation.z<<std::endl;

//  imu::Vector<3> euler = bno.getVector(Adafruit_BNO055::VECTOR_EULER);

  /* Display the floating point data */
//  Serial.print("X: ");
//  Serial.print(euler.x());
//  Serial.print(" Y: ");
//  Serial.print(euler.y());
//  Serial.print(" Z: ");
//  Serial.print(euler.z());
//  Serial.print("\t\t");

  /*
  // Quaternion data
  imu::Quaternion quat = bno.getQuat();
  Serial.print("qW: ");
  Serial.print(quat.w(), 4);
  Serial.print(" qX: ");
  Serial.print(quat.x(), 4);
  Serial.print(" qY: ");
  Serial.print(quat.y(), 4);
  Serial.print(" qZ: ");
  Serial.print(quat.z(), 4);
  Serial.print("\t\t");
  */

  /* Display calibration status for each sensor. */
  uint8_t system, gyro, accel, mag = 0;
  bno.getCalibration(&system, &gyro, &accel, &mag);
  std::cout<<"System "<<(int)system<<"gyro "<<(int)gyro << \
            "accel "<<(int)accel<<"mag "<<(int)mag<<std::endl;
  #ifdef Debug
    if(system>=1){
        imu::Vector<3> magn = bno.getVector(Adafruit_BNO055::VECTOR_MAGNETOMETER);
        Magnedata<<magn.x()<<" "<<magn.y()<<" "<<magn.z()<<"\n";
        Oriendata<<(float)event.orientation.x<<\
                       " "<<(float)event.orientation.y<<\
                       " "<<(float)event.orientation.z<<"\n";
    }
    #endif // Debug


  delay(BNO055_SAMPLERATE_DELAY_MS);
}


/**************************************************************************/
/*
    Arduino loop function, called once 'setup' is complete (your own code
    should go here)
*/
/**************************************************************************/
#ifdef Debug
bool CaliProfileGeted=false;
std::ofstream CaliProfile;
adafruit_bno055_offsets_t CaliProfileData;
#endif // Debug
int main(){
    #ifdef Debug
    bno.CaliMagneDataCollectAndCali();

    Magnedata.open("./Magnedata.txt",std::ios::trunc|std::ios::binary |std::ios::in|std::ios::out);
    Oriendata.open("./Oriendata.txt",std::ios::trunc|std::ios::binary |std::ios::in|std::ios::out);
    #endif // Debug
    setup();
    #ifdef Debug
    CaliProfile.open("./CaliProfile.txt",std::ios::trunc|std::ios::binary |std::ios::in|std::ios::out);
    #endif // Debug
    while(1){
        loop();
    #ifdef Debug
        if(bno.isFullyCalibrated()&& !CaliProfileGeted){
           bno.getSensorOffsets(CaliProfileData);
        CaliProfile<<CaliProfileData.accel_offset_x<<" "<<CaliProfileData.accel_offset_y<<" "<<CaliProfileData.accel_offset_z<<"\n"\
                    << CaliProfileData.accel_radius<<"\n"\
                    << CaliProfileData.gyro_offset_x<<" "<<CaliProfileData.gyro_offset_y<<" "<<CaliProfileData.gyro_offset_z<<"\n" \
                    << CaliProfileData.mag_offset_x<<" "<<CaliProfileData.mag_offset_y<<" "<<CaliProfileData.mag_offset_z<<"\n"\
                    << CaliProfileData.mag_radius<<"\n";
        CaliProfileGeted=true;
        CaliProfile.close();
        }
    #endif // Debug
    }

return 0;
}


