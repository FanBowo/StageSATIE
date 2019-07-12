
#include "Tasks.h"

pthread_mutex_t TimeStampBaseMutex =PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t TimeStampBaseCond=PTHREAD_COND_INITIALIZER;

void * UpdateTimeStampBaseFunc(void *){
    while(1){
        pthread_mutex_lock(& TimeStampBaseMutex );

        pthread_cond_wait(&TimeStampBaseCond,&TimeStampBaseMutex);

        if (( AssembleDevice.GPS.parse(AssembleDevice.GPS.lastNMEA()) && AssembleDevice.GPS.bRecvdRMCflag() )){
        // this also sets the newNMEAreceived() flag to false
            if(EnableParseOutput){
                AssembleDevice.GPS.ResetRecvdRMCflag();
                // if millis() or timer wraps around, we'll just reset it
                std::cout<<"\nTime: "<<std::endl;
                std::cout<<(int)AssembleDevice.GPS.hour<<":"<<(int)AssembleDevice.GPS.minute<<":"<<(int)AssembleDevice.GPS.seconds<<":"<<AssembleDevice.GPS.milliseconds<<std::endl;
                std::cout<<"Date: "<<std::endl;
                std::cout<<(int)AssembleDevice.GPS.day<<":"<<(int)AssembleDevice.GPS.month<<":"<<(int)AssembleDevice.GPS.year<<std::endl;
                std::cout<<"Fix: "<<std::endl;
                std::cout<<(int)AssembleDevice.GPS.fix<<std::endl;
                std::cout<<"quality: "<<std::endl;
                std::cout<<(int)AssembleDevice.GPS.fixquality<<std::endl;
                if (AssembleDevice.GPS.fix) {
                    std::cout<<"Location: "<<std::endl;
                    std::cout<<AssembleDevice.GPS.latitude<<","<<AssembleDevice.GPS.lat<<","<<AssembleDevice.GPS.longitude<<","<<AssembleDevice.GPS.lon<<std::endl;
                    std::cout<<"Speed (knots): "<<std::endl;
                    std::cout<<AssembleDevice.GPS.speed<<std::endl;
                    std::cout<<"Angle: "<<std::endl;
                    std::cout<<AssembleDevice.GPS.angle<<std::endl;
                    std::cout<<"Altitude: "<<std::endl;
                    std::cout<<AssembleDevice.GPS.altitude<<std::endl;
                    std::cout<<"Satellites:: "<<std::endl;
                    std::cout<<AssembleDevice.GPS.satellites<<std::endl;
                }
                //delay(2000);
            }
        }

        pthread_mutex_unlock(& TimeStampBaseMutex );
    }
}
