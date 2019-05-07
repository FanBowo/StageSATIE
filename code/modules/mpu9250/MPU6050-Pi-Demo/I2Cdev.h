// I2Cdev library collection - Main I2C device class header file
// Abstracts bit and byte I2C R/W functions into a convenient class
// 6/9/2012 by Jeff Rowberg <jeff@rowberg.net>
//
// Changelog:
//     2012-06-09 - fix major issue with reading > 32 bytes at a time with Arduino Wire
//                - add compiler warnings when using outdated or IDE or limited I2Cdev implementation
//     2011-11-01 - fix write*Bits mask calculation (thanks sasquatch @ Arduino forums)
//     2011-10-03 - added automatic Arduino version detection for ease of use
//     2011-10-02 - added Gene Knight's NBWire TwoWire class implementation with small modifications
//     2011-08-31 - added support for Arduino 1.0 Wire library (methods are different from 0.x)
//     2011-08-03 - added optional timeout parameter to read* methods to easily change from default
//     2011-08-02 - added support for 16-bit registers
//                - fixed incorrect Doxygen comments on some methods
//                - added timeout value for read operations (thanks mem @ Arduino forums)
//     2011-07-30 - changed read/write function structures to return success or byte counts
//                - made all methods static for multi-device memory savings
//     2011-07-28 - initial release

/* ============================================
I2Cdev device library code is placed under the MIT license
Copyright (c) 2012 Jeff Rowberg

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
===============================================
*/

#ifndef _I2CDEV_H_
#define _I2CDEV_H_

#ifndef TRUE
#define TRUE	(1==1)
#define FALSE	(0==1)
#endif

class I2Cdev {
    public:
        I2Cdev();
        
        static int8_t LinuxReadBit(uint8_t devAddr, uint8_t regAddr, uint8_t bitNum, uint8_t *data, uint16_t timeout=I2Cdev::LinuxReadTimeout);
        static int8_t LinuxReadBitW(uint8_t devAddr, uint8_t regAddr, uint8_t bitNum, uint16_t *data, uint16_t timeout=I2Cdev::LinuxReadTimeout);
        static int8_t LinuxReadBits(uint8_t devAddr, uint8_t regAddr, uint8_t bitStart, uint8_t length, uint8_t *data, uint16_t timeout=I2Cdev::LinuxReadTimeout);
        static int8_t LinuxReadBitsW(uint8_t devAddr, uint8_t regAddr, uint8_t bitStart, uint8_t length, uint16_t *data, uint16_t timeout=I2Cdev::LinuxReadTimeout);
        static int8_t LinuxReadByte(uint8_t devAddr, uint8_t regAddr, uint8_t *data, uint16_t timeout=I2Cdev::LinuxReadTimeout);
        static int8_t LinuxReadWord(uint8_t devAddr, uint8_t regAddr, uint16_t *data, uint16_t timeout=I2Cdev::LinuxReadTimeout);
        static int8_t LinuxReadBytes_T(uint8_t devAddr, uint8_t regAddr, uint8_t length, uint8_t *data, uint16_t timeout=I2Cdev::LinuxReadTimeout);
		static int8_t LinuxReadBytes(uint8_t devAddr, uint8_t regAddr, uint8_t length, uint8_t *data);
        static int8_t LinuxReadWords(uint8_t devAddr, uint8_t regAddr, uint8_t length, uint16_t *data, uint16_t timeout=I2Cdev::LinuxReadTimeout);

        static bool LinuxWriteBit(uint8_t devAddr, uint8_t regAddr, uint8_t bitNum, uint8_t data);
        static bool LinuxWriteBitW(uint8_t devAddr, uint8_t regAddr, uint8_t bitNum, uint16_t data);
        static bool LinuxWriteBits(uint8_t devAddr, uint8_t regAddr, uint8_t bitStart, uint8_t length, uint8_t data);
        static bool LinuxWriteBitsW(uint8_t devAddr, uint8_t regAddr, uint8_t bitStart, uint8_t length, uint16_t data);
        static bool LinuxWriteByte(uint8_t devAddr, uint8_t regAddr, uint8_t data);
        static bool LinuxWriteWord(uint8_t devAddr, uint8_t regAddr, uint16_t data);
        static bool LinuxWriteBytes(uint8_t devAddr, uint8_t regAddr, uint8_t length, uint8_t *data);
        static bool LinuxWriteWords(uint8_t devAddr, uint8_t regAddr, uint8_t length, uint16_t *data);

        static uint16_t LinuxReadTimeout;
};

#endif /* _I2CDEV_H_ */
