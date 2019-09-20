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
//    AssembleDevice.InitGPS_Module();
    AssembleDevice.GPS.begin(9600);
    delay(1000);
    sem_init(&SEM_FIFO_BRUT_IMU,0,0);
    sem_init(&SEM_FIFO_IMAGE_IMU,0,0);
    sem_init(&SEM_ FIFO_INFO_GPS,0,0);


    AssembleDevice.InitIMU_Module();

    InitTimerDevice();

    init_SerialPortInt();
    InitTimerIMU();//i2c1

    AssembleDevice.TheCamera.InitCameraTriggerGPIO();//CAMERA TRIGGER SDO1
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
		Interrupt: Timer interrupt of updating device timestamp timer
        Interrupt: IMU Timer interrupt to send signal of updating IMU timestamp
                    and of reading raw IMU data
        Interrupt: Cameras Timer interrupt to send signal to take photo
        Interrupt: Receiving a frame of photo Interrupt to save photo into RAM and
                    to read IMU direction data
        

        Task:
		
        Task1:THREAD_RENOUVELER_BASE_HORODATAGE:Update base time stamp
		MaxPriorityRR-2
		
		Task2: THREAD_RENOUVELER_HORODATAGE:Update time stamp
		MaxPriorityRR-6
		
		Task3:THREAD_RENOUVELER_GPS_FIFO:Update GPS Data fifo
		MaxPriorityRR-6
		
		Task4:THREAD_LIRE_IMU_BRU:Read IMU raw data and save into fifo
		MaxPriorityRR-10
		
		Task5:THREAD_SAUVEGARDER_BRUT_IMU:Save raw IMU data to csv file
		MaxPriorityRR-30
		
		Task6-9:THREAD_SAUVEGARDER_BRUT_IMU:Save photos and save IMU direction data to csv file
		MaxPriorityRR-30
		
		Task10:THREAD_LIRE_IMAGE:Get photos and save IMU direction data to fifo
		MaxPriorityRR-12
		
		Task11:THREAD_SAUVEGARDER_INFO_GPS:Save raw GPS data to csv file
        MaxPriorityRR-30
   
    */

    std::cout<<"Initialization finished"<<std::endl;

    pthread_t THREAD_RENOUVELER_BASE_HORODATAGE;
    struct sched_param THREAD_RENOUVELER_BASE_HORODATAGEPara;
    memset(&THREAD_RENOUVELER_BASE_HORODATAGEPara,0,sizeof(sched_param));
    THREAD_RENOUVELER_BASE_HORODATAGEPara.__sched_priority=sched_get_priority_max(SCHED_RR)-2;
    pthread_attr_t THREAD_RENOUVELER_BASE_HORODATAGEParaAttr;
    pthread_attr_init(&THREAD_RENOUVELER_BASE_HORODATAGEParaAttr);
    pthread_attr_setinheritsched(&THREAD_RENOUVELER_BASE_HORODATAGEParaAttr,PTHREAD_EXPLICIT_SCHED);
    pthread_attr_setschedpolicy(&THREAD_RENOUVELER_BASE_HORODATAGEParaAttr,SCHED_RR);
    pthread_attr_setschedparam(&THREAD_RENOUVELER_BASE_HORODATAGEParaAttr,&THREAD_RENOUVELER_BASE_HORODATAGEPara);
    pthread_create(&THREAD_RENOUVELER_BASE_HORODATAGE,&THREAD_RENOUVELER_BASE_HORODATAGEParaAttr,&UpdateTimeStampBaseFunc,NULL);

    pthread_t THREAD_RENOUVELER_HORODATAGE;
    struct sched_param THREAD_RENOUVELER_HORODATAGEPara;
    memset(&THREAD_RENOUVELER_HORODATAGEPara,0,sizeof(sched_param));
    THREAD_RENOUVELER_HORODATAGEPara.__sched_priority=sched_get_priority_max(SCHED_RR)-6;
    pthread_attr_t THREAD_RENOUVELER_HORODATAGEParaAttr;
    pthread_attr_init(&THREAD_RENOUVELER_HORODATAGEParaAttr);
    pthread_attr_setinheritsched(&THREAD_RENOUVELER_HORODATAGEParaAttr,PTHREAD_EXPLICIT_SCHED);
    pthread_attr_setschedpolicy(&THREAD_RENOUVELER_HORODATAGEParaAttr,SCHED_RR);
    pthread_attr_setschedparam(&THREAD_RENOUVELER_HORODATAGEParaAttr,&THREAD_RENOUVELER_HORODATAGEPara);
    pthread_create(&THREAD_RENOUVELER_HORODATAGE,&THREAD_RENOUVELER_HORODATAGEParaAttr,&UpdateDeviceTimeStampFunc,NULL);

    pthread_t THREAD_RENOUVELER_GPS_FIFO;
    struct sched_param THREAD_RENOUVELER_GPS_FIFOPara;
    memset(&THREAD_RENOUVELER_GPS_FIFOPara,0,sizeof(sched_param));
    THREAD_RENOUVELER_GPS_FIFOPara.__sched_priority=sched_get_priority_max(SCHED_RR)-3;
    pthread_attr_t THREAD_RENOUVELER_GPS_FIFOParaAttr;
    pthread_attr_init(&THREAD_RENOUVELER_GPS_FIFOParaAttr);
    pthread_attr_setinheritsched(&THREAD_RENOUVELER_GPS_FIFOParaAttr,PTHREAD_EXPLICIT_SCHED);
    pthread_attr_setschedpolicy(&THREAD_RENOUVELER_GPS_FIFOParaAttr,SCHED_RR);
    pthread_attr_setschedparam(&THREAD_RENOUVELER_GPS_FIFOParaAttr,&THREAD_RENOUVELER_GPS_FIFOPara);
    pthread_create(&THREAD_RENOUVELER_GPS_FIFO,&THREAD_RENOUVELER_GPS_FIFOParaAttr,&GPS_UpdateFIFOFunc,NULL);

    pthread_t THREAD_LIRE_IMU_BRU;
    struct sched_param THREAD_LIRE_IMU_BRUPara;
    memset(&THREAD_LIRE_IMU_BRUPara,0,sizeof(sched_param));
    THREAD_LIRE_IMU_BRUPara.__sched_priority=sched_get_priority_max(SCHED_RR)-10;
    pthread_attr_t THREAD_LIRE_IMU_BRUParaAttr;
    pthread_attr_init(&THREAD_LIRE_IMU_BRUParaAttr);
    pthread_attr_setinheritsched(&THREAD_LIRE_IMU_BRUParaAttr,PTHREAD_EXPLICIT_SCHED);
    pthread_attr_setschedpolicy(&THREAD_LIRE_IMU_BRUParaAttr,SCHED_RR);
    pthread_attr_setschedparam(&THREAD_LIRE_IMU_BRUParaAttr,&THREAD_LIRE_IMU_BRUPara);
    pthread_create(&THREAD_LIRE_IMU_BRU,&THREAD_LIRE_IMU_BRUParaAttr,&IMU_UpdateRawDataFunc,NULL);


    pthread_t THREAD_SAUVEGARDER_BRUT_IMU;
    struct sched_param THREAD_SAUVEGARDER_BRUT_IMUPara;
    memset(&THREAD_SAUVEGARDER_BRUT_IMUPara,0,sizeof(sched_param));
    THREAD_SAUVEGARDER_BRUT_IMUPara.__sched_priority=sched_get_priority_max(SCHED_RR)-30;
    pthread_attr_t THREAD_SAUVEGARDER_BRUT_IMUParaAttr;
    pthread_attr_init(&THREAD_SAUVEGARDER_BRUT_IMUParaAttr);
    pthread_attr_setinheritsched(&THREAD_SAUVEGARDER_BRUT_IMUParaAttr,PTHREAD_EXPLICIT_SCHED);
    pthread_attr_setschedpolicy(&THREAD_SAUVEGARDER_BRUT_IMUParaAttr,SCHED_RR);
    pthread_attr_setschedparam(&THREAD_SAUVEGARDER_BRUT_IMUParaAttr,&THREAD_SAUVEGARDER_BRUT_IMUPara);
    pthread_create(&THREAD_SAUVEGARDER_BRUT_IMU,&THREAD_SAUVEGARDER_BRUT_IMUParaAttr,&SaveIMU_RawDataFunc,NULL);

    pthread_t THREAD_SAUVEGARDER_CAMERA_IMU;
    struct sched_param THREAD_SAUVEGARDER_CAMERA_IMUPara;
    memset(&THREAD_SAUVEGARDER_CAMERA_IMUPara,0,sizeof(sched_param));
    THREAD_SAUVEGARDER_CAMERA_IMUPara.__sched_priority=sched_get_priority_max(SCHED_RR)-30;
    pthread_attr_t THREAD_SAUVEGARDER_CAMERA_IMUParaAttr;
    pthread_attr_init(&THREAD_SAUVEGARDER_CAMERA_IMUParaAttr);
    pthread_attr_setstacksize(&THREAD_SAUVEGARDER_CAMERA_IMUParaAttr,33554432);
    pthread_attr_setinheritsched(&THREAD_SAUVEGARDER_CAMERA_IMUParaAttr,PTHREAD_EXPLICIT_SCHED);
    pthread_attr_setschedpolicy(&THREAD_SAUVEGARDER_CAMERA_IMUParaAttr,SCHED_RR);
    pthread_attr_setschedparam(&THREAD_SAUVEGARDER_CAMERA_IMUParaAttr,&THREAD_SAUVEGARDER_CAMERA_IMUPara);
    pthread_create(&THREAD_SAUVEGARDER_CAMERA_IMU,&THREAD_SAUVEGARDER_CAMERA_IMUParaAttr,&SaveCamera_IMU_DataFunc,NULL);

    pthread_t THREAD_SAUVEGARDER_CAMERA_IMU1;
    pthread_create(&THREAD_SAUVEGARDER_CAMERA_IMU1,&THREAD_SAUVEGARDER_CAMERA_IMUParaAttr,&SaveCamera_IMU_DataFunc,NULL);

    pthread_t THREAD_SAUVEGARDER_CAMERA_IMU2;
    pthread_create(&THREAD_SAUVEGARDER_CAMERA_IMU2,&THREAD_SAUVEGARDER_CAMERA_IMUParaAttr,&SaveCamera_IMU_DataFunc,NULL);

