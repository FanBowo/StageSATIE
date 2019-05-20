#include "BNO055_Cali.h"
#include <iostream>
#include "LinuxClk.h"
#include <fstream>
#define delay(a) Linux_delay_ms(a)

#define CaliMagneSampleDelayMs (110)

BNO055_Cali::BNO055_Cali(){
    DataCollectedCount=0;
}
//We collecte data in interrupt and do calibration directely with

//void BNO055_Cali::CollectMagneDataAndCali(){
//
//    MagnAcceCalibration();
//}
void BNO055_Cali::CaliMagneDataCollectAndCali(){
  std::cout<<"Magne Sensor Calibration"<<std::endl;

  /* Initialise the sensor */
  if(!BNO055_Cali::begin(BNO055_Cali::OPERATION_MODE_MAGONLY))
  {
    /* There was a problem detecting the BNO055 ... check your connections */
    //Serial.print("Ooops, no BNO055 detected ... Check your wiring or I2C ADDR!");
    std::cout<<"Ooops, no BNO055 detected ... Check your wiring or I2C ADDR!"<<std::endl;

    while(1);
  }

  delay(1000);
  std::ofstream MagneCaliData;

  MagneCaliData.open("./MagneCaliData.txt",std::ios::trunc|std::ios::binary |std::ios::in|std::ios::out);

    XaxisData.clear();
    YaxisData.clear();
    ZaxisData.clear();
    for (int i =0;i<NUM_DATA_NEEDED;i++){
        imu::Vector<3> magn = getVector(Adafruit_BNO055::VECTOR_MAGNETOMETER);
        XaxisData.push_back(magn.x());
        YaxisData.push_back(magn.y());
        ZaxisData.push_back(magn.z());
        std::cout<<XaxisData.back()<<"   "<<YaxisData.back()<<"   "<<ZaxisData.back()<<"   "<<std::endl;
        MagneCaliData<<XaxisData.back()<<" "<<YaxisData.back()<<" "<<ZaxisData.back()<<"\n";
        delay(CaliMagneSampleDelayMs);
    }
    MagneCaliData.close();
    //std::cout << _DataCaliCollected << "/" << NUM_DATA_NEEDED << "collected"<<std::endl;
    //std::cout<<"XaxisData Size"<<XaxisData.size()<<std::endl;
    std::cout << "Data collected finished, start of calcule bias"<<std::endl;
    MagnAcceCalibration();
    MagneCentreAfterCali.XaxisData=CentreAfterCaliData(0,0);
    MagneCentreAfterCali.YaxisData=CentreAfterCaliData(0,1);
    MagneCentreAfterCali.ZaxisData=CentreAfterCaliData(0,2);
    std::cout << "Magnetic bias:"<<std::endl;
    std::cout<<"X bias"<<MagneCentreAfterCali.XaxisData<<std::endl;
    std::cout<<"Y bias"<<MagneCentreAfterCali.YaxisData<<std::endl;
    std::cout<<"Z bias"<<MagneCentreAfterCali.ZaxisData<<std::endl;

    adafruit_bno055_offsets_t OffsetToWriteToRegister;
    OffsetToWriteToRegister.accel_offset_x=0;
    OffsetToWriteToRegister.accel_offset_y=0;
    OffsetToWriteToRegister.accel_offset_z=0;
    OffsetToWriteToRegister.accel_radius=0;
    OffsetToWriteToRegister.gyro_offset_x=0;
    OffsetToWriteToRegister.gyro_offset_y=0;
    OffsetToWriteToRegister.gyro_offset_z=0;


}
