#include "Assemble.h"

Assemble::Assemble()
{
//    // what's the name of the hardware serial port?
//    LinuxSerialPackage GPSSerial(SERIAL_PORT_1);
//    // Connect to the GPS on the hardware port
//    Adafruit_GPS GPS(&_GPSSerialPort);
    IMU_TimeStamp=0.0;
    pSaveRawIMU_Data.open("imu0.csv",std::ios::out|std::ios::trunc);
    pSaveRawIMU_Data<<"timestamp"<<","\
                    <<"omega_x"<<","<<"omega_y"<<","<<"omega_z"<<","\
                    <<"alpha_x"<<","<<"alpha_y"<<","<<"alpha_z"<<std::endl;
    pSaveCamera_IMU_Data.open("Camera_IMU.csv",std::ios::out|std::ios::trunc);
    pSaveCamera_IMU_Data<<"timestamp"<<","\
                    <<"x"<<","<<"y"<<","<<"z"<<std::endl;
}

Assemble::~Assemble(){
    pSaveRawIMU_Data.close();
    pSaveCamera_IMU_Data.close();
}

LinuxSerialPackage Assemble::GPSSerial(SERIAL_PORT_1);

// Connect to the GPS on the hardware port
Adafruit_GPS Assemble:: GPS(&GPSSerial);
BNO055_Cali Assemble::IMU_BNO055 =BNO055_Cali();
CameraMako130 Assemble::TheCamera=CameraMako130();

struct PhotoFormat Assemble::PhotoFormatInfo={.nImageSize=0,\
                            .nWidth=0,\
                            .nHeight=0,\
                            .bFormatGetted=false};

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


void Assemble::InitIMU_Module(){
  //Serial.begin(9600);
  //Serial.println("Orientation Sensor Raw Data Test"); Serial.println("");
  std::cout<<"Orientation Sensor Raw Data Test"<<std::endl;

  /* Initialise the sensor */

  if(!IMU_BNO055.begin(BNO055_Cali::OPERATION_MODE_NDOF))
  {
    /* There was a problem detecting the BNO055 ... check your connections */
    //Serial.print("Ooops, no BNO055 detected ... Check your wiring or I2C ADDR!");
    std::cout<<"Ooops, no BNO055 detected ... Check your wiring or I2C ADDR!"<<std::endl;

    while(1);
  }

  delay(1000);
  #ifdef Debug
  if(bno.bMagnBiasGeted){
    std::cout<<"Rewrite magn bias"<<std::endl;
    bno.setMode(BNO055_Cali::OPERATION_MODE_CONFIG);
    bno.setSensorOffsets(bno.OffsetToWriteToRegister);
    bno.setMode(BNO055_Cali::OPERATION_MODE_NDOF);
    delay(1000);
  }
  #endif // Debug
  /* Display the current temperature */
  int8_t temp = IMU_BNO055.getTemp();
  std::cout<<"Current Temperature: "<<temp<<" C"<<std::endl;
//  Serial.print("Current Temperature: ");
//  Serial.print(temp);
//  Serial.println(" C");
//  Serial.println("");

  IMU_BNO055.setExtCrystalUse(true);
    std::cout<<"Calibration status values: 0=uncalibrated, 3=fully calibrated"<<std::endl;
  //Serial.println("Calibration status values: 0=uncalibrated, 3=fully calibrated");

}

