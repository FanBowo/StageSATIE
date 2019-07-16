/*Version 1.0 April 18,2019 15:15 Functions of modules GPS test completed */
/*The next version to test GPS with interrupt of serial port*/




#include "Tasks.h"



// Set GPSECHO to 'false' to turn off echoing the GPS data to the Serial console
// Set to 'true' if you want to debug and listen to the raw GPS sentences





//bool RevNewPack =false;
void init()
{
    init_SerialPortInt();
    InitTimerIMU();
}


int main() // run over and over again
{
    AssembleDevice.InitGPS_Module();
    AssembleDevice.InitIMU_Module();
    init();

    std::cout<<"Initialization finished"<<std::endl;
    pthread_t ThreadUpdateTimeStampBase;
    pthread_create(&ThreadUpdateTimeStampBase,NULL,&UpdateTimeStampBaseFunc,NULL);

    pthread_t ThreadIMU_UpdateTimeStamp;
    pthread_create(&ThreadIMU_UpdateTimeStamp,NULL,&IMU_UpdateTimeStampFunc,NULL);

    pthread_join(ThreadUpdateTimeStampBase,NULL);
    pthread_join(ThreadIMU_UpdateTimeStamp,NULL);

    std::cout<<"End of programme"<<std::endl;
}
