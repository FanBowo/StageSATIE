#include "BNO055_Cali.h"
#include <iostream>
#include "LinuxClk.h"
#include <fstream>
#define delay(a) Linux_delay_ms(a)

#define CaliMagneSampleDelayMs (110)

BNO055_Cali::BNO055_Cali(){
    DataCollectedCount=0;
    LastNorthPoleCount=0;
    bInitWithCaliProfileCompleted=false;
    bMagnBiasGeted=false;
    //bManuCaliMagnCompleted=false;
}
//We collecte data in interrupt and do calibration directely with

//void BNO055_Cali::CollectMagneDataAndCali(){
//
//    MagnAcceCalibration();
//}
void BNO055_Cali::CaliMagneDataCollectAndCali(){
  bMagnBiasGeted=false;

  std::cout<<"Magne Sensor Calibration"<<std::endl;

//  /* Initialise the sensor */
//  if(!BNO055_Cali::begin(BNO055_Cali::OPERATION_MODE_MAGONLY))
//  {
//    /* There was a problem detecting the BNO055 ... check your connections */
//    //Serial.print("Ooops, no BNO055 detected ... Check your wiring or I2C ADDR!");
//    std::cout<<"Ooops, no BNO055 detected ... Check your wiring or I2C ADDR!"<<std::endl;
//
//    while(1);
//  }
    setMode(BNO055_Cali::OPERATION_MODE_CONFIG);
    delay(20);
    setMode(BNO055_Cali::OPERATION_MODE_MAGONLY);
    delay(20);

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
    MagnRadiuAvg=RadiuAvg;
    std::cout << "Magnetic bias:"<<std::endl;
    std::cout<<"X bias"<<MagneCentreAfterCali.XaxisData<<std::endl;
    std::cout<<"Y bias"<<MagneCentreAfterCali.YaxisData<<std::endl;
    std::cout<<"Z bias"<<MagneCentreAfterCali.ZaxisData<<std::endl;
    WriteMagneBias();
}

void BNO055_Cali::WriteMagneBias(){

    std::ofstream  ManuMagneCaliData;

    ManuMagneCaliData.open("./ManuMagneCaliData.txt",std::ios::trunc|std::ios::binary |std::ios::in|std::ios::out);

    OffsetToWriteToRegister.accel_offset_x=0;
    OffsetToWriteToRegister.accel_offset_y=0;
    OffsetToWriteToRegister.accel_offset_z=0;
    OffsetToWriteToRegister.accel_radius=0;
    OffsetToWriteToRegister.gyro_offset_x=0;
    OffsetToWriteToRegister.gyro_offset_y=0;
    OffsetToWriteToRegister.gyro_offset_z=0;
    OffsetToWriteToRegister.mag_offset_x=(int16_t)(MagneCentreAfterCali.XaxisData*16);
    OffsetToWriteToRegister.mag_offset_y=(int16_t)(MagneCentreAfterCali.YaxisData*16);
    OffsetToWriteToRegister.mag_offset_z=(int16_t)(MagneCentreAfterCali.ZaxisData*16);
    OffsetToWriteToRegister.mag_radius=((int16_t)(MagnRadiuAvg*16))>960?960:((int16_t)(MagnRadiuAvg*16));
    ManuMagneCaliData<<OffsetToWriteToRegister.mag_offset_x<<" "\
                        <<OffsetToWriteToRegister.mag_offset_y<<" "\
                        <<OffsetToWriteToRegister.mag_offset_z<<" "\
                        <<OffsetToWriteToRegister.mag_radius<<"\n";
    //delay(1000);
    //ManuMagneCaliData<<""
    ManuMagneCaliData.close();
    bMagnBiasGeted=true;
}

void BNO055_Cali::InitWithCaliProfile(){
    adafruit_bno055_offsets_t CaliProfileDataGetFromTxt;
    CaliProfileDataGetFromTxt.accel_offset_x=0.0;
    CaliProfileDataGetFromTxt.accel_offset_y=0.0;
    CaliProfileDataGetFromTxt.accel_offset_z=0.0;
    CaliProfileDataGetFromTxt.accel_radius=0.0;
    CaliProfileDataGetFromTxt.gyro_offset_x=0.0;
    CaliProfileDataGetFromTxt.gyro_offset_y=0.0;
    CaliProfileDataGetFromTxt.gyro_offset_z=0.0;
    CaliProfileDataGetFromTxt.mag_offset_x=0.0;
    CaliProfileDataGetFromTxt.mag_offset_y=0.0;
    CaliProfileDataGetFromTxt.mag_offset_z=0.0;
    CaliProfileDataGetFromTxt.mag_radius=0.0;
    int16_t CaliProfileOriDataGetFromTxt[11];
    std::ifstream CaliProfileTxt;
    CaliProfileTxt.open("./CaliProfile.txt");
    if(!CaliProfileTxt) {
        std::cout<<"Open Cali profile txt failed"<<std::endl;
        exit(-1);
    }
    for(int i=0;i<11;i++){
        CaliProfileTxt>>CaliProfileOriDataGetFromTxt[i];
        //std::cout<< i <<"th: "<<CaliProfileOriDataGetFromTxt[i]<<std::endl;
    }

    CaliProfileDataGetFromTxt.accel_offset_x=CaliProfileOriDataGetFromTxt[0];
    CaliProfileDataGetFromTxt.accel_offset_y=CaliProfileOriDataGetFromTxt[1];
    CaliProfileDataGetFromTxt.accel_offset_z=CaliProfileOriDataGetFromTxt[2];
    CaliProfileDataGetFromTxt.accel_radius=CaliProfileOriDataGetFromTxt[3];
    CaliProfileDataGetFromTxt.gyro_offset_x=CaliProfileOriDataGetFromTxt[4];
    CaliProfileDataGetFromTxt.gyro_offset_y=CaliProfileOriDataGetFromTxt[5];
    CaliProfileDataGetFromTxt.gyro_offset_z=CaliProfileOriDataGetFromTxt[6];
    CaliProfileDataGetFromTxt.mag_offset_x=CaliProfileOriDataGetFromTxt[7];
    CaliProfileDataGetFromTxt.mag_offset_y=CaliProfileOriDataGetFromTxt[8];
    CaliProfileDataGetFromTxt.mag_offset_z=CaliProfileOriDataGetFromTxt[9];
    CaliProfileDataGetFromTxt.mag_radius=CaliProfileOriDataGetFromTxt[10];
    if(bMagnBiasGeted){
        CaliProfileDataGetFromTxt.mag_offset_x=OffsetToWriteToRegister.mag_offset_x;
        CaliProfileDataGetFromTxt.mag_offset_y=OffsetToWriteToRegister.mag_offset_y;
        CaliProfileDataGetFromTxt.mag_offset_z=OffsetToWriteToRegister.mag_offset_z;
        CaliProfileDataGetFromTxt.mag_radius=OffsetToWriteToRegister.mag_radius;
    }
    setMode(BNO055_Cali::OPERATION_MODE_CONFIG);
    delay(20);
    setSensorOffsets(CaliProfileDataGetFromTxt);
    delay(20);
    setMode(BNO055_Cali::OPERATION_MODE_NDOF);
    delay(20);
    bInitWithCaliProfileCompleted=true;

}
