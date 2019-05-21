#ifndef BNO055_CALI_H_INCLUDED
#define BNO055_CALI_H_INCLUDED

#include "Adafruit_BNO055.h"
#include "util/DataCalibration.h"

class BNO055_Cali:public Adafruit_BNO055, public DataCalibration{
    public:
    BNO055_Cali();
    long DataCollectedCount;
    void CaliMagneDataCollectAndCali();
    void WriteMagneBias();
    volatile bool bMagnBiasGeted;
    adafruit_bno055_offsets_t OffsetToWriteToRegister;
    void InitWithCaliProfile();
    volatile bool bInitWithCaliProfileCompleted;
    //volatile bool bNeedReCalibration;
    //bool begin(adafruit_bno055_opmode_t mode);

};

#endif // BNO055_CALI_H_INCLUDED
