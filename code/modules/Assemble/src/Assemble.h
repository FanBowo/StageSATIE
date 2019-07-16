#ifndef ASSEMBLE_H_INCLUDED
#define ASSEMBLE_H_INCLUDED

#include "Adafruit_GPS.h"
#include <iostream>
#include <unistd.h>
#include <sys/signal.h>
#include <fcntl.h>

#define delay(a) usleep(1000*a);

constexpr const char* const SERIAL_PORT_1 = "/dev/ttyUSB0" ;

class Assemble{

    public:
    Assemble();
    void InitGPS_Module();
    static LinuxSerialPackage GPSSerial;
    static Adafruit_GPS GPS;
    double IMU_TimeStamp;
};

// what's the name of the hardware serial port?



#endif // ASSEMBLE_H_INCLUDED
