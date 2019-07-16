
#include "Tasks.h"
Assemble AssembleDevice;

pthread_mutex_t TimeStampBaseMutex =PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t TimeStampBaseCond=PTHREAD_COND_INITIALIZER;
pthread_mutex_t TimeStampBaseReNewMutex =PTHREAD_MUTEX_INITIALIZER;

pthread_cond_t IMU_TimeStampCond=PTHREAD_COND_INITIALIZER;
pthread_mutex_t IMU_TimeStampMutex =PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t IMU_TimerCounterMutex=PTHREAD_MUTEX_INITIALIZER;

timer_t IMU_Timer;
struct itimerspec IMU_Timer_trigger;
int IMU_TimerCounter=0;

void * UpdateTimeStampBaseFunc(void *){
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
    while(1){

        pthread_mutex_lock(&IMU_TimeStampMutex);
        pthread_cond_wait(&IMU_TimeStampCond,&IMU_TimeStampMutex);

        pthread_mutex_lock(& TimeStampBaseReNewMutex );
        pthread_mutex_lock(&IMU_TimerCounterMutex);

        AssembleDevice.IMU_TimeStamp=IMU_TimerCounter*(1.0/(float)TimerIMUFre)+ \
                                AssembleDevice.GPS.GpsTimeGetted;
        std::cout<<"TimeStamp: "<<AssembleDevice.IMU_TimeStamp<<std::endl;
        AssembleDevice.UpdateIMU_RawData();
        pthread_mutex_unlock(&IMU_TimerCounterMutex);
        pthread_mutex_unlock(& TimeStampBaseReNewMutex );

        pthread_mutex_unlock(&IMU_TimeStampMutex);
    }

}
