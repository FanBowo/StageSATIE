#ifndef ASSEMBLE_H_INCLUDED
#define ASSEMBLE_H_INCLUDED

#include <queue>
#include <iostream>
#include <unistd.h>
#include <sys/signal.h>
#include <fcntl.h>
#

#include "Adafruit_GPS.h"//GPS Module
#include "BNO055_Cali.h"//IMU Module

#define delay(a) usleep(1000*a);

constexpr const char* const SERIAL_PORT_1 = "/dev/ttyUSB0" ;
typedef struct{
    float timestamp;                        /**< time is in milliseconds */
    sensors_vec_t   acceleration;         /**< acceleration values are in meter per second per second (m/s^2) */
    sensors_vec_t   gyro;                 /**< gyroscope values are in rad/s */
} IMU_RawData_t;

class Assemble{

    public:
    Assemble();
    void InitGPS_Module();
    void InitIMU_Module();
    static LinuxSerialPackage GPSSerial;
    static Adafruit_GPS GPS;
    static BNO055_Cali IMU_BNO055;
    double IMU_TimeStamp;
    std::queue <IMU_RawData_t>IMU_RawDataFifo;
};

// what's the name of the hardware serial port?



#endif // ASSEMBLE_H_INCLUDED
