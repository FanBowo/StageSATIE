#ifndef _DATACALIBRATION_H_
#define _DATACALIBRATION_H_
#include <eigen3/Eigen/Dense>
#include <vector>
typedef struct{
    float XaxisData;
    float YaxisData;
    float ZaxisData;
}CentreAfterCali_t;

#define NUM_DATA_NEEDED 3000
//typedef Matrix<float, Dynamic, 9> MatrixDataProduct;

class DataCalibration
{
public:
    DataCalibration();
    std::vector<float> XaxisData;
    std::vector<float> YaxisData;
    std::vector<float> ZaxisData;
    CentreAfterCali_t CentreAfterCali;
    void Calibration();

private:
     Eigen::MatrixXf XaxisDataM;
     Eigen::MatrixXf YaxisDataM;
     Eigen::MatrixXf ZaxisDataM;
     Eigen::Matrix<float, 1, 3> CentreAfterCaliData;
     void ConvertStdType2EigenType();
};


#endif
