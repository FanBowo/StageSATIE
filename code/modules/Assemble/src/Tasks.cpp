
#include "Tasks.h"
Assemble AssembleDevice;

pthread_mutex_t GPS_DataFifoMutex=PTHREAD_MUTEX_INITIALIZER;
sem_t GPS_DataFifoSem;
pthread_mutex_t pSaveGPS_DataMutex=PTHREAD_MUTEX_INITIALIZER;

pthread_mutex_t Write2EmmcMutex=PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t Write2TerminalMutex=PTHREAD_MUTEX_INITIALIZER;

pthread_mutex_t bIMU_Data_StableMutex=PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t bIMU_Data_StableCond=PTHREAD_COND_INITIALIZER;

pthread_mutex_t bCSV_PointerPreparedMutex=PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t bCSV_PointerPreparedCond=PTHREAD_COND_INITIALIZER;

pthread_mutex_t Camera_IMU_DataFifoMutex=PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t IMU_RawDataFifoMutex=PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t pSaveRawIMU_DataMutex=PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t pSaveCamera_IMU_DataMutex=PTHREAD_MUTEX_INITIALIZER;

pthread_mutex_t ReadIMU_Mutex=PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t TimeStampBaseMutex =PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t TimeStampBaseCond=PTHREAD_COND_INITIALIZER;
pthread_mutex_t TimeStampBaseReNewMutex =PTHREAD_MUTEX_INITIALIZER;

pthread_cond_t IMU_RawDataCond=PTHREAD_COND_INITIALIZER;
pthread_mutex_t IMU_RawDataMutex =PTHREAD_MUTEX_INITIALIZER;
//pthread_mutex_t IMU_TimerCounterMutex=PTHREAD_MUTEX_INITIALIZER;
sem_t IMU_RawDataFifoSem;


sem_t Camera_IMUDataFifoSem;
//sem_t PhotoSem;
//sem_t PhotoPositionSem;
pthread_mutex_t OnlySaveCamera_IMU_DataPthreadMutex=PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t SaveCamera_IMU_DataMutex=PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t SaveCamera_IMU_DataCond=PTHREAD_COND_INITIALIZER;

FramePtr pNewFrame;
pthread_mutex_t pNewFrameMutex=PTHREAD_MUTEX_INITIALIZER;

//timer_t timerid_EXTERN_TRIGGER1;//Camera trigger pull up
//timer_t timerid_EXTERN_TRIGGER2;//Camera trigger push down
//int fd_GPIO_P2_c4;//GPIO file descriptor

pthread_mutex_t Device_TimerCounterMutex=PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t Device_TimeStampCond=PTHREAD_COND_INITIALIZER;
pthread_mutex_t Device_TimeStampMutex=PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t RW_Device_TimeStampMutex=PTHREAD_MUTEX_INITIALIZER;
timer_t Device_Timer;
long Device_TimerCounter=0;
struct itimerspec Device_Timer_trigger;


timer_t IMU_Timer;
struct itimerspec IMU_Timer_trigger;
//int IMU_TimerCounter=0;

//#define EnableConsoleDisplay

void * UpdateTimeStampBaseFunc(void *){
    std::cout<<"EnterThread_UpdateTimeStampBase"<<std::endl;
    while(1){
        pthread_mutex_lock(& TimeStampBaseMutex );

        pthread_cond_wait(&TimeStampBaseCond,&TimeStampBaseMutex);
            pthread_mutex_lock(& TimeStampBaseReNewMutex );

            if (( AssembleDevice.GPS.parse(AssembleDevice.GPS.lastNMEA()) && AssembleDevice.GPS.bRecvdRMCflag() )){
            // this also sets the newNMEAreceived() flag to false
                if(EnableParseOutput){
                    AssembleDevice.GPS.ResetRecvdRMCflag();
                    #ifdef EnableConsoleDisplay
                    pthread_mutex_lock(&(AssembleDevice.GPS.GpsTimeGettedMutex));
                        std::cout<<"Timestamp base:"<<AssembleDevice.GPS.GpsTimeGetted<<std::endl;
                    pthread_mutex_unlock(&(AssembleDevice.GPS.GpsTimeGettedMutex));
                    #endif // EnableConsoleDisplay

                    pthread_mutex_lock(& bIMU_Data_StableMutex );
                    bool bTempIMU_Data_Stable=AssembleDevice.bIMU_Data_Stable;
                    pthread_mutex_unlock(& bIMU_Data_StableMutex );

                    if(bTempIMU_Data_Stable){
                        GPS_data_t TempGPS_Data;
                        TempGPS_Data.fix=AssembleDevice.GPS.fix;
                        TempGPS_Data.TimeStamp=AssembleDevice.GPS.GpsTimeGetted;
                        TempGPS_Data.fixquality=AssembleDevice.GPS.fixquality;
                        if(TempGPS_Data.fix){
                            TempGPS_Data.latitude=AssembleDevice.GPS.latitude;
                            TempGPS_Data.lat=AssembleDevice.GPS.lat;
                            TempGPS_Data.longitude=AssembleDevice.GPS.longitude;
                            TempGPS_Data.lon=AssembleDevice.GPS.lon;
                            TempGPS_Data.speed=AssembleDevice.GPS.speed;
                            TempGPS_Data.angle=AssembleDevice.GPS.angle;
                            TempGPS_Data.satellites=AssembleDevice.GPS.satellites;
                        }
                        else{
                            TempGPS_Data.latitude=0.0;
                            TempGPS_Data.lat='0';
                            TempGPS_Data.longitude=0.0;
                            TempGPS_Data.lon='0';
                            TempGPS_Data.speed=0.0;
                            TempGPS_Data.angle=0.0;
                            TempGPS_Data.satellites=0;
                        }

                        pthread_mutex_lock(&GPS_DataFifoMutex);
                        AssembleDevice.GPS_DataFifo.push(TempGPS_Data);
                        sem_post(&GPS_DataFifoSem);
                        pthread_mutex_unlock(&GPS_DataFifoMutex);
                    }

                }
            }

//        pthread_mutex_lock(&IMU_TimerCounterMutex);
//        IMU_TimerCounter=0;
//        timer_settime(IMU_Timer,0,&IMU_Timer_trigger,NULL);
//        pthread_mutex_unlock(&IMU_TimerCounterMutex);


            pthread_mutex_lock(&Device_TimerCounterMutex);
                Device_TimerCounter=0;
                timer_settime(Device_Timer,0,&Device_Timer_trigger,NULL);
            pthread_mutex_unlock(&Device_TimerCounterMutex);


        pthread_mutex_unlock(& TimeStampBaseReNewMutex );


        pthread_mutex_unlock(& TimeStampBaseMutex );
    }
}