//    pthread_t THREAD_SAUVEGARDER_CAMERA_IMU3;
//    pthread_create(&THREAD_SAUVEGARDER_CAMERA_IMU3,&THREAD_SAUVEGARDER_CAMERA_IMUParaAttr,&SaveCamera_IMU_DataFunc,NULL);

    pthread_t THREAD_LIRE_IMAGE;
    struct sched_param THREAD_LIRE_IMAGEPara;
    memset(&THREAD_LIRE_IMAGEPara,0,sizeof(sched_param));
    THREAD_LIRE_IMAGEPara.__sched_priority=sched_get_priority_max(SCHED_RR)-12;
    pthread_attr_t THREAD_LIRE_IMAGEParaAttr;
    pthread_attr_init(&THREAD_LIRE_IMAGEParaAttr);
    pthread_attr_setstacksize(&THREAD_LIRE_IMAGEParaAttr,33554432);
    pthread_attr_setinheritsched(&THREAD_LIRE_IMAGEParaAttr,PTHREAD_EXPLICIT_SCHED);
    pthread_attr_setschedpolicy(&THREAD_LIRE_IMAGEParaAttr,SCHED_RR);
    pthread_attr_setschedparam(&THREAD_LIRE_IMAGEParaAttr,&THREAD_LIRE_IMAGEPara);
    pthread_create(&THREAD_LIRE_IMAGE,&THREAD_LIRE_IMAGEParaAttr,&SaveCamera_IMU_DataToFifoFunc,NULL);

    pthread_t THREAD_SAUVEGARDER_INFO_GPS;
    struct sched_param THREAD_SAUVEGARDER_INFO_GPSPara;
    memset(&THREAD_SAUVEGARDER_INFO_GPSPara,0,sizeof(sched_param));
    THREAD_SAUVEGARDER_INFO_GPSPara.__sched_priority=sched_get_priority_max(SCHED_RR)-30;
    pthread_attr_t THREAD_SAUVEGARDER_INFO_GPSParaAttr;
    pthread_attr_init(&THREAD_SAUVEGARDER_INFO_GPSParaAttr);
    pthread_attr_setinheritsched(&THREAD_SAUVEGARDER_INFO_GPSParaAttr,PTHREAD_EXPLICIT_SCHED);
    pthread_attr_setschedpolicy(&THREAD_SAUVEGARDER_INFO_GPSParaAttr,SCHED_RR);
    pthread_attr_setschedparam(&THREAD_SAUVEGARDER_INFO_GPSParaAttr,&THREAD_SAUVEGARDER_INFO_GPSPara);
    pthread_create(&THREAD_SAUVEGARDER_INFO_GPS,&THREAD_SAUVEGARDER_INFO_GPSParaAttr,&SaveGPS_DataFunc,NULL);

    pthread_join(THREAD_RENOUVELER_BASE_HORODATAGE,NULL);
    pthread_join(THREAD_RENOUVELER_HORODATAGE,NULL);
    pthread_join(THREAD_RENOUVELER_GPS_FIFO,NULL);
    pthread_join(THREAD_LIRE_IMU_BRU,NULL);
    pthread_join(THREAD_SAUVEGARDER_BRUT_IMU,NULL);
    pthread_join(THREAD_SAUVEGARDER_CAMERA_IMU,NULL);
    pthread_join(THREAD_SAUVEGARDER_CAMERA_IMU1,NULL);
    pthread_join(THREAD_SAUVEGARDER_CAMERA_IMU2,NULL);
//    pthread_join(THREAD_SAUVEGARDER_CAMERA_IMU3,NULL);
    pthread_join(THREAD_LIRE_IMAGE,NULL);
    pthread_join(THREAD_SAUVEGARDER_INFO_GPS,NULL);


    std::cout<<"End of programme"<<std::endl;
}
