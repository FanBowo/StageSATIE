#ifndef BNO055_CALI_H_INCLUDED
#define BNO055_CALI_H_INCLUDED

#include "Adafruit_BNO055.h"
#include "util/DataCalibration.h"

class BNO055_Cali:public Adafruit_BNO055, public DataCalibration{
    public:
    BNO055_Cali();
    long DataCollectedCount;
    void CaliMagneDataCollectAndCali();


};

#endif // BNO055_CALI_H_INCLUDED
