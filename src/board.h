/*
Copyright (c) 2020, Advanced Wireless Network
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice, this
  list of conditions and the following disclaimer.

* Redistributions in binary form must reproduce the above copyright notice,
  this list of conditions and the following disclaimer in the documentation
  and/or other materials provided with the distribution.

* Neither the name of the copyright holder nor the names of its
  contributors may be used to endorse or promote products derived from
  this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

Author: Device Innovation team     
Create Date: 2 January 2020. 
Modified: 11 February 2020.

Released for private usage
*/

// Define the board used AltSoftSerial.h 
#if defined(ARDUINO_AVR_UNO) || (ARDUINO_AVR_MEGA2560)
    #define ATLSOFTSERIAL 1
#else 
    #define ATLSOFTSERIAL 0
#endif 
#if ATLSOFTSERIAL 
	#include "AltSoftSerial.h"
#endif


//Hardware specification and configuration
#if defined( __ARDUINO_X86__)
	#define isHwReset 0
	#define isATReset 1
	#define isNetLight 0
	#define serialConfig 0

	//Serial configuration
    HardwareSerial serialPort(1);
    #define buadrate 9600
    #define description "PLEASE USE Hardware Serial"

#elif defined(ARDUINO_NUCLEO_L476RG) || (ARDUINO_STM32L4_NUCLEO) || (ARDUINO_ARCH_STM32) || (NUCLEO_L476RG)
	
	#define isHwReset 0
	#define isATReset 1
	#define hwResetPin 26
	#define isNetLight 0
	#define netLightPin 2

    //Serial configuration
	HardwareSerial serialPort(UART4); //NUCLEO_L053
	#define buadrate 9600
	#define description "PLEASE USE PIN RX=4 & TX=5"

#elif defined(ARDUINO_Node32s)
	#define isHwReset 0
	#define isATReset 1
	#define isNetLight 0

	//Serial configuration
    //Serial2 serialPort;
    HardwareSerial serialPort(2);
    #define buadrate 9600
    #define description "PLEASE USE PIN RX=RX2 & TX=TX2 & 3V3=IOREF"

#elif defined(ARDUINO_NodeMCU_32S)
 	#define isHwReset 0
	#define isATReset 1
	#define isNetLight 0

	//Serial configuration
    HardwareSerial serialPort(2);
    #define buadrate 9600
    #define description "PLEASE USE PIN RX=RX2 & TX=TX2 & 3V3=IOREF"

#elif defined(ARDUINO_ESP32_DEV) ||(ARDUINO_ARCH_ESP32)
 	#define isHwReset 1
 	#define hwResetPin 26
	#define isATReset 1
	#define isNetLight 0

	#define serialConfig 1
	#define rxPin 16
	#define txPin 17
	#define configParam SERIAL_8N1

	//Serial configuration
	//HardwareSerial serialPort(2);
    HardwareSerial serialPort(1);
    #define buadrate 115200
    #define description "PLEASE USE PIN RX=RX2 & TX=TX2 & 3V3=IOREF"

#elif defined(ARDUINO_AVR_UNO) || (ARDUINO_AVR_MEGA2560)
 	#define isHwReset 0
	#define isATReset 1
	#define isNetLight 0

	//Serial configuration
    AltSoftSerial serialPort;
    #define buadrate 9600
    #define description "PLEASE USE PIN RX=48 & TX=46"
#else 
 	#define isHwReset 0
	#define isATReset 1
	#define isNetLight 0

	//Serial configuration
    AltSoftSerial serialPort;
    #define buadrate 9600
    #define description "USE AltSoftSerial Serial"
#endif