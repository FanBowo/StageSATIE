/*Version 1.0 April 18,2019 15:15 Functions of modules GPS test completed */
/*The next version to test GPS with interrupt of serial port*/




#include "Tasks.h"



// Set GPSECHO to 'false' to turn off echoing the GPS data to the Serial console
// Set to 'true' if you want to debug and listen to the raw GPS sentences



void CameraFailed(int sign_no){
//    if(sign_no==SIGINT||sign_no==SIGQUIT||sign_no==SIGSEGV){//Ctrl + C ||
    if(sign_no==SIGINT){//Ctrl + C ||
        std::cout<<"I have get SIGINT"<<std::endl;
        if(AssembleDevice.GPSSerial.IsOpen()){
            AssembleDevice.GPSSerial.close();
        }
        timer_delete(Device_Timer);
        timer_delete(IMU_Timer);
        AssembleDevice.TheCamera.CameraFailed();
        AssembleDevice.pSaveRawIMU_Data.close();
        AssembleDevice.pSaveCamera_IMU_Data.close();
    }
}


//bool RevNewPack =false;
void init()
{
    AssembleDevice.InitGPS_Module();

    sem_init(&IMU_RawDataFifoSem,0,0);
    sem_init(&Camera_IMUDataFifoSem,0,0);

    AssembleDevice.InitIMU_Module();

    InitTimerDevice();

    init_SerialPortInt();
    InitTimerIMU();//i2c1

    AssembleDevice.TheCamera.InitCameraTriggerGPIO();//CAMERA TRIGGER SDO1
    AssembleDevice.TheCamera.InitCameraTriggrtTimer();
    AssembleDevice.TheCamera.InitCameraParas();

    signal(SIGINT,CameraFailed);
//    signal(SIGQUIT,CameraFailed);
//    signal(SIGSEGV,CameraFailed);
}