struct sigaction saio;
void init_SerialPortInt()
{

    int fd =AssembleDevice.GPSSerial.SerialPortFilefileDescriptor;
    saio.sa_handler = signal_handler_IO;
    saio.sa_flags = 0;
    saio.sa_restorer = NULL;
    sigaction(SIGIO,&saio,NULL);

    fcntl(fd, F_SETFL, FNDELAY|FASYNC);
    fcntl(fd, F_SETOWN, getpid());
    std::cout<<"Finishe initializing serial port"<<std::endl;
}

//long tempCounter=0;
void signal_handler_IO (int status)
{
    // read data from the GPS in the 'main loop'
//    std::cout<< "."<<std::endl;
    char c = AssembleDevice.GPS.read();
    //std::cout<< (char)c<<std::endl;
    // if you want to debug, this is a good time to do it!
    if (GPSECHO)
        if (c) std::cout<<c;
    // if a sentence is received, we can check the checksum, parse it...
    if (AssembleDevice.GPS.newNMEAreceived()) {
        // a tricky thing here is if we print the NMEA sentence, or data
        // we end up not listening and catching other sentences!
        // so be very wary if using OUTPUT_ALLDATA and trytng to print out data
    //std::cout<<GPS.lastNMEA()<<std::endl; // this also sets the newNMEAreceived() flag to false
    pthread_mutex_lock(&TimeStampBaseMutex);
    pthread_cond_signal(&TimeStampBaseCond);
    pthread_mutex_unlock(&TimeStampBaseMutex);
            //continue; // we can fail to parse a sentence in which case we should just wait for another
    //tempCounter++;
    //std::cout<<"TempCounter"<<tempCounter<<std::endl;
        AssembleDevice.GPS.ResetRecvdflag();
    }
    //printf("received data from UART.\n");
}


void InitTimerDevice(){
    struct sigevent SevTimerDevice;
    memset(&SevTimerDevice,0,sizeof(struct sigevent));
    memset (&Device_Timer_trigger,0,sizeof(struct itimerspec));

    SevTimerDevice.sigev_notify=SIGEV_THREAD;
    SevTimerDevice.sigev_notify_function=&TimerDevice_Feedback;

    timer_create(CLOCK_REALTIME, &SevTimerDevice,&Device_Timer);
    Device_Timer_trigger.it_interval.tv_sec=0;
    Device_Timer_trigger.it_interval.tv_nsec=(long)(Nano10_9/TimerDeviceFre);
    Device_Timer_trigger.it_value.tv_sec=0;
    Device_Timer_trigger.it_value.tv_nsec=1;
    Device_TimerCounter=0;
//    timer_settime(Device_Timer,0,&Device_Timer_trigger,NULL);

}

void TimerDevice_Feedback(union sigval sv){

    pthread_mutex_lock(&Device_TimeStampMutex);
    pthread_mutex_lock(&Device_TimerCounterMutex);

    pthread_cond_signal(&Device_TimeStampCond);
    Device_TimerCounter++;

    pthread_mutex_unlock(&Device_TimerCounterMutex);
    pthread_mutex_unlock(&Device_TimeStampMutex);
}

