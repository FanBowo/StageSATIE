#ifndef ASSEMBLE_H_INCLUDED
#define ASSEMBLE_H_INCLUDED


#include <iostream>
#include <unistd.h>
#include <sys/signal.h>
#include <fcntl.h>

#include "Adafruit_GPS.h"//GPS Module
#include "BNO055_Cali.h"//IMU Module

#define delay(a) usleep(1000*a);

constexpr const char* const SERIAL_PORT_1 = "/dev/ttyUSB0" ;

class Assemble{

    public:
    Assemble();
    void InitGPS_Module();
    void InitIMU_Module();
    void UpdateIMU_RawData();
    static LinuxSerialPackage GPSSerial;
    static Adafruit_GPS GPS;
    static BNO055_Cali IMU_BNO055;
    double IMU_TimeStamp;
};

// what's the name of the hardware serial port?



#endif // ASSEMBLE_H_INCLUDED
