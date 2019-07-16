#include "Assemble.h"

Assemble::Assemble()
{
//    // what's the name of the hardware serial port?
//    LinuxSerialPackage GPSSerial(SERIAL_PORT_1);
//    // Connect to the GPS on the hardware port
//    Adafruit_GPS GPS(&_GPSSerialPort);
    IMU_TimeStamp=0.0;
}

LinuxSerialPackage Assemble::GPSSerial(SERIAL_PORT_1);

// Connect to the GPS on the hardware port
Adafruit_GPS Assemble:: GPS(&GPSSerial);


void Assemble::InitGPS_Module(){
    std::cout<<"Adafruit GPS library basic test!"<<std::endl;
    // 9600 NMEA is the default baud rate for Adafruit MTK GPS's- some use 4800

    GPS.begin(9600);
//    std::cout<<"Set baud rate"<<std::endl;
////    GPS.sendCommand(PMTK_SET_BAUD_115200);
//////
////    GPS.SetBaudRate(115200);
//    std::cout<<"Set baud rate finished"<<std::endl;

    // uncomment this line to turn on RMC (recommended minimum) and GGA (fix data) including altitude
    //GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);
    // uncomment this line to turn on only the "minimum recommended" data
    GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCONLY);
    // For parsing data, we don't suggest using anything but either RMC only or RMC+GGA since
    // the parser doesn't care about other sentences at this time
    // Set the update rate
    GPS.sendCommand(PMTK_SET_NMEA_UPDATE_10HZ); // 1 Hz update rate
    // For the parsing code to work nicely and have time to sort thru the data, and
    // print it out we don't suggest using anything higher than 1 Hz

    // Request updates on antenna status, comment out to keep quiet
    //GPS.sendCommand(PGCMD_ANTENNA);

    delay(1000);

    // Ask for firmware version
    GPSSerial.println(PMTK_Q_RELEASE);
}