void * UpdateDeviceTimeStampFunc(void *){
    std::cout<<"EnterThread_UpdateDeviceTimeStamp"<<std::endl;
    while(1){

        pthread_mutex_lock(&Device_TimeStampMutex);
        pthread_cond_wait(&Device_TimeStampCond,&Device_TimeStampMutex);

        pthread_mutex_lock(& TimeStampBaseReNewMutex );
            pthread_mutex_lock(&Device_TimerCounterMutex);

                pthread_mutex_lock(&RW_Device_TimeStampMutex);
                    pthread_mutex_lock(&(AssembleDevice.GPS.GpsTimeGettedMutex));

                    AssembleDevice.DeviceTimeStamp=Device_TimerCounter*(1.0/(double)TimerDeviceFre)+ \
                                    AssembleDevice.GPS.GpsTimeGetted;
                    pthread_mutex_unlock(&(AssembleDevice.GPS.GpsTimeGettedMutex));

//        std::cout<<"Device_TimerCounter: "<<Device_TimerCounter<<std::endl;
                pthread_mutex_unlock(&RW_Device_TimeStampMutex);
//
//        std::cout<<"DeviceTimeStamp: "<<std::setiosflags(std::ios::fixed)\
//                        <<std::setprecision(4)<<AssembleDevice.DeviceTimeStamp<<std::endl;

            pthread_mutex_unlock(&Device_TimerCounterMutex);
        pthread_mutex_unlock(& TimeStampBaseReNewMutex );

        pthread_mutex_unlock(&Device_TimeStampMutex);
    }

}


void InitTimerIMU(){
    struct sigevent SevTimerIMU;
    memset(&SevTimerIMU,0,sizeof(struct sigevent));
    memset (&IMU_Timer_trigger,0,sizeof(struct itimerspec));

    SevTimerIMU.sigev_notify=SIGEV_THREAD;
    SevTimerIMU.sigev_notify_function=&TimerIMU_Feedback;

    timer_create(CLOCK_REALTIME, &SevTimerIMU,&IMU_Timer);
    IMU_Timer_trigger.it_interval.tv_sec=0;
    IMU_Timer_trigger.it_interval.tv_nsec=(long)(Nano10_9/TimerIMUFre);
    IMU_Timer_trigger.it_value.tv_sec=0;
    IMU_Timer_trigger.it_value.tv_nsec=1;
//    IMU_TimerCounter=0;
    timer_settime(IMU_Timer,0,&IMU_Timer_trigger,NULL);

}

void TimerIMU_Feedback(union sigval sv){

    pthread_mutex_lock(&IMU_RawDataMutex);
//    pthread_mutex_lock(&IMU_TimerCounterMutex);

    pthread_cond_signal(&IMU_RawDataCond);
//    IMU_TimerCounter++;

//    pthread_mutex_unlock(&IMU_TimerCounterMutex);
    pthread_mutex_unlock(&IMU_RawDataMutex);

}

void * IMU_UpdateRawDataFunc(void *){
    std::cout<<"EnterThread_IMU_UpdateRawData"<<std::endl;

    while(1){

        pthread_mutex_lock(&IMU_RawDataMutex);
        pthread_cond_wait(&IMU_RawDataCond,&IMU_RawDataMutex);

//        pthread_mutex_lock(& TimeStampBaseReNewMutex );
//        pthread_mutex_lock(&IMU_TimerCounterMutex);

//        AssembleDevice.IMU_TimeStamp=IMU_TimerCounter*(1.0/(float)TimerIMUFre)+ \
//                                AssembleDevice.GPS.GpsTimeGetted;
//        std::cout<<"TimeStamp: "<<AssembleDevice.IMU_TimeStamp<<std::endl;
        UpdateIMU_RawData();
//        pthread_mutex_unlock(&IMU_TimerCounterMutex);
//        pthread_mutex_unlock(& TimeStampBaseReNewMutex );

        pthread_mutex_unlock(&IMU_RawDataMutex);
    }

}

#define RADIAN2DEG (57.3f)
void OpenCSVfile(){

    pthread_mutex_lock(& bCSV_PointerPreparedMutex );

    pthread_mutex_lock(&RW_Device_TimeStampMutex);
    double timestamp=AssembleDevice.DeviceTimeStamp;
    pthread_mutex_unlock(&RW_Device_TimeStampMutex);

    std::string pIMU_CSV_FileNameTemp=std::to_string((long)(timestamp*Nano10_9))\
                        + "imu0.csv";
    const char *pIMU_CSV_FileName=pIMU_CSV_FileNameTemp.c_str();

    pthread_mutex_lock(&Write2EmmcMutex);

    AssembleDevice.pSaveRawIMU_Data.open(pIMU_CSV_FileName,std::ios::out|std::ios::trunc);
    AssembleDevice.pSaveRawIMU_Data<<"timestamp"<<","\
                    <<"omega_x"<<","<<"omega_y"<<","<<"omega_z"<<","\
                    <<"alpha_x"<<","<<"alpha_y"<<","<<"alpha_z"<<std::endl;

    pthread_mutex_unlock(&Write2EmmcMutex);

    std::string pGPS_CSV_FileNameTemp=std::to_string((unsigned long long int)(timestamp*Nano10_9))\
                        + "GPS.csv";
    const char *pGPS_CSV_FileName=pGPS_CSV_FileNameTemp.c_str();

    pthread_mutex_lock(&Write2EmmcMutex);

    AssembleDevice.pSaveGPS_Data.open(pGPS_CSV_FileName,std::ios::out|std::ios::trunc);
    AssembleDevice.pSaveGPS_Data<<"timestamp"<<","\
                    <<"fix"<<","<<"fixquality"<<","<<"latitude"<<","\
                    <<"lat"<<","<<"longitude"<<","<<"lon"<<","\
                    <<"speed"<<","<<"angle"<<","<<"satellites"\
                    <<std::endl;

    pthread_mutex_unlock(&Write2EmmcMutex);

        std::string pCamera_IMU_CSV_FileNameTemp=std::to_string((unsigned long long int)(timestamp*Nano10_9))\
                        + "Camera_IMU.csv";
    const char *pCamera_IMU_CSV_FileName=pCamera_IMU_CSV_FileNameTemp.c_str();

    pthread_mutex_lock(&Write2EmmcMutex);

    AssembleDevice.pSaveCamera_IMU_Data.open(pCamera_IMU_CSV_FileName,std::ios::out|std::ios::trunc);
    AssembleDevice.pSaveCamera_IMU_Data<<"timestamp"<<","<<"SystèmeCaliLevel"<<","\
                    <<"x"<<","<<"y"<<","<<"z"<<std::endl;

    pthread_mutex_unlock(&Write2EmmcMutex);

    AssembleDevice.bCSV_PointerPrepared=true;
    pthread_cond_broadcast(&bCSV_PointerPreparedCond);
    std::cout<<"broadcast bCSV_PointerPreparedCond signal"<<std::endl;
    pthread_mutex_unlock(& bCSV_PointerPreparedMutex );
}


