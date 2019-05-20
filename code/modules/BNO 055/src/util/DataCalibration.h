#ifndef _DATACALIBRATION_H_
#define _DATACALIBRATION_H_
#include <eigen3/Eigen/Dense>
#include <vector>
typedef struct{
    float XaxisData;
    float YaxisData;
    float ZaxisData;
}CentreAfterCali_t;

#define NUM_DATA_NEEDED 300
//typedef Matrix<float, Dynamic, 9> MatrixDataProduct;

class DataCalibration
{
public:
    DataCalibration();
    std::vector<float> XaxisData;
    std::vector<float> YaxisData;
    std::vector<float> ZaxisData;
    CentreAfterCali_t MagneCentreAfterCali;
    CentreAfterCali_t AcceCentreAfterCali;
    CentreAfterCali_t GyroCentreAfterCali;
    Eigen::Matrix<float, 1, 3> CentreAfterCaliData;

    void MagnAcceCalibration();
    float XaxisGyroDataSum;
    float YaxisGyroDataSum;
    float ZaxisGyroDataSum;
    void GyroCalibration();

private:
     Eigen::MatrixXf XaxisDataM;
     Eigen::MatrixXf YaxisDataM;
     Eigen::MatrixXf ZaxisDataM;
     void ConvertStdType2EigenType();
};


#endif
