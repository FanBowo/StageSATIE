
#include "Tasks.h"
Assemble AssembleDevice;

pthread_mutex_t TimeStampBaseMutex =PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t TimeStampBaseCond=PTHREAD_COND_INITIALIZER;
pthread_mutex_t TimeStampBaseReNewMutex =PTHREAD_MUTEX_INITIALIZER;

pthread_cond_t IMU_TimeStampCond=PTHREAD_COND_INITIALIZER;
pthread_mutex_t IMU_TimeStampMutex =PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t IMU_TimerCounterMutex=PTHREAD_MUTEX_INITIALIZER;
sem_t IMU_RawDataFifoSem;

timer_t IMU_Timer;
struct itimerspec IMU_Timer_trigger;
int IMU_TimerCounter=0;

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
                std::cout<<"Timestamp base:"<<AssembleDevice.GPS.GpsTimeGetted<<std::endl;
//                // if millis() or timer wraps around, we'll just reset it
//                std::cout<<"\nTime: "<<std::endl;
//                std::cout<<(int)AssembleDevice.GPS.hour<<":"<<(int)AssembleDevice.GPS.minute<<":"<<(int)AssembleDevice.GPS.seconds<<":"<<AssembleDevice.GPS.milliseconds<<std::endl;
//                std::cout<<"Date: "<<std::endl;
//                std::cout<<(int)AssembleDevice.GPS.day<<":"<<(int)AssembleDevice.GPS.month<<":"<<(int)AssembleDevice.GPS.year<<std::endl;
//                std::cout<<"Fix: "<<std::endl;
//                std::cout<<(int)AssembleDevice.GPS.fix<<std::endl;
//                std::cout<<"quality: "<<std::endl;
//                std::cout<<(int)AssembleDevice.GPS.fixquality<<std::endl;
//                if (AssembleDevice.GPS.fix) {
//                    std::cout<<"Location: "<<std::endl;
//                    std::cout<<AssembleDevice.GPS.latitude<<","<<AssembleDevice.GPS.lat<<","<<AssembleDevice.GPS.longitude<<","<<AssembleDevice.GPS.lon<<std::endl;
//                    std::cout<<"Speed (knots): "<<std::endl;
//                    std::cout<<AssembleDevice.GPS.speed<<std::endl;
//                    std::cout<<"Angle: "<<std::endl;
//                    std::cout<<AssembleDevice.GPS.angle<<std::endl;
//                    std::cout<<"Altitude: "<<std::endl;
//                    std::cout<<AssembleDevice.GPS.altitude<<std::endl;
//                    std::cout<<"Satellites:: "<<std::endl;
//                    std::cout<<AssembleDevice.GPS.satellites<<std::endl;
//                }
                //delay(2000);
            }
        }

        pthread_mutex_lock(&IMU_TimerCounterMutex);
        IMU_TimerCounter=0;
        timer_settime(IMU_Timer,0,&IMU_Timer_trigger,NULL);
        pthread_mutex_unlock(&IMU_TimerCounterMutex);
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


void InitTimerIMU(){
    struct sigevent SevTimerIMU;
    memset(&SevTimerIMU,0,sizeof(struct sigevent));
    memset (&IMU_Timer_trigger,0,sizeof(struct itimerspec));

    SevTimerIMU.sigev_notify=SIGEV_THREAD;
    SevTimerIMU.sigev_notify_function=&TimerIMU_Feedback;

    timer_create(CLOCK_REALTIME, &SevTimerIMU,&IMU_Timer);
    IMU_Timer_trigger.it_interval.tv_sec=0;
    IMU_Timer_trigger.it_interval.tv_nsec=Nano10_9/TimerIMUFre;
    IMU_Timer_trigger.it_value.tv_sec=0;
    IMU_Timer_trigger.it_value.tv_nsec=1;
    IMU_TimerCounter=0;
//    timer_settime(IMU_Timer,0,&IMU_Timer_trigger,NULL);

}

void TimerIMU_Feedback(union sigval sv){

    pthread_mutex_lock(&IMU_TimeStampMutex);
    pthread_mutex_lock(&IMU_TimerCounterMutex);

    pthread_cond_signal(&IMU_TimeStampCond);
    IMU_TimerCounter++;

    pthread_mutex_unlock(&IMU_TimerCounterMutex);
    pthread_mutex_unlock(&IMU_TimeStampMutex);


}