void UpdateIMU_RawData(){
    // Possible vector values can be:
    // - VECTOR_ACCELEROMETER - m/s^2
    // - VECTOR_MAGNETOMETER  - uT
    // - VECTOR_GYROSCOPE     - rad/s
    // - VECTOR_EULER         - degrees
    // - VECTOR_LINEARACCEL   - m/s^2
    // - VECTOR_GRAVITY       - m/s^2
      /* Display calibration status for each sensor. */
    uint8_t system, gyro, accel, mag = 0;

    pthread_mutex_lock(& bIMU_Data_StableMutex );
    bool TempbIMU_Data_Stable=AssembleDevice.bIMU_Data_Stable;
    pthread_mutex_unlock(& bIMU_Data_StableMutex );

    if(!TempbIMU_Data_Stable){
        pthread_mutex_lock(&ReadIMU_Mutex);
        AssembleDevice.IMU_BNO055.getCalibration(&system, &gyro, &accel, &mag);
        pthread_mutex_unlock(&ReadIMU_Mutex);


        std::cout<<"System "<<(int)system<<"gyro "<<(int)gyro << \
                "accel "<<(int)accel<<"mag "<<(int)mag<<std::endl;
        if((int)system==3){

            pthread_mutex_lock(& bIMU_Data_StableMutex );
            AssembleDevice.bIMU_Data_Stable=true;
            pthread_cond_broadcast(&bIMU_Data_StableCond);
            std::cout<<"broadcast bIMU_Data_StableCond signal"<<std::endl;
            pthread_mutex_unlock(& bIMU_Data_StableMutex );
        }
    }
    else{
        sensors_event_t event;

        pthread_mutex_lock(&ReadIMU_Mutex);
        AssembleDevice.IMU_BNO055.getEvent(& event,Adafruit_BNO055::VECTOR_ACCELEROMETER);
        pthread_mutex_unlock(&ReadIMU_Mutex);

        IMU_RawData_t TempIMU_RawData;

        TempIMU_RawData.acceleration.x=(float)event.acceleration.x;
        TempIMU_RawData.acceleration.y=(float)event.acceleration.y;
        TempIMU_RawData.acceleration.z=0.0f-(float)event.acceleration.z;
    //    std::cout<<"acc :"<<(float)event.acceleration.x<<\
    //                           " "<<(float)event.acceleration.y<<\
    //                           " "<<(float)event.acceleration.z<<std::endl;

        pthread_mutex_lock(&ReadIMU_Mutex);
        AssembleDevice.IMU_BNO055.getEvent(& event,Adafruit_BNO055::VECTOR_GYROSCOPE);
        pthread_mutex_unlock(&ReadIMU_Mutex);

        TempIMU_RawData.gyro.x=(float)event.gyro.x/RADIAN2DEG;
        TempIMU_RawData.gyro.y=(float)event.gyro.y/RADIAN2DEG;
        TempIMU_RawData.gyro.z=0.0f-(float)event.gyro.z/RADIAN2DEG;
    //    std::cout<<"omega :"<<(float)event.gyro.x<<\
    //                       " "<<(float)event.gyro.y<<\
    //                       " "<<(float)event.gyro.z<<std::endl;
        pthread_mutex_lock(&RW_Device_TimeStampMutex);
        TempIMU_RawData.timestamp=AssembleDevice.DeviceTimeStamp;
        pthread_mutex_unlock(&RW_Device_TimeStampMutex);

        pthread_mutex_lock(&IMU_RawDataFifoMutex);
        AssembleDevice.IMU_RawDataFifo.push(TempIMU_RawData);
        sem_post(&IMU_RawDataFifoSem);
        pthread_mutex_unlock(&IMU_RawDataFifoMutex);


        pthread_mutex_lock(& bCSV_PointerPreparedMutex );
        bool TempbCSV_PointerPreparedMutex=AssembleDevice.bCSV_PointerPrepared;
        pthread_mutex_unlock(& bCSV_PointerPreparedMutex );

        if(!TempbCSV_PointerPreparedMutex){
            OpenCSVfile();
        }
    }



//    std::cout<<TempIMU_RawData.acceleration.x<<" "\
//            <<TempIMU_RawData.acceleration.y<<" "\
//            <<TempIMU_RawData.acceleration.z<<" "\
//            <<TempIMU_RawData.gyro.x<<" "\
//            <<TempIMU_RawData.gyro.y<<" "\
//            <<TempIMU_RawData.gyro.z<<" "<<std::endl;




  #ifdef ManuCaliMagn
    if(system>=1){
        imu::Vector<3> magn = bno.getVector(Adafruit_BNO055::VECTOR_MAGNETOMETER);
        Magnedata<<magn.x()<<" "<<magn.y()<<" "<<magn.z()<<"\n";
        Oriendata<<(float)event.orientation.x<<\
                       " "<<(float)event.orientation.y<<\
                       " "<<(float)event.orientation.z<<"\n";
    }
    #endif // ManuCaliMagn
#ifdef UsingProfileConfig
  if((system>=2&&gyro>=3&&mag>=3&& !(bno.bInitWithCaliProfileCompleted))){
    bno.InitWithCaliProfile();
    bno.LastNorthPoleCount=0;
  }
  if(system<=0&&(bno.bInitWithCaliProfileCompleted)){
    bno.LastNorthPoleCount++;
    if(bno.LastNorthPoleCount>200){
        bno.bInitWithCaliProfileCompleted=false;//Need re-calibration
    }
  }
#endif // UsingProfileConfig
}

