#ifndef IMU_MODULE
#define IMU_MODULE

#include "SparkFunMPU9250-DMP.h"
#include "DataCalibration.h"

class ImuModule :public MPU9250_DMP, public DataCalibration{

    public :
    ImuModule();

    void CollectDataAndCali();

    private:

    long _DataMagnCollected;
};
#endif // IMU_MODULE
