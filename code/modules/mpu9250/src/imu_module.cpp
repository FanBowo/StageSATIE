#include "imu_module.hpp"
#include <iostream>
ImuModule::ImuModule()
{
    _DataMagnCollected=0;
}

void ImuModule::CollectDataAndCali(){

    for (_DataMagnCollected =0;_DataMagnCollected<NUM_DATA_NEEDED;_DataMagnCollected++){
        while(!dataReady()){
            ;
        }
        update(UPDATE_ACCEL | UPDATE_GYRO | UPDATE_COMPASS);
        XaxisData.push_back(calcMag(mx));
        YaxisData.push_back(calcMag(mx));
        ZaxisData.push_back(calcMag(my));
    }
    std::cout << _DataMagnCollected << "/" << NUM_DATA_NEEDED << "collected"<<std::endl;
    //std::cout<<"XaxisData Size"<<XaxisData.size()<<std::endl;
    std::cout << "Magnetic Data collected finished, start of calcule bias"<<std::endl;
    Calibration();
    std::cout<<"X bias"<<CentreAfterCali.XaxisData<<std::endl;
    std::cout<<"Y bias"<<CentreAfterCali.YaxisData<<std::endl;
    std::cout<<"Z bias"<<CentreAfterCali.ZaxisData<<std::endl;

}
