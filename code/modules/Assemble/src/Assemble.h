#ifndef ASSEMBLE_H_INCLUDED
#define ASSEMBLE_H_INCLUDED

#include <queue>
#include <iostream>
#include <unistd.h>
#include <sys/signal.h>
#include <fcntl.h>

#include "CameraMako130.h"//Camera module
#include "Adafruit_GPS.h"//GPS Module
#include "BNO055_Cali.h"//IMU Module

//#define delay(a) usleep(1000*a);

constexpr const char* const SERIAL_PORT_1 = "/dev/ttyUSB0" ;
typedef struct{
    float x;
    float y;
    float z;
}sensors_save_vec_t;

typedef struct{
    float timestamp;                 /**< time is in seconds */
    sensors_save_vec_t   acceleration; /**< acceleration values are in meter per second per second (m/s^2) */
    sensors_save_vec_t   gyro;         /**< gyroscope values are in rad/s */
} IMU_RawData_t;

typedef struct{
    float timestamp;                /**< time is in seconds */
    sensors_event_t   CameraPose;  /**< orientation values are in degrees *//**< orientation values are in degrees */
    VmbUchar_t *pImage;/*Pointer of image data*/
} Camera_IMU_Data_t;

class Assemble{

    public:
    Assemble();
    void InitGPS_Module();
    void InitIMU_Module();
    static LinuxSerialPackage GPSSerial;
    static Adafruit_GPS GPS;
    static BNO055_Cali IMU_BNO055;
    static CameraMako130 TheCamera;
    static struct PhotoFormat PhotoFormatInfo;
    bool bIMU_Data_Stable;
    double IMU_TimeStamp;
    std::queue <IMU_RawData_t>IMU_RawDataFifo;
    std::ofstream pSaveRawIMU_Data;
    std::ofstream pSaveCamera_IMU_Data;
    std::queue <Camera_IMU_Data_t>Camera_IMU_DataFifo;
    ~Assemble();
};


// what's the name of the hardware serial port?



#endif // ASSEMBLE_H_INCLUDED
