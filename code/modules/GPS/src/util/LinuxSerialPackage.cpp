#include "LinuxSerialPackage.h"


LinuxSerialPackage ::LinuxSerialPackage(){
    ;
}

LinuxSerialPackage ::LinuxSerialPackage(const std::string&   fileName){
    _PortName=fileName;
}

char LinuxSerialPackage::read(){
    char tempChar;
    ReadByte(tempChar);
    return tempChar;
}

void LinuxSerialPackage::begin(uint32_t BaudRateVal){
    Open(_PortName);
    std::cout<<"Open Serial Port Succeded"<<std::endl;
    LibSerial::BaudRate TempBaudRate;
    switch (BaudRateVal){
        case 9600: TempBaudRate=LibSerial::BaudRate::BAUD_9600;break;
        case 19200: TempBaudRate=LibSerial::BaudRate::BAUD_19200;break;
        case 38400: TempBaudRate=LibSerial::BaudRate::BAUD_38400;break;
        case 57600: TempBaudRate=LibSerial::BaudRate::BAUD_57600;break;
        case 115200: TempBaudRate=LibSerial::BaudRate::BAUD_115200;break;
        case 230400: TempBaudRate=LibSerial::BaudRate::BAUD_230400;break;
        case 460800: TempBaudRate=LibSerial::BaudRate::BAUD_460800;break;
        case 500000: TempBaudRate=LibSerial::BaudRate::BAUD_500000;break;
        case 576000: TempBaudRate=LibSerial::BaudRate::BAUD_576000;break;
        case 1000000: TempBaudRate=LibSerial::BaudRate::BAUD_1000000;break;
        case 1152000: TempBaudRate=LibSerial::BaudRate::BAUD_1152000;break;
        case 1500000: TempBaudRate=LibSerial::BaudRate::BAUD_500000;break;
        default:std::cout<<"This baund rate note supported"<<BaudRateVal<<"bps"<<std::endl;break;

    }
    SetFlowControl( LibSerial::FlowControl::FLOW_CONTROL_NONE );
    SetCharacterSize( LibSerial::CharacterSize::CHAR_SIZE_8 );
    SetParity( LibSerial::Parity::PARITY_NONE );
    SetBaudRate( TempBaudRate);
    SetStopBits( LibSerial::StopBits::STOP_BITS_1 );

}

void LinuxSerialPackage:: println(const char *str){
    Write(str);
}

bool LinuxSerialPackage::available(){
    return IsDataAvailable();
}

LinuxSerialPackage::~LinuxSerialPackage(){
    close();
}

void LinuxSerialPackage::close(){
    if(IsOpen()){
        this->Close();
    }
}
