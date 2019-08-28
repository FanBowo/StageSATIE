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

#define UseDefaultPhotoFormat
#ifdef UseDefaultPhotoFormat
    #define Default_Size  1310720
    #define Default_Width   1280
    #define Default_Height 1024
#endif // UseDefaultPhotoFormat
typedef struct{
    float timestamp;                /**< time is in seconds */
    sensors_event_t   CameraPose;  /**< orientation values are in degrees *//**< orientation values are in degrees */
    VmbUchar_t pImage[Default_Size];/*Pointer of image data*/
} Camera_IMU_Data_t;

typedef struct{
  float latitude;   ///< Floating point latitude value in degrees/minutes as received from the GPS (DDMM.MMMM)
  float longitude;  ///< Floating point longitude value in degrees/minutes as received from the GPS (DDDMM.MMMM)

//  float latitudeDegrees;    ///< Latitude in decimal degrees
//  float longitudeDegrees;   ///< Longitude in decimal degrees
//  float geoidheight;        ///< Diff between geoid height and WGS84 height
//  float altitude;           ///< Altitude in meters above MSL
  float speed;              ///< Current speed over ground in knots
  float angle;              ///< Course in degrees from true north
//  float magvariation;       ///< Magnetic variation in degrees (vs. true north)
//  float HDOP;               ///< Horizontal Dilution of Precision - relative accuracy of horizontal position
  char lat;                 ///< N/S
  char lon;                 ///< E/W
//  char mag;                 ///< Magnetic variation direction
  boolean fix;              ///< Have a fix?
  int fixquality;       ///< Fix quality (0, 1, 2 = Invalid, GPS, DGPS)
  int satellites;       ///< Number of satellites in use
  double TimeStamp;

} GPS_data_t;

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
    bool bCSV_PointerPrepared;
//    double IMU_TimeStamp;
    double DeviceTimeStamp;
    std::queue <IMU_RawData_t>IMU_RawDataFifo;
    std::ofstream pSaveRawIMU_Data;
    std::ofstream pSaveCamera_IMU_Data;
    std::ofstream pSaveGPS_Data;
    std::queue <Camera_IMU_Data_t>Camera_IMU_DataFifo;
    std::queue <GPS_data_t>GPS_DataFifo;
    ~Assemble();
};


// what's the name of the hardware serial port?



#endif // ASSEMBLE_H_INCLUDED