void * SaveIMU_RawDataFunc(void *){
    std::cout<<"EnterThread_SaveIMU_RawData"<<std::endl;

    pthread_mutex_lock(& bCSV_PointerPreparedMutex );
    pthread_cond_wait(&bCSV_PointerPreparedCond,&bCSV_PointerPreparedMutex);
    std::cout<<"Wait for bCSV_PointerPreparedCond signal"<<std::endl;
    pthread_mutex_unlock(& bCSV_PointerPreparedMutex );

    std::cout<<"Get bCSV_PointerPreparedCond signal"<<std::endl;

    while(1){
        sem_wait(&IMU_RawDataFifoSem);

        pthread_mutex_lock(&IMU_RawDataFifoMutex);
            if(AssembleDevice.IMU_RawDataFifo.empty()){
                pthread_mutex_unlock(&IMU_RawDataFifoMutex);
                continue;
            }
            IMU_RawData_t TempIMU_RawData=AssembleDevice.IMU_RawDataFifo.front();
            AssembleDevice.IMU_RawDataFifo.pop();
        pthread_mutex_unlock(&IMU_RawDataFifoMutex);

        pthread_mutex_lock(&pSaveRawIMU_DataMutex);
        pthread_mutex_lock(&Write2EmmcMutex);

        AssembleDevice.pSaveRawIMU_Data
                        <<std::to_string((unsigned long long int )(TempIMU_RawData.timestamp*Nano10_9))<<"," \
                        <<std::setiosflags(std::ios::fixed)\
                        <<std::setprecision(4)\
                        << TempIMU_RawData.gyro.x<<","\
                        << TempIMU_RawData.gyro.y<<","\
                        << TempIMU_RawData.gyro.z<<","\
                        << TempIMU_RawData.acceleration.x<<","\
                        << TempIMU_RawData.acceleration.y<<","\
                        << TempIMU_RawData.acceleration.z<<std::endl;

        pthread_mutex_unlock(&Write2EmmcMutex);
        pthread_mutex_unlock(&pSaveRawIMU_DataMutex);
    }
}

/*GPS*/

void * SaveGPS_DataFunc(void *){
    std::cout<<"SaveGPS_DataFunc"<<std::endl;

    pthread_mutex_lock(& bCSV_PointerPreparedMutex );
    pthread_cond_wait(&bCSV_PointerPreparedCond,&bCSV_PointerPreparedMutex);
    std::cout<<"Wait for bCSV_PointerPreparedCond signal"<<std::endl;
    pthread_mutex_unlock(& bCSV_PointerPreparedMutex );

    std::cout<<"Get bCSV_PointerPreparedCond signal"<<std::endl;

    while(1){
        sem_wait(&GPS_DataFifoSem);

        pthread_mutex_lock(&GPS_DataFifoMutex);
            if(AssembleDevice.GPS_DataFifo.empty()){
                pthread_mutex_unlock(&GPS_DataFifoMutex);
                continue;
            }
            GPS_data_t TempGPS_data=AssembleDevice.GPS_DataFifo.front();
            AssembleDevice.GPS_DataFifo.pop();
        pthread_mutex_unlock(&GPS_DataFifoMutex);

        pthread_mutex_lock(&pSaveGPS_DataMutex);
        pthread_mutex_lock(&Write2EmmcMutex);

        AssembleDevice.pSaveGPS_Data
                        <<std::to_string((unsigned long long int )(TempGPS_data.TimeStamp*Nano10_9))<<"," \
                        << (int)TempGPS_data.fix<<","\
                        << TempGPS_data.fixquality<<","\
                        << TempGPS_data.latitude<<","\
                        << TempGPS_data.lat<<","\
                        << TempGPS_data.longitude<<","\
                        << TempGPS_data.lon<<","\
                        << TempGPS_data.speed<<","\
                        << TempGPS_data.angle<<","\
                        << TempGPS_data.satellites<<","\
                        <<std::endl;

        pthread_mutex_unlock(&Write2EmmcMutex);
        pthread_mutex_unlock(&pSaveGPS_DataMutex);
    }
}