int main() // run over and over again
{

    init();

    /*
        Interrupt: GPS serial port int to send signal of updating base timestamp
        Interrupt: IMU Timer interrupt to send signal of updating IMU timestamp
                    and of reading raw IMU data
        Interrupt: Cameras Timer interrupt to send signal to take photo
        Interrupt: Receiving a frame of photo Interrupt to save photo into RAM and
                    to read IMU direction data
        Interrupt: Timer interrupt of updating device timestamp timer

        Task:
        Task1:Update base time stamp
        Task2:Read IMU raw data and save into fifo
        Task3:Save raw IMU data to csv file
        Task4-7:Save photos and save IMU direction data to csv file
        Task8:Save photos and save IMU direction data to fifo
        Task9: Update time stamp

        Schedule policy: RR
        Task1:MaxPriorityRR
        Task2:MaxPriorityRR-3
        Task3:MaxPriorityRR-25
        Task4-7:MaxPriorityRR-20
        Task8:MaxPriorityRR-2
        Task9:MaxPriorityRR-1
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

    pthread_t ThreadDevice_UpdateTimeStamp;
    struct sched_param ThreadDevice_UpdateTimeStampPara;
    memset(&ThreadDevice_UpdateTimeStampPara,0,sizeof(sched_param));
    ThreadDevice_UpdateTimeStampPara.__sched_priority=sched_get_priority_max(SCHED_RR)-1;
    pthread_attr_t ThreadDevice_UpdateTimeStampParaAttr;
    pthread_attr_init(&ThreadDevice_UpdateTimeStampParaAttr);
    pthread_attr_setinheritsched(&ThreadDevice_UpdateTimeStampParaAttr,PTHREAD_EXPLICIT_SCHED);
    pthread_attr_setschedpolicy(&ThreadDevice_UpdateTimeStampParaAttr,SCHED_RR);
    pthread_attr_setschedparam(&ThreadDevice_UpdateTimeStampParaAttr,&ThreadDevice_UpdateTimeStampPara);
    pthread_create(&ThreadDevice_UpdateTimeStamp,&ThreadDevice_UpdateTimeStampParaAttr,&UpdateDeviceTimeStampFunc,NULL);

    pthread_t ThreadIMU_UpdateRawData;
    struct sched_param ThreadIMU_UpdateRawDataPara;
    memset(&ThreadIMU_UpdateRawDataPara,0,sizeof(sched_param));
    ThreadIMU_UpdateRawDataPara.__sched_priority=sched_get_priority_max(SCHED_RR)-3;
    pthread_attr_t ThreadIMU_UpdateRawDataParaAttr;
    pthread_attr_init(&ThreadIMU_UpdateRawDataParaAttr);
    pthread_attr_setinheritsched(&ThreadIMU_UpdateRawDataParaAttr,PTHREAD_EXPLICIT_SCHED);
    pthread_attr_setschedpolicy(&ThreadIMU_UpdateRawDataParaAttr,SCHED_RR);
    pthread_attr_setschedparam(&ThreadIMU_UpdateRawDataParaAttr,&ThreadIMU_UpdateRawDataPara);
    pthread_create(&ThreadIMU_UpdateRawData,&ThreadIMU_UpdateRawDataParaAttr,&IMU_UpdateRawDataFunc,NULL);

//    while(1){
//        pthread_mutex_lock(& bIMU_Data_StableMutex );
//            if(AssembleDevice.bIMU_Data_Stable){
//                break;
//            }
//        pthread_mutex_unlock(& bIMU_Data_StableMutex );
//    }

    pthread_t ThreadSaveIMU_RawData;
    struct sched_param ThreadSaveIMU_RawDataPara;
    memset(&ThreadSaveIMU_RawDataPara,0,sizeof(sched_param));
    ThreadSaveIMU_RawDataPara.__sched_priority=sched_get_priority_max(SCHED_RR)-25;
    pthread_attr_t ThreadSaveIMU_RawDataParaAttr;
    pthread_attr_init(&ThreadSaveIMU_RawDataParaAttr);
    pthread_attr_setinheritsched(&ThreadSaveIMU_RawDataParaAttr,PTHREAD_EXPLICIT_SCHED);
    pthread_attr_setschedpolicy(&ThreadSaveIMU_RawDataParaAttr,SCHED_RR);
    pthread_attr_setschedparam(&ThreadSaveIMU_RawDataParaAttr,&ThreadSaveIMU_RawDataPara);
    pthread_create(&ThreadSaveIMU_RawData,&ThreadSaveIMU_RawDataParaAttr,&SaveIMU_RawDataFunc,NULL);

    pthread_t ThreadSaveCamera_IMU_Data;
    struct sched_param ThreadSaveCamera_IMU_DataPara;
    memset(&ThreadSaveCamera_IMU_DataPara,0,sizeof(sched_param));
    ThreadSaveCamera_IMU_DataPara.__sched_priority=sched_get_priority_max(SCHED_RR)-20;
    pthread_attr_t ThreadSaveCamera_IMU_DataParaAttr;
    pthread_attr_init(&ThreadSaveCamera_IMU_DataParaAttr);
    pthread_attr_setinheritsched(&ThreadSaveCamera_IMU_DataParaAttr,PTHREAD_EXPLICIT_SCHED);
    pthread_attr_setschedpolicy(&ThreadSaveCamera_IMU_DataParaAttr,SCHED_RR);
    pthread_attr_setschedparam(&ThreadSaveCamera_IMU_DataParaAttr,&ThreadSaveCamera_IMU_DataPara);
    pthread_create(&ThreadSaveCamera_IMU_Data,&ThreadSaveCamera_IMU_DataParaAttr,&SaveCamera_IMU_DataFunc,NULL);

    pthread_t ThreadSaveCamera_IMU_Data1;
    pthread_create(&ThreadSaveCamera_IMU_Data1,&ThreadSaveCamera_IMU_DataParaAttr,&SaveCamera_IMU_DataFunc,NULL);

    pthread_t ThreadSaveCamera_IMU_Data2;
    pthread_create(&ThreadSaveCamera_IMU_Data2,&ThreadSaveCamera_IMU_DataParaAttr,&SaveCamera_IMU_DataFunc,NULL);

//    pthread_t ThreadSaveCamera_IMU_Data3;
//    pthread_create(&ThreadSaveCamera_IMU_Data3,&ThreadSaveCamera_IMU_DataParaAttr,&SaveCamera_IMU_DataFunc,NULL);

    pthread_t ThreadSaveCamera_IMU_DataToFifo;
    struct sched_param ThreadSaveCamera_IMU_DataToFifoPara;
    memset(&ThreadSaveCamera_IMU_DataToFifoPara,0,sizeof(sched_param));
    ThreadSaveCamera_IMU_DataToFifoPara.__sched_priority=sched_get_priority_max(SCHED_RR)-2;
    pthread_attr_t ThreadSaveCamera_IMU_DataToFifoParaAttr;
    pthread_attr_init(&ThreadSaveCamera_IMU_DataToFifoParaAttr);
    pthread_attr_setinheritsched(&ThreadSaveCamera_IMU_DataToFifoParaAttr,PTHREAD_EXPLICIT_SCHED);
    pthread_attr_setschedpolicy(&ThreadSaveCamera_IMU_DataToFifoParaAttr,SCHED_RR);
    pthread_attr_setschedparam(&ThreadSaveCamera_IMU_DataToFifoParaAttr,&ThreadSaveCamera_IMU_DataToFifoPara);
    pthread_create(&ThreadSaveCamera_IMU_DataToFifo,&ThreadSaveCamera_IMU_DataToFifoParaAttr,&SaveCamera_IMU_DataToFifoFunc,NULL);


    pthread_join(ThreadUpdateTimeStampBase,NULL);
    pthread_join(ThreadDevice_UpdateTimeStamp,NULL);
    pthread_join(ThreadIMU_UpdateRawData,NULL);
    pthread_join(ThreadSaveIMU_RawData,NULL);
    pthread_join(ThreadSaveCamera_IMU_Data,NULL);
    pthread_join(ThreadSaveCamera_IMU_Data1,NULL);
    pthread_join(ThreadSaveCamera_IMU_Data2,NULL);
//    pthread_join(ThreadSaveCamera_IMU_Data3,NULL);
    pthread_join(ThreadSaveCamera_IMU_DataToFifo,NULL);

    std::cout<<"End of programme"<<std::endl;
}
