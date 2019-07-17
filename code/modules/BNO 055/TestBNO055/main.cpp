
//#include <Wire.h>
//#include "Adafruit_Sensor.h"
//#include "Adafruit_BNO055.h"
#include "BNO055_Cali.h"

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



//#define ManuCaliMagn
#ifdef ManuCaliMagn
    #define UsingProfileConfig
#endif // ManuCaliMagn
#define EXCUTE
//#define Debug
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
#ifdef ManuCaliMagn
std::ofstream Magnedata,Oriendata;
#endif // ManuCaliMagn
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
                       " "<<(float)event.orientation.z\
                       <<" "<<(float)bno.bInitWithCaliProfileCompleted<<std::endl;

  bno.getEvent(& event,Adafruit_BNO055::VECTOR_GYROSCOPE);
  std::cout<<"omega :"<<(float)event.gyro.x<<\
                       " "<<(float)event.gyro.y<<\
                       " "<<(float)event.gyro.z<<std::endl;

  bno.getEvent(& event,Adafruit_BNO055::VECTOR_LINEARACCEL);
  std::cout<<"acc :"<<(float)event.acceleration.x<<\
                       " "<<(float)event.acceleration.y<<\
                       " "<<(float)event.acceleration.z<<std::endl;

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
  #ifdef ManuCaliMagn
    if(system>=1){
        imu::Vector<3> magn = bno.getVector(Adafruit_BNO055::VECTOR_MAGNETOMETER);
        Magnedata<<magn.x()<<" "<<magn.y()<<" "<<magn.z()<<"\n";
        Oriendata<<(float)event.orientation.x<<\
                       " "<<(float)event.orientation.y<<\
                       " "<<(float)event.orientation.z<<"\n";
    }
    #endif // ManuCaliMagn
#ifdef UsingProfileConfig
  if((system>=2&&gyro>=3&&mag>=3&& !(bno.bInitWithCaliProfileCompleted))){
    bno.InitWithCaliProfile();
    bno.LastNorthPoleCount=0;
  }
  if(system<=0&&(bno.bInitWithCaliProfileCompleted)){
    bno.LastNorthPoleCount++;
    if(bno.LastNorthPoleCount>200){
        bno.bInitWithCaliProfileCompleted=false;//Need re-calibration
    }
  }
#endif // UsingProfileConfig

  delay(BNO055_SAMPLERATE_DELAY_MS);
}

void ShowMagnBias();

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
    setup();
    #ifdef UsingProfileConfig
        #ifdef ManuCaliMagn
        bno.CaliMagneDataCollectAndCali();

        Magnedata.open("./Magnedata.txt",std::ios::trunc|std::ios::binary |std::ios::in|std::ios::out);
        Oriendata.open("./Oriendata.txt",std::ios::trunc|std::ios::binary |std::ios::in|std::ios::out);
        #endif // ManuCaliMagn
    #endif // UsingProfileConfig

    #ifdef Debug
    CaliProfile.open("./CaliProfile.txt",std::ios::trunc|std::ios::binary |std::ios::in|std::ios::out);
    #endif // Debug
    #ifdef UsingProfileConfig
    bno.InitWithCaliProfile();
    #endif // UsingProfileConfig
    while(1){
    #ifdef EXCUTE
        loop();
        /*static int tempcount=0;
        tempcount++;
        if(tempcount>100){
            ShowMagnBias();
            tempcount=0;

        }*/
    #endif // EXCUTE

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

void ShowMagnBias(){
        adafruit_bno055_offsets_t OffsetVal;
        bno.getSensorOffsets(OffsetVal);
        std::cout<<OffsetVal.accel_offset_x<<" "<<OffsetVal.accel_offset_y<<" "<<OffsetVal.accel_offset_z<<"\n"\
                    << OffsetVal.accel_radius<<"\n"\
                    << OffsetVal.gyro_offset_x<<" "<<OffsetVal.gyro_offset_y<<" "<<OffsetVal.gyro_offset_z<<"\n" \
                    << OffsetVal.mag_offset_x<<" "<<OffsetVal.mag_offset_y<<" "<<OffsetVal.mag_offset_z<<"\n"\
                    << OffsetVal.mag_radius<<std::endl;
}
