#include "imu_module.hpp"
#include <iostream>
#include <fstream>
ImuModule::ImuModule()
{
    _DataCaliCollected=0;
    move_data.accelX =0;
    move_data.accelY =0;
    move_data.accelZ =0;
    move_data.gyroX =0;
    move_data.gyroY =0;
    move_data.gyroZ =0;
    move_data.magX =0;
    move_data.magY =0;
    move_data.magZ =0;


}


//std::ofstream MagneCaliData;

void ImuModule::CollectMagDataAndCali(){
    // MagneCaliData.open("./MagneCaliData.txt",std::ios::trunc|std::ios::binary |std::ios::in|std::ios::out);

    XaxisData.clear();
    YaxisData.clear();
    ZaxisData.clear();
    for (_DataCaliCollected =0;_DataCaliCollected<NUM_DATA_NEEDED;_DataCaliCollected++){
        while(!dataReady()){
            ;
        }
        MPU9250_DMP::update(UPDATE_ACCEL | UPDATE_GYRO | UPDATE_COMPASS);
        XaxisData.push_back(calcMag(mx)/100.0);
        YaxisData.push_back(calcMag(my)/100.0);
        ZaxisData.push_back(calcMag(mz)/100.0);
        std::cout<<XaxisData.back()<<"   "<<YaxisData.back()<<"   "<<ZaxisData.back()<<"   "<<std::endl;
        //MagneCaliData<<XaxisData.back()<<" "<<YaxisData.back()<<" "<<ZaxisData.back()<<"\n";
    }
    //MagneCaliData.close();
    std::cout << _DataCaliCollected << "/" << NUM_DATA_NEEDED << "collected"<<std::endl;
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

}

void ImuModule::CollectAccGyrDataAndCali(){
    // MagneCaliData.open("./MagneCaliData.txt",std::ios::trunc|std::ios::binary |std::ios::in|std::ios::out);

    XaxisGyroDataSum=0.0;
    YaxisGyroDataSum=0.0;
    ZaxisGyroDataSum=0.0;
    XaxisData.clear();
    YaxisData.clear();
    ZaxisData.clear();
    for (_DataCaliCollected =0;_DataCaliCollected<NUM_DATA_NEEDED;_DataCaliCollected++){
        while(!dataReady()){
            ;
        }
        MPU9250_DMP::update(UPDATE_ACCEL | UPDATE_GYRO | UPDATE_COMPASS);
        XaxisData.push_back(calcAccel(ax));
        YaxisData.push_back(calcAccel(ay));
        ZaxisData.push_back(calcAccel(az));
        XaxisGyroDataSum+=calcGyro(gx);
        YaxisGyroDataSum+=calcGyro(gy);
        ZaxisGyroDataSum+=calcGyro(gz);
        std::cout<<XaxisData.back()<<"   "<<YaxisData.back()<<"   "<<ZaxisData.back()<<"   "<<std::endl;
        //MagneCaliData<<XaxisData.back()<<" "<<YaxisData.back()<<" "<<ZaxisData.back()<<"\n";
    }
    //MagneCaliData.close();
    std::cout << _DataCaliCollected << "/" << NUM_DATA_NEEDED << "collected"<<std::endl;
    //std::cout<<"XaxisData Size"<<XaxisData.size()<<std::endl;
    std::cout << "Acc qnd Gyro Data collected finished, start of calcule bias"<<std::endl;
    MagnAcceCalibration();
    AcceCentreAfterCali.XaxisData=CentreAfterCaliData(0,0);
    AcceCentreAfterCali.YaxisData=CentreAfterCaliData(0,1);
    AcceCentreAfterCali.ZaxisData=CentreAfterCaliData(0,2);
    std::cout << "Acc bias:"<<std::endl;
    std::cout<<"X bias"<<AcceCentreAfterCali.XaxisData<<std::endl;
    std::cout<<"Y bias"<<AcceCentreAfterCali.YaxisData<<std::endl;
    std::cout<<"Z bias"<<AcceCentreAfterCali.ZaxisData<<std::endl;
    GyroCalibration();
    std::cout << "Gyro bias:"<<std::endl;
    std::cout<<"X bias"<<GyroCentreAfterCali.XaxisData<<std::endl;
    std::cout<<"Y bias"<<GyroCentreAfterCali.YaxisData<<std::endl;
    std::cout<<"Z bias"<<GyroCentreAfterCali.ZaxisData<<std::endl;
}


void ImuModule::CalMoveData(){
    move_data.accelX = calcAccel(ax);
    move_data.accelY = calcAccel(ay);
    move_data.accelZ = calcAccel(az);
    move_data.gyroX = calcGyro(gx)-GyroCentreAfterCali.XaxisData;
    move_data.gyroY = calcGyro(gy)-GyroCentreAfterCali.YaxisData;
    move_data.gyroZ = calcGyro(gz)-GyroCentreAfterCali.ZaxisData;
    move_data.magX = calcMag(mx)/100.0-MagneCentreAfterCali.XaxisData;
    move_data.magY = calcMag(my)/100.0-MagneCentreAfterCali.YaxisData;
    move_data.magZ = calcMag(mz)/100.0-MagneCentreAfterCali.ZaxisData;

}

#define PI 3.141592654f
void ImuModule::EstimationPose(){
    Madgwick_cpp::update(move_data.gyroX*PI/180.0f,move_data.gyroY*PI/180.0f,move_data.gyroZ*PI/180.0f, \
                        move_data.accelX,move_data.accelY,move_data.accelZ, \
                        move_data.magX,move_data.magY, move_data.magZ
    );

}
