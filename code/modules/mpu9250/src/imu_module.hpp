#ifndef IMU_MODULE
#define IMU_MODULE

#include "SparkFunMPU9250-DMP.h"
#include "DataCalibration.h"
#include "MadgwickAHRS.h"

typedef struct{
    float accelX ;
    float accelY ;
    float accelZ ;
    float gyroX ;
    float gyroY ;
    float gyroZ ;
    float magX ;
    float magY ;
    float magZ ;
}MovData_t;



class ImuModule :public MPU9250_DMP, public DataCalibration,public Madgwick{

    public :
    ImuModule();

    void CollectDataAndCali();

    void CalMoveData();
    MovData_t move_data;

    void EstimationPose();

    private:

    long _DataMagnCollected;
};
#endif // IMU_MODULE
