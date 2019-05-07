#include <iostream>
#include "src/util/DataCalibration.h"
#include <random>
using namespace std;
int main()
{
    cout << "Hello world!" << endl;

    std::default_random_engine generator;
    std::normal_distribution<float> distributionX(40.0,400);
    std::normal_distribution<float> distributionY(15.5,150);
    std::normal_distribution<float> distributionZ(26.0,200);

    DataCalibration DataRamdom3Axi;

    for (int i=0; i<NUM_DATA_NEEDED; ++i) {
        DataRamdom3Axi.XaxisData.push_back(distributionX(generator));
        DataRamdom3Axi.YaxisData.push_back(distributionY(generator));
        DataRamdom3Axi.ZaxisData.push_back(distributionZ(generator));
    }
    DataRamdom3Axi.Calibration();
    cout<<"X bias:"<<DataRamdom3Axi.CentreAfterCali.XaxisData<<endl;
    cout<<"Y bias:"<<DataRamdom3Axi.CentreAfterCali.YaxisData<<endl;
    cout<<"Z bias:"<<DataRamdom3Axi.CentreAfterCali.ZaxisData<<endl;
    return 0;
}
