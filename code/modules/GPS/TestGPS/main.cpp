


#include "Adafruit_GPS.h"
#include <iostream>
#include <unistd.h>

#define delay(a) usleep(1000*a);
constexpr const char* const SERIAL_PORT_1 = "/dev/ttyUSB0" ;
// what's the name of the hardware serial port?
LinuxSerialPackage GPSSerial(SERIAL_PORT_1);

// Connect to the GPS on the hardware port
Adafruit_GPS GPS(&GPSSerial);

// Set GPSECHO to 'false' to turn off echoing the GPS data to the Serial console
// Set to 'true' if you want to debug and listen to the raw GPS sentences
#define GPSECHO false


void init()
{

    std::cout<<"Adafruit GPS library basic test!"<<std::endl;
    // 9600 NMEA is the default baud rate for Adafruit MTK GPS's- some use 4800
    GPS.begin(9600);
    // uncomment this line to turn on RMC (recommended minimum) and GGA (fix data) including altitude
    GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);
    // uncomment this line to turn on only the "minimum recommended" data
    //GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCONLY);
    // For parsing data, we don't suggest using anything but either RMC only or RMC+GGA since
    // the parser doesn't care about other sentences at this time
    // Set the update rate
    GPS.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ); // 1 Hz update rate
    // For the parsing code to work nicely and have time to sort thru the data, and
    // print it out we don't suggest using anything higher than 1 Hz

    // Request updates on antenna status, comment out to keep quiet
    GPS.sendCommand(PGCMD_ANTENNA);

    delay(1000);

    // Ask for firmware version
    GPSSerial.println(PMTK_Q_RELEASE);
}

int main() // run over and over again
{
    init();
    while(1){
         // read data from the GPS in the 'main loop'
        char c = GPS.read();
        // if you want to debug, this is a good time to do it!
        if (GPSECHO)
            if (c) std::cout<<c<<std::endl;
        // if a sentence is received, we can check the checksum, parse it...
        if (GPS.newNMEAreceived()) {
            // a tricky thing here is if we print the NMEA sentence, or data
            // we end up not listening and catching other sentences!
            // so be very wary if using OUTPUT_ALLDATA and trytng to print out data
        std::cout<<GPS.lastNMEA()<<std::endl; // this also sets the newNMEAreceived() flag to false
        if (!GPS.parse(GPS.lastNMEA())) // this also sets the newNMEAreceived() flag to false
            continue; // we can fail to parse a sentence in which case we should just wait for another
        }
        // if millis() or timer wraps around, we'll just reset it
        std::cout<<"\nTime: "<<std::endl;
        std::cout<<GPS.hour<<":"<<GPS.minute<<":"<<GPS.seconds<<":"<<GPS.milliseconds<<std::endl;
        std::cout<<"Date: "<<std::endl;
        std::cout<<GPS.day<<":"<<GPS.month<<":"<<GPS.year<<":"<<GPS.milliseconds<<std::endl;
        std::cout<<"Fix: "<<std::endl;
        std::cout<<(int)GPS.fix<<std::endl;
        std::cout<<"quality: "<<std::endl;
        std::cout<<(int)GPS.fixquality<<std::endl;
        if (GPS.fix) {
            std::cout<<"Location: "<<std::endl;
            std::cout<<GPS.latitude<<","<<GPS.lat<<","<<GPS.longitude<<","<<GPS.lon<<std::endl;
            std::cout<<"Speed (knots): "<<std::endl;
            std::cout<<GPS.speed<<std::endl;
            std::cout<<"Angle: "<<std::endl;
            std::cout<<GPS.angle<<std::endl;
            std::cout<<"Altitude: "<<std::endl;
            std::cout<<GPS.altitude<<std::endl;
            std::cout<<"Satellites:: "<<std::endl;
            std::cout<<GPS.satellites<<std::endl;
        }
    }

}



