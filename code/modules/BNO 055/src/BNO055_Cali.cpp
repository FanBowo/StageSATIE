#include "BNO055_Cali.h"

BNO055_Cali::BNO055_Cali(){
    DataCollectedCount=0;
}
//We collecte data in interrupt and do calibration directely with

void BNO055_Cali::CollectMagneDataAndCali(){

    MagnAcceCalibration();
}