void * IMU_UpdateTimeStampFunc(void *){
    std::cout<<"EnterThread_IMU_UpdateTimeStamp"<<std::endl;
    while(1){

        pthread_mutex_lock(&IMU_TimeStampMutex);
        pthread_cond_wait(&IMU_TimeStampCond,&IMU_TimeStampMutex);

        pthread_mutex_lock(& TimeStampBaseReNewMutex );
        pthread_mutex_lock(&IMU_TimerCounterMutex);

        AssembleDevice.IMU_TimeStamp=IMU_TimerCounter*(1.0/(float)TimerIMUFre)+ \
                                AssembleDevice.GPS.GpsTimeGetted;
        std::cout<<"TimeStamp: "<<AssembleDevice.IMU_TimeStamp<<std::endl;
        UpdateIMU_RawData();
        pthread_mutex_unlock(&IMU_TimerCounterMutex);
        pthread_mutex_unlock(& TimeStampBaseReNewMutex );

        pthread_mutex_unlock(&IMU_TimeStampMutex);
    }

}


void UpdateIMU_RawData(){
    // Possible vector values can be:
    // - VECTOR_ACCELEROMETER - m/s^2
    // - VECTOR_MAGNETOMETER  - uT
    // - VECTOR_GYROSCOPE     - rad/s
    // - VECTOR_EULER         - degrees
    // - VECTOR_LINEARACCEL   - m/s^2
    // - VECTOR_GRAVITY       - m/s^2
    sensors_event_t event;
    AssembleDevice.IMU_BNO055.getEvent(& event,Adafruit_BNO055::VECTOR_LINEARACCEL);

    IMU_RawData_t TempIMU_RawData;

    TempIMU_RawData.acceleration.x=(float)event.acceleration.x;
    TempIMU_RawData.acceleration.y=(float)event.acceleration.y;
    TempIMU_RawData.acceleration.z=(float)event.acceleration.z;
//    std::cout<<"acc :"<<(float)event.acceleration.x<<\
//                           " "<<(float)event.acceleration.y<<\
//                           " "<<(float)event.acceleration.z<<std::endl;


    AssembleDevice.IMU_BNO055.getEvent(& event,Adafruit_BNO055::VECTOR_GYROSCOPE);
    TempIMU_RawData.gyro.x=(float)event.gyro.x;
    TempIMU_RawData.gyro.y=(float)event.gyro.y;
    TempIMU_RawData.gyro.z=(float)event.gyro.z;
//    std::cout<<"omega :"<<(float)event.gyro.x<<\
//                       " "<<(float)event.gyro.y<<\
//                       " "<<(float)event.gyro.z<<std::endl;

    TempIMU_RawData.timestamp=AssembleDevice.IMU_TimeStamp;


    AssembleDevice.IMU_RawDataFifo.push(TempIMU_RawData);

    sem_post(&IMU_RawDataFifoSem);

//    std::cout<<TempIMU_RawData.acceleration.x<<" "\
//            <<TempIMU_RawData.acceleration.y<<" "\
//            <<TempIMU_RawData.acceleration.z<<" "\
//            <<TempIMU_RawData.gyro.x<<" "\
//            <<TempIMU_RawData.gyro.y<<" "\
//            <<TempIMU_RawData.gyro.z<<" "<<std::endl;

  /* Display calibration status for each sensor. */
  uint8_t system, gyro, accel, mag = 0;
  AssembleDevice.IMU_BNO055.getCalibration(&system, &gyro, &accel, &mag);
  std::cout<<"System "<<(int)system<<"gyro "<<(int)gyro << \
            "accel "<<(int)accel<<"mag "<<(int)mag<<std::endl;

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
    while(1){
        sem_wait(&IMU_RawDataFifoSem);
        IMU_RawData_t TempIMU_RawData=AssembleDevice.IMU_RawDataFifo.front();
        AssembleDevice.pSaveRawIMU_Data
                        <<(long)(TempIMU_RawData.timestamp*Nano10_9)<<"," \
                        <<std::setiosflags(std::ios::fixed)\
                        <<std::setprecision(4)\
                        << TempIMU_RawData.gyro.x<<","\
                        << TempIMU_RawData.gyro.y<<","\
                        << TempIMU_RawData.gyro.z<<","\
                        << TempIMU_RawData.acceleration.x<<","\
                        << TempIMU_RawData.acceleration.y<<","\
                        << TempIMU_RawData.acceleration.z<<std::endl;
        AssembleDevice.IMU_RawDataFifo.pop();

    }
}

/*Camera*/

