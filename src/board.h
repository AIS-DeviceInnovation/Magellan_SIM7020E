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
    #define buadrate 9600
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