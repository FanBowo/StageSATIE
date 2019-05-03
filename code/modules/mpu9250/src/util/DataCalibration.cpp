#include "DataCalibration.h"
#include <fstream>

DataCalibration::DataCalibration(){
    MagneCentreAfterCali.XaxisData=-0.75f;
    MagneCentreAfterCali.YaxisData=0.41f;
    MagneCentreAfterCali.ZaxisData=1.23f;
    AcceCentreAfterCali.XaxisData=0.0f;
    AcceCentreAfterCali.YaxisData=0.0f;
    AcceCentreAfterCali.ZaxisData=0.0f;
    GyroCentreAfterCali.XaxisData=-1.03f;
    GyroCentreAfterCali.YaxisData=0.96f;
    GyroCentreAfterCali.ZaxisData=-0.26f;
    CentreAfterCaliData(0,0)=0.0;
    CentreAfterCaliData(0,1)=0.0;
    CentreAfterCaliData(0,2)=0.0;
    ;
}
void DataCalibration::ConvertStdType2EigenType(){
    assert((XaxisData.size()==YaxisData.size())&&(YaxisData.size()==ZaxisData.size()));
    assert(XaxisData.size()==NUM_DATA_NEEDED);
    XaxisDataM=Eigen::Map<Eigen::Matrix<float, NUM_DATA_NEEDED, 1> >(XaxisData.data());
    YaxisDataM=Eigen::Map<Eigen::Matrix<float, NUM_DATA_NEEDED, 1> >(YaxisData.data());
    ZaxisDataM=Eigen::Map<Eigen::Matrix<float, NUM_DATA_NEEDED, 1> >(ZaxisData.data());
}

void DataCalibration::MagnAcceCalibration(){
    ConvertStdType2EigenType();
    Eigen::Matrix<float, NUM_DATA_NEEDED, 9> DataOriProduct;
    DataOriProduct.col(0)=XaxisDataM.cwiseProduct(XaxisDataM);
    DataOriProduct.col(1)=YaxisDataM.cwiseProduct(YaxisDataM);
    DataOriProduct.col(2)=ZaxisDataM.cwiseProduct(ZaxisDataM);
    DataOriProduct.col(3)=XaxisDataM.cwiseProduct(YaxisDataM);
    DataOriProduct.col(4)=XaxisDataM.cwiseProduct(ZaxisDataM);
    DataOriProduct.col(5)=YaxisDataM.cwiseProduct(ZaxisDataM);
    DataOriProduct.col(6)=XaxisDataM;
    DataOriProduct.col(7)=YaxisDataM;
    DataOriProduct.col(8)=ZaxisDataM;
    Eigen::MatrixXf MatrixI=Eigen::MatrixXf::Ones(NUM_DATA_NEEDED,1);
    Eigen::Matrix<float, 9, 1> NineParas;
    NineParas =DataOriProduct.fullPivHouseholderQr().solve(MatrixI);
    Eigen::Matrix<float, 3, 3> NineParasM;
    NineParasM<<NineParas(0,0),NineParas(3,0)/2.0,NineParas(4,0)/2.0,
                NineParas(3,0)/2.0,NineParas(1,0),NineParas(5,0)/2.0,
                NineParas(4,0)/2.0,NineParas(5,0)/2.0,NineParas(2,0);
    Eigen::Matrix<float, 1, 3> TempData;
    TempData<<(-0.5*NineParas(6,0)),(-0.5*NineParas(7,0)),(-0.5*NineParas(8,0));
    CentreAfterCaliData=TempData*NineParasM.inverse();

}

void DataCalibration::GyroCalibration(){
    GyroCentreAfterCali.XaxisData=XaxisGyroDataSum/float (NUM_DATA_NEEDED);
    GyroCentreAfterCali.YaxisData=YaxisGyroDataSum/float (NUM_DATA_NEEDED);
    GyroCentreAfterCali.ZaxisData=ZaxisGyroDataSum/float (NUM_DATA_NEEDED);
}
