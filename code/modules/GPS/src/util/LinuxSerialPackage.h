#ifndef LINUXSERIALPACKAGE_H_INCLUDED
#define LINUXSERIALPACKAGE_H_INCLUDED

#include "SerialPort.h"
#include <iostream>

class LinuxSerialPackage : public LibSerial::SerialPort
{
    public:

         /**
         * @brief Default Constructor.
         */
        explicit LinuxSerialPackage() ;

        /**
         * @brief Constructor that allows a SerialPort instance to be
         *        created and opened, initializing the corresponding
         *        serial port with the specified parameters.
         * @param fileName The file name of the serial port.
         * @param baudRate The communications baud rate.
         * @param characterSize The size of the character buffer for
         *        storing read/write streams.
         * @param parityType The parity type for the serial port.
         * @param stopBits The number of stop bits for the serial port.
         * @param flowControlType The flow control type for the serial port.
         */
        explicit LinuxSerialPackage(const std::string&   fileName) ;

        /**
         * @brief Default Destructor for a SerialPort object. Closes the
         *        serial port associated with mFileDescriptor if open.
         */
        virtual ~LinuxSerialPackage() noexcept;

        char read();

        void begin(uint32_t BaudRateVal);

        void println(const char *str);

        bool available();

        void close();

        int SerialPortFilefileDescriptor;

    private:
        std::string _PortName;

} ; // class LinuxSerialPackage


#endif // LINUXSERIALPACKAGE_H_INCLUDED
