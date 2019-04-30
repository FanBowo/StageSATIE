#include "imu_module.hpp"
#include <iostream>
ImuModule::ImuModule()
{
    _DataMagnCollected=0;
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

void ImuModule::CollectDataAndCali(){

    for (_DataMagnCollected =0;_DataMagnCollected<NUM_DATA_NEEDED;_DataMagnCollected++){
        while(!dataReady()){
            ;
        }
        MPU9250_DMP::update(UPDATE_ACCEL | UPDATE_GYRO | UPDATE_COMPASS);
        XaxisData.push_back(calcMag(mx));
        YaxisData.push_back(calcMag(mx));
        ZaxisData.push_back(calcMag(my));
        std::cout<<XaxisData.back()<<"   "<<YaxisData.back()<<"   "<<ZaxisData.back()<<"   "<<std::endl;
    }
    std::cout << _DataMagnCollected << "/" << NUM_DATA_NEEDED << "collected"<<std::endl;
    //std::cout<<"XaxisData Size"<<XaxisData.size()<<std::endl;
    std::cout << "Magnetic Data collected finished, start of calcule bias"<<std::endl;
    Calibration();
    std::cout<<"X bias"<<CentreAfterCali.XaxisData<<std::endl;
    std::cout<<"Y bias"<<CentreAfterCali.YaxisData<<std::endl;
    std::cout<<"Z bias"<<CentreAfterCali.ZaxisData<<std::endl;

}

void ImuModule::CalMoveData(){
    move_data.accelX = calcAccel(ax);
    move_data.accelY = calcAccel(ay);
    move_data.accelZ = calcAccel(az);
    move_data.gyroX = calcGyro(gx);
    move_data.gyroY = calcGyro(gy);
    move_data.gyroZ = calcGyro(gz);
    move_data.magX = calcMag(mx);
    move_data.magY = calcMag(my);
    move_data.magZ = calcMag(mz);

}

void ImuModule::EstimationPose(){
    Madgwick::update(move_data.accelX,move_data.accelY,move_data.accelZ, \
                        move_data.gyroX,move_data.gyroY,move_data.gyroZ,\
                        move_data.magX,move_data.magY,move_data.magZ \
    );

}
