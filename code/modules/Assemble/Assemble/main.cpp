/*Version 1.0 April 18,2019 15:15 Functions of modules GPS test completed */
/*The next version to test GPS with interrupt of serial port*/




#include "Tasks.h"



// Set GPSECHO to 'false' to turn off echoing the GPS data to the Serial console
// Set to 'true' if you want to debug and listen to the raw GPS sentences





//bool RevNewPack =false;
void init()
{
    AssembleDevice.InitGPS_Module();

    sem_init(&IMU_RawDataFifoSem,0,0);

    AssembleDevice.InitIMU_Module();

    init_SerialPortInt();
    InitTimerIMU();
}


int main() // run over and over again
{

    init();
    /*
        Interrupt: GPS serial port int to send signal of updating base timestamp
        Interrupt: IMU Timer interrupt to send signal of updating IMU timestamp
                    and of reading raw IMU data
        Interrupt: Cameras Timer interrupt to send signal of updating Camera timestamp
                    of taking photo,and of reading IMU direction data
        Task:
        Task1:Update base time stamp
        Task2:Update IMU timestamp and read IMU raw data
        Task3:Save raw IMU dato to csv file
        Task4:Camera timestamp,taking photo,and read IMU direction data
        Task5:Save photos and save IMU direction data to csv file

        Schedule policy: RR
        Task1:MaxPriorityRR
        Task2:MaxPriorityRR-1
        Task3:MaxPriorityRR-3
        Task4:MaxPriorityRR-1
        Task5:MaxPriorityRR-2

    */

    std::cout<<"Initialization finished"<<std::endl;
    pthread_t ThreadUpdateTimeStampBase;
    struct sched_param ThreadUpdateTimeStampBasePara;
    memset(&ThreadUpdateTimeStampBasePara,0,sizeof(sched_param));
    ThreadUpdateTimeStampBasePara.__sched_priority=sched_get_priority_max(SCHED_RR);
    pthread_attr_t ThreadUpdateTimeStampBaseParaAttr;
    pthread_attr_init(&ThreadUpdateTimeStampBaseParaAttr);
    pthread_attr_setinheritsched(&ThreadUpdateTimeStampBaseParaAttr,PTHREAD_EXPLICIT_SCHED);
    pthread_attr_setschedpolicy(&ThreadUpdateTimeStampBaseParaAttr,SCHED_RR);
    pthread_attr_setschedparam(&ThreadUpdateTimeStampBaseParaAttr,&ThreadUpdateTimeStampBasePara);
    pthread_create(&ThreadUpdateTimeStampBase,&ThreadUpdateTimeStampBaseParaAttr,&UpdateTimeStampBaseFunc,NULL);

    pthread_t ThreadIMU_UpdateTimeStamp;
    struct sched_param ThreadIMU_UpdateTimeStampPara;
    memset(&ThreadUpdateTimeStampBasePara,0,sizeof(sched_param));
    ThreadUpdateTimeStampBasePara.__sched_priority=sched_get_priority_max(SCHED_RR)-1;
    pthread_attr_t ThreadIMU_UpdateTimeStampParaAttr;
    pthread_attr_init(&ThreadIMU_UpdateTimeStampParaAttr);
    pthread_attr_setinheritsched(&ThreadIMU_UpdateTimeStampParaAttr,PTHREAD_EXPLICIT_SCHED);
    pthread_attr_setschedpolicy(&ThreadIMU_UpdateTimeStampParaAttr,SCHED_RR);
    pthread_attr_setschedparam(&ThreadIMU_UpdateTimeStampParaAttr,&ThreadIMU_UpdateTimeStampPara);
    pthread_create(&ThreadIMU_UpdateTimeStamp,&ThreadIMU_UpdateTimeStampParaAttr,&IMU_UpdateTimeStampFunc,NULL);

    pthread_t ThreadSaveIMU_RawData;
    struct sched_param ThreadSaveIMU_RawDataPara;
    memset(&ThreadSaveIMU_RawDataPara,0,sizeof(sched_param));
    ThreadSaveIMU_RawDataPara.__sched_priority=sched_get_priority_max(SCHED_RR)-1;
    pthread_attr_t ThreadSaveIMU_RawDataParaAttr;
    pthread_attr_init(&ThreadSaveIMU_RawDataParaAttr);
    pthread_attr_setinheritsched(&ThreadSaveIMU_RawDataParaAttr,PTHREAD_EXPLICIT_SCHED)-3;
    pthread_attr_setschedpolicy(&ThreadSaveIMU_RawDataParaAttr,SCHED_RR);
    pthread_attr_setschedparam(&ThreadSaveIMU_RawDataParaAttr,&ThreadSaveIMU_RawDataPara);
    pthread_create(&ThreadSaveIMU_RawData,&ThreadSaveIMU_RawDataParaAttr,&SaveIMU_RawDataFunc,NULL);

    pthread_join(ThreadUpdateTimeStampBase,NULL);
    pthread_join(ThreadIMU_UpdateTimeStamp,NULL);
    pthread_join(ThreadSaveIMU_RawData,NULL);

    std::cout<<"End of programme"<<std::endl;
}