/*Camera*/

/*A new image received save into fifo*/
void CreatAndSaveImag(const FramePtr pFrame ){
    std::cout<<"Received one new frame"<<std::endl;
    pthread_mutex_lock(&SaveCamera_IMU_DataMutex);

    pthread_mutex_lock(&pNewFrameMutex);

    pNewFrame=pFrame;

    pthread_mutex_unlock(&pNewFrameMutex);

    pthread_cond_signal(&SaveCamera_IMU_DataCond);
    pthread_mutex_unlock(&SaveCamera_IMU_DataMutex);
}

#define UseDefaultPhotoFormat
void *SaveCamera_IMU_DataToFifoFunc(void *){
    std::cout<<"Enter SaveCamera_IMU_DataToFifo thread "<<std::endl;

    pthread_mutex_lock(& bIMU_Data_StableMutex );
    pthread_cond_wait(&bIMU_Data_StableCond,&bIMU_Data_StableMutex);
    std::cout<<"wait bIMU_Data_StableCond Signal"<<std::endl;
    pthread_mutex_unlock(& bIMU_Data_StableMutex );
    std::cout<<"Reiceived IMU_Data_Stable condition signal and begin taking photo"<<std::endl;
    AssembleDevice.TheCamera.InitCameraTriggrtTimer();
#ifdef UseDefaultPhotoFormat
    AssembleDevice.PhotoFormatInfo.nImageSize=Default_Size;
    AssembleDevice.PhotoFormatInfo.nWidth=Default_Width;
    AssembleDevice.PhotoFormatInfo.nWidth=Default_Height;
    AssembleDevice.PhotoFormatInfo.bFormatGetted=true;
#endif // UseDefaultPhotoFormat
    while(1){
        pthread_mutex_lock(&SaveCamera_IMU_DataMutex);
        pthread_cond_wait(&SaveCamera_IMU_DataCond,&SaveCamera_IMU_DataMutex);

        //    std::cout<<"Received one new frame"<<std::endl;
        /*use PhotoFormatInfo to save frame format*/
        Camera_IMU_Data_t TempCamera_IMU_Data;

        pthread_mutex_lock(&RW_Device_TimeStampMutex);
        TempCamera_IMU_Data.timestamp=AssembleDevice.DeviceTimeStamp;
        pthread_mutex_unlock(&RW_Device_TimeStampMutex);

        sensors_event_t event;

        pthread_mutex_lock(&ReadIMU_Mutex);
    //    std::cout<<"CreatAndSaveImag event get ReadIMU_Mutex"<<std::endl;
        AssembleDevice.IMU_BNO055.getEvent(& event);
        pthread_mutex_unlock(&ReadIMU_Mutex);

        event.orientation.x=event.orientation.x+90.0f;
        if(event.orientation.x>360.0f){
            event.orientation.x=event.orientation.x-360.0f;
        }
        TempCamera_IMU_Data.CameraPose.orientation.x=(float)event.orientation.x;
        TempCamera_IMU_Data.CameraPose.orientation.y=(float)event.orientation.y;
        TempCamera_IMU_Data.CameraPose.orientation.z=(float)event.orientation.z;

        uint8_t Temp_Sys_cali_level,Temp_Gyro_cali_level,Temp_Acc_cali_level,Temp_Magn_cali_level=0;

        pthread_mutex_lock(&ReadIMU_Mutex);
    //    std::cout<<"CreatAndSaveImag event get ReadIMU_Mutex"<<std::endl;
        AssembleDevice.IMU_BNO055.getCalibration(&Temp_Sys_cali_level,
                                                    &Temp_Gyro_cali_level,
                                                    &Temp_Acc_cali_level,
                                                    &Temp_Magn_cali_level
                                                    );
        pthread_mutex_unlock(&ReadIMU_Mutex);

        TempCamera_IMU_Data.Sys_cali_level=Temp_Sys_cali_level;

        VmbErrorType    err         = VmbErrorSuccess;
        //VmbPixelFormatType ePixelFormat = VmbPixelFormatMono8;


        pthread_mutex_lock(&pNewFrameMutex);
        FramePtr TempPtr=pNewFrame;
        pthread_mutex_unlock(&pNewFrameMutex);
#ifndef UseDefaultPhotoFormat
        if(! AssembleDevice.PhotoFormatInfo.bFormatGetted){
            VmbUint32_t nImageSize = 0;
            err = TempPtr->GetImageSize( nImageSize );
            std::cout<<"nImageSize "<<nImageSize<<std::endl;
            AssembleDevice.PhotoFormatInfo.nImageSize=nImageSize;
            if ( VmbErrorSuccess == err )
            {
                VmbUint32_t nWidth = 0;
                err = TempPtr->GetWidth( nWidth );
                 AssembleDevice.PhotoFormatInfo.nWidth=nWidth;
                 std::cout<<"nWidth "<<nWidth<<std::endl;
                if ( VmbErrorSuccess == err )
                {
                    VmbUint32_t nHeight = 0;
                    err = TempPtr->GetHeight( nHeight );
                    std::cout<<"nHeight "<<nHeight<<std::endl;
                     AssembleDevice.PhotoFormatInfo.nHeight=nHeight;
                    if ( VmbErrorSuccess == err )
                    {    AssembleDevice.PhotoFormatInfo.bFormatGetted=true;
                        VmbUchar_t ImageBuffer[nImageSize];
                        VmbUchar_t *pImage = ImageBuffer;
                        err = TempPtr->GetImage( pImage );

                        if ( VmbErrorSuccess == err )
                        {
                            memcpy(TempCamera_IMU_Data.pImage,pImage,nImageSize);
//                            *(TempCamera_IMU_Data.pImage)=*(pImage);
                        }

                    }
                }
            }
        }
        else{
            VmbUint32_t nImageSize=AssembleDevice.PhotoFormatInfo.nImageSize;
            VmbUchar_t ImageBuffer[nImageSize];
            VmbUchar_t *pImage = ImageBuffer;
            err = TempPtr->GetImage( pImage );

            if ( VmbErrorSuccess == err )
            {
                memcpy(TempCamera_IMU_Data.pImage,pImage,nImageSize);
            }
        }
#endif // UseDefaultPhotoFormat
#ifdef UseDefaultPhotoFormat
        VmbUchar_t ImageBuffer[Default_Size];
        VmbUchar_t *pImage = ImageBuffer;
        err = TempPtr->GetImage( pImage );

        if ( VmbErrorSuccess == err ){
            memcpy(TempCamera_IMU_Data.pImage,pImage,Default_Size);
        }
#endif // UseDefaultPhotoFormat
        pthread_mutex_lock(&Camera_IMU_DataFifoMutex);
        AssembleDevice.Camera_IMU_DataFifo.push(TempCamera_IMU_Data);
        sem_post(&Camera_IMUDataFifoSem);
        pthread_mutex_unlock(&Camera_IMU_DataFifoMutex);

        pthread_mutex_unlock(&SaveCamera_IMU_DataMutex);


    }
}