CameraMako130 TheCamera;

timer_t timerid_EXTERN_TRIGGER1;//pull up
timer_t timerid_EXTERN_TRIGGER2;//push down
int fd_GPIO_P2_c4;

void InitCameraTimer(){
    printf("Start initialize timer\n");
    struct sigevent sev1;//pull up
    struct itimerspec trigger1;//pull up
    memset(&sev1, 0, sizeof(struct sigevent));//pull up
    memset(&trigger1, 0, sizeof(struct itimerspec));//pull up

    sev1.sigev_notify=SIGEV_THREAD;//pull up
    sev1.sigev_notify_function=&TriggerPWM_pullup;//pull up

    timer_create(CLOCK_REALTIME,&sev1,&timerid_EXTERN_TRIGGER1);//pull up

    trigger1.it_interval.tv_sec=0;//pull up
    trigger1.it_interval.tv_nsec=Nano10_9/ExternTriggerFre;//pull up
    //trigger.it_interval.tv_nsec=0;
    trigger1.it_value.tv_sec=0;//pull up
    trigger1.it_value.tv_nsec=1;//pull up

    struct sigevent sev2;//push down
    struct itimerspec trigger2;//push down
    memset(&sev2, 0, sizeof(struct sigevent));//push down
    memset(&trigger2, 0, sizeof(struct itimerspec));//push down

    sev2.sigev_notify=SIGEV_THREAD;//push down
    sev2.sigev_notify_function=&TriggerPWM_pushdown;//push down

    timer_create(CLOCK_REALTIME,&sev2,&timerid_EXTERN_TRIGGER2);//push down

    trigger2.it_interval.tv_sec=0;//push down
    trigger2.it_interval.tv_nsec=Nano10_9/ExternTriggerFre;//push down
    //trigger.it_interval.tv_nsec=0;
    trigger2.it_value.tv_sec=0;//push down
    trigger2.it_value.tv_nsec=1+ExposureTime;//push down

    timer_settime(timerid_EXTERN_TRIGGER1,0,&trigger1,NULL);
    timer_settime(timerid_EXTERN_TRIGGER2,0,&trigger2,NULL);

    printf("Successfully initialize timer\n");
}


void TriggerPWM_pullup(union sigval sv){
    write(fd_GPIO_P2_c4, SYSFS_GPIO_RST_VAL_H, sizeof(SYSFS_GPIO_RST_VAL_H));
//    printf("H\n");
}

void TriggerPWM_pushdown(union sigval sv){
    write(fd_GPIO_P2_c4, SYSFS_GPIO_RST_VAL_L, sizeof(SYSFS_GPIO_RST_VAL_L));
//    printf("L\n");
}

int InitCameraTriggerGPIO(){

    //open gpio
    printf("Start initialize GPIO\n");
    fd_GPIO_P2_c4 = open(SYSFS_GPIO_EXPORT, O_WRONLY);
    if(fd_GPIO_P2_c4 == -1)
    {
              printf("ERR: Radio hard reset pin open error.\n");
              return EXIT_FAILURE;
    }
    write(fd_GPIO_P2_c4, SYSFS_GPIO_RST_PIN_VAL ,sizeof(SYSFS_GPIO_RST_PIN_VAL));
    close(fd_GPIO_P2_c4);

    //set direction
    fd_GPIO_P2_c4 = open(SYSFS_GPIO_RST_DIR, O_WRONLY);
    if(fd_GPIO_P2_c4 == -1)
    {
              printf("ERR: Radio hard reset pin direction open error.\n");
              return EXIT_FAILURE;
    }
    write(fd_GPIO_P2_c4, SYSFS_GPIO_RST_DIR_VAL, sizeof(SYSFS_GPIO_RST_DIR_VAL));
    close(fd_GPIO_P2_c4);

    //output reset sigal
    fd_GPIO_P2_c4 = open(SYSFS_GPIO_RST_VAL, O_RDWR);
    if(fd_GPIO_P2_c4 == -1)
    {
              printf("ERR: Radio hard reset pin value open error.\n");
              return EXIT_FAILURE;
    }
    printf("Successfully initialize GPIO\n");
    return 0;
}

void CloseTimerGPIO(){
    timer_delete(timerid_EXTERN_TRIGGER1);
    close(fd_GPIO_P2_c4);
}

