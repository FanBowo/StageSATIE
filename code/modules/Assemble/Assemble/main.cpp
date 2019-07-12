/*Version 1.0 April 18,2019 15:15 Functions of modules GPS test completed */
/*The next version to test GPS with interrupt of serial port*/




#include "Tasks.h"



// Set GPSECHO to 'false' to turn off echoing the GPS data to the Serial console
// Set to 'true' if you want to debug and listen to the raw GPS sentences
#define GPSECHO false

Assemble AssembleDevice;

void signal_handler_IO (int status);   /* definition of signal handler,
                                        used to process serial port interrupt */

struct sigaction saio;
bool RevNewPack =false;
void init()
{

    int fd =AssembleDevice.GPSSerial.SerialPortFilefileDescriptor;
    saio.sa_handler = signal_handler_IO;
    saio.sa_flags = 0;
    saio.sa_restorer = NULL;
    sigaction(SIGIO,&saio,NULL);

    fcntl(fd, F_SETFL, FNDELAY|FASYNC);
    fcntl(fd, F_SETOWN, getpid());
}


int main() // run over and over again
{
    AssembleDevice.InitGPS_Module();
    init();

    std::cout<<"Initialization finished"<<std::endl;
    pthread_t ThreadUpdateTimeStampBase;
    pthread_create(&ThreadUpdateTimeStampBase,NULL,&UpdateTimeStampBaseFunc,NULL);

    pthread_join(ThreadUpdateTimeStampBase,NULL);

    std::cout<<"End of programme"<<std::endl;
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