#define SaveImagAsJPEG
#ifdef SaveImagAsJPEG

#define THROW(action, message) { \
  printf("ERROR in line %d while %s:\n%s\n", __LINE__, action, message); \
  goto bailout; \
}

#define THROW_TJ(action)  THROW(action, tjGetErrorStr2(tjInstance))

#define THROW_UNIX(action)  THROW(action, strerror(errno))

#define DEFAULT_QUALITY  95

#endif // SaveImagAsJPEG

void * SaveCamera_IMU_DataFunc(void *){
    std::cout<<"EnterThread_SaveCamera_IMU_Data"<<std::endl;

    pthread_mutex_lock(& bCSV_PointerPreparedMutex );
    pthread_cond_wait(&bCSV_PointerPreparedCond,&bCSV_PointerPreparedMutex);


    std::cout<<"Wait for bCSV_PointerPreparedCond signal"<<std::endl;


    pthread_mutex_unlock(& bCSV_PointerPreparedMutex );

    std::cout<<"Get bCSV_PointerPreparedCond signal"<<std::endl;

    while(1){


        sem_wait(&Camera_IMUDataFifoSem);

        pthread_mutex_lock(&Camera_IMU_DataFifoMutex);
            if( AssembleDevice.Camera_IMU_DataFifo.empty()){
                pthread_mutex_unlock(&Camera_IMU_DataFifoMutex);
                continue;
            }
            Camera_IMU_Data_t TempCamera_IMU_Data=AssembleDevice.Camera_IMU_DataFifo.front();
            AssembleDevice.Camera_IMU_DataFifo.pop();
        pthread_mutex_unlock(&Camera_IMU_DataFifoMutex);

        pthread_mutex_lock(&pSaveCamera_IMU_DataMutex);
        pthread_mutex_lock(&Write2EmmcMutex);

        AssembleDevice.pSaveCamera_IMU_Data
                        <<std::to_string((unsigned long long int)(TempCamera_IMU_Data.timestamp*Nano10_9))<<"," \
                        << (int) TempCamera_IMU_Data.Sys_cali_level<<","\
                        <<std::setiosflags(std::ios::fixed)\
                        <<std::setprecision(4)\
                        << TempCamera_IMU_Data.CameraPose.orientation.x<<","\
                        << TempCamera_IMU_Data.CameraPose.orientation.y<<","\
                        << TempCamera_IMU_Data.CameraPose.orientation.z<<std::endl;

        pthread_mutex_unlock(&Write2EmmcMutex);
        pthread_mutex_unlock(&pSaveCamera_IMU_DataMutex);




#ifdef SaveImagAsJPEG
//        pthread_mutex_lock(&OnlySaveCamera_IMU_DataPthreadMutex);

        std::string pFileNametemp="./cam0/"+std::to_string((unsigned long long int)(TempCamera_IMU_Data.timestamp*Nano10_9))\
                        + ".jpg";
        const char *pFileName=pFileNametemp.c_str();

        FILE *jpegFile = NULL;
        unsigned char *jpegBuf = NULL;
        tjhandle tjInstance = NULL;
        int outQual = DEFAULT_QUALITY;
        int pixelFormat=TJPF_GRAY;
        unsigned long jpegSize=0;
        int outSubsamp=TJSAMP_GRAY;
        int flags=0;
//        int retval=0;

        if ((tjInstance = tjInitCompress()) == NULL)
          THROW_TJ("initializing compressor");
        #ifdef UseDefaultPhotoFormat
            if (tjCompress2(tjInstance, TempCamera_IMU_Data.pImage, Default_Width, \
            0, Default_Height, pixelFormat,
            &jpegBuf, &jpegSize, outSubsamp, outQual, flags) < 0)
          THROW_TJ("compressing image");
        #endif // UseDefaultPhotoFormat
        #ifndef UseDefaultPhotoFormat
            if (tjCompress2(tjInstance, TempCamera_IMU_Data.pImage, AssembleDevice.PhotoFormatInfo.nWidth, \
                0, AssembleDevice.PhotoFormatInfo.nHeight, pixelFormat,
                &jpegBuf, &jpegSize, outSubsamp, outQual, flags) < 0)
              THROW_TJ("compressing image");
        #endif // UseDefaultPhotoFormat

        tjDestroy(tjInstance);
        tjInstance = NULL;


        /* Write the JPEG image to disk. */
        pthread_mutex_lock(&Write2EmmcMutex);

        if ((jpegFile = fopen(pFileName, "wb")) == NULL)
          THROW_UNIX("opening output file");
        if (fwrite(jpegBuf, jpegSize, 1, jpegFile) < 1)
          THROW_UNIX("writing output file");
        #ifdef EnableConsoleDisplay
        std::cout<<"write one jpg"<<std::endl;
        #endif // EnableConsoleDisplay
        pthread_mutex_unlock(&Write2EmmcMutex);

        tjDestroy(tjInstance);
        tjInstance = NULL;
        fclose(jpegFile);
        jpegFile = NULL;
        tjFree(jpegBuf);
        jpegBuf = NULL;

        bailout:
//          if (pImage) tjFree(pImage);
          if (tjInstance) tjDestroy(tjInstance);
          if (jpegBuf) tjFree(jpegBuf);
          if (jpegFile) fclose(jpegFile);
//        pthread_mutex_unlock(&OnlySaveCamera_IMU_DataPthreadMutex);
#endif // SaveImagAsJPEG
#ifndef SaveImagAsJPEG

//    pthread_mutex_lock(&OnlySaveCamera_IMU_DataPthreadMutex);

        AVTBitmap bitmap;

        std::string pFileNametemp="./cam0/"+std::to_string((long)(TempCamera_IMU_Data.timestamp*Nano10_9))\
                        + ".bmp";
        const char *pFileName=pFileNametemp.c_str();

        bitmap.colorCode = ColorCodeMono8;
        #ifndef UseDefaultPhotoFormat
            bitmap.bufferSize =  AssembleDevice.PhotoFormatInfo.nImageSize;
            bitmap.width =  AssembleDevice.PhotoFormatInfo.nWidth;
            bitmap.height =  AssembleDevice.PhotoFormatInfo.nHeight;
        #endif // UseDefaultPhotoFormat
        #ifdef UseDefaultPhotoFormat
            bitmap.bufferSize=Default_Size;
            bitmap.width=Default_Width;
            bitmap.height=Default_Height;
        #endif // UseDefaultPhotoFormat
        VmbError_t err =VmbErrorSuccess;


        if ( 0 == AVTCreateBitmap( &bitmap, TempCamera_IMU_Data.pImage )){
            std::cout << "Could not create bitmap.\n";
            err = VmbErrorResources;
        }
        else{
            pthread_mutex_lock(&Write2EmmcMutex);
            // Save the bitmap
            if ( 0 == AVTWriteBitmapToFile( &bitmap, pFileName )){
                std::cout << "Could not write bitmap to file.\n";
                err = VmbErrorOther;
                //                // Release the bitmap's buffer
                if ( 0 == AVTReleaseBitmap( &bitmap )){
                    std::cout << "Could not release the bitmap.\n";
                    err = VmbErrorInternalFault;
                }
                pthread_mutex_unlock(&Write2EmmcMutex);
            }
            else{
                pthread_mutex_unlock(&Write2EmmcMutex);
                #ifdef EnableConsoleDisplay
                    std::cout << "Bitmap successfully written to file \"" << pFileName << "\"\n" ;
                #endif // EnableConsoleDisplay

                // Release the bitmap's buffer
                if ( 0 == AVTReleaseBitmap( &bitmap )){
                    std::cout << "Could not release the bitmap.\n";
                    err = VmbErrorInternalFault;
                }
            }
        }
//        pthread_mutex_unlock(&OnlySaveCamera_IMU_DataPthreadMutex);
#endif // SaveImagAsJPEG

    }
}

void CheckAcquisitionStatus(){
    std::cout<<"check acquisition status..."<<std::endl;
    SetAcquisitionStatusSelector(AssembleDevice.TheCamera.camera,"AcquisitionActive");
    bool AcquisitionStatus;
    FeaturePtr pFeature;
    if(VmbErrorSuccess==AssembleDevice.TheCamera.camera->GetFeatureByName ("AcquisitionStatus", pFeature )){
        if(VmbErrorSuccess==pFeature -> GetValue (AcquisitionStatus)){
            if(AcquisitionStatus){
                std::cout<<"AcquisitionActive"<<std::endl;
            }
        }
        else{
            std::cout<<"Can't get feature AcquisitionStatus"<<std::endl;
        }
    }
    else{
        std::cout<<"Can't get feature AcquisitionStatus"<<std::endl;
    }
}