void CreatAndSaveImag(const FramePtr pFrame ){
    static struct PhotoFormat PhotoFormatInfo={.nImageSize=0,\
                                                .nWidth=0,\
                                                .nHeight=0,\
                                                .bFormatGetted=false};

    std::string pFileNameBase = "SynchronousGrab.bmp";
    static int picnum=0;
    picnum++;
    std::string pFileNametemp=pFileNameBase+std::to_string(picnum);
    const char *pFileName=pFileNametemp.c_str();
    VmbErrorType    err         = VmbErrorSuccess;
    VmbPixelFormatType ePixelFormat = VmbPixelFormatMono8;
    if(!PhotoFormatInfo.bFormatGetted){
        VmbUint32_t nImageSize = 0;
        err = pFrame->GetImageSize( nImageSize );
        PhotoFormatInfo.nImageSize=nImageSize;
        if ( VmbErrorSuccess == err )
        {
            VmbUint32_t nWidth = 0;
            err = pFrame->GetWidth( nWidth );
            PhotoFormatInfo.nWidth=nWidth;
            if ( VmbErrorSuccess == err )
            {
                VmbUint32_t nHeight = 0;
                err = pFrame->GetHeight( nHeight );
                PhotoFormatInfo.nHeight=nHeight;
                if ( VmbErrorSuccess == err )
                {   PhotoFormatInfo.bFormatGetted=true;
                    VmbUchar_t *pImage = NULL;
                    err = pFrame->GetImage( pImage );
                    if ( VmbErrorSuccess == err )
                    {

                        AVTBitmap bitmap;

                        if ( VmbPixelFormatRgb8 == ePixelFormat )
                        {
                            bitmap.colorCode = ColorCodeRGB24;
                        }
                        else
                        {
                            bitmap.colorCode = ColorCodeMono8;
                        }

                        bitmap.bufferSize = PhotoFormatInfo.nImageSize;
                        bitmap.width = PhotoFormatInfo.nWidth;
                        bitmap.height = PhotoFormatInfo.nHeight;

                        // Create the bitmap
                        if ( 0 == AVTCreateBitmap( &bitmap, pImage ))
                        {
                            std::cout << "Could not create bitmap.\n";
                            err = VmbErrorResources;
                        }
                        else
                        {
                            // Save the bitmap
                            if ( 0 == AVTWriteBitmapToFile( &bitmap, pFileName ))
                            {
                                std::cout << "Could not write bitmap to file.\n";
                                err = VmbErrorOther;
                            }
                            else
                            {
                                std::cout << "Bitmap successfully written to file \"" << pFileName << "\"\n" ;
                                // Release the bitmap's buffer
                                if ( 0 == AVTReleaseBitmap( &bitmap ))
                                {
                                    std::cout << "Could not release the bitmap.\n";
                                    err = VmbErrorInternalFault;
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    else{
        VmbUchar_t *pImage = NULL;
        err = pFrame->GetImage( pImage );
        if ( VmbErrorSuccess == err )
        {

            AVTBitmap bitmap;

            if ( VmbPixelFormatRgb8 == ePixelFormat )
            {
                bitmap.colorCode = ColorCodeRGB24;
            }
            else
            {
                bitmap.colorCode = ColorCodeMono8;
            }

            bitmap.bufferSize = PhotoFormatInfo.nImageSize;
            bitmap.width = PhotoFormatInfo.nWidth;
            bitmap.height = PhotoFormatInfo.nHeight;

            // Create the bitmap
            if ( 0 == AVTCreateBitmap( &bitmap, pImage ))
            {
                std::cout << "Could not create bitmap.\n";
                err = VmbErrorResources;
            }
            else
            {
                // Save the bitmap
                if ( 0 == AVTWriteBitmapToFile( &bitmap, pFileName ))
                {
                    std::cout << "Could not write bitmap to file.\n";
                    err = VmbErrorOther;
                }
                else
                {
                    std::cout << "Bitmap successfully written to file \"" << pFileName << "\"\n" ;
                    // Release the bitmap's buffer
                    if ( 0 == AVTReleaseBitmap( &bitmap ))
                    {
                        std::cout << "Could not release the bitmap.\n";
                        err = VmbErrorInternalFault;
                    }
                }
            }
        }
    }

}

void CheckAcquisitionStatus(){
    std::cout<<"check acquisition status..."<<std::endl;
    SetAcquisitionStatusSelector(TheCamera.camera,"AcquisitionActive");
    bool AcquisitionStatus;
    FeaturePtr pFeature;
    if(VmbErrorSuccess==TheCamera.camera->GetFeatureByName ("AcquisitionStatus", pFeature )){
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

