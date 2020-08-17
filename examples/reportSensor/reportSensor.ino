/*
  Supported DEVIO NB-DEVKIT I Board 
    |  Do not use PIN   |
    |      16 TX        |
    |      17 RX        |
    |      4 EINT       |
    |   26 power key    |
    |     27 reset      |
  Pre required install library
         - ClosedCube_HDC1080
         
  This is an example for DEVIO NB-DEVKIT I. That has read sensor on board and report data to Magellan IoT Platform.
  
  Please login and enjoy with https://magellan.ais.co.th
  
  If you have any questions, please see more details at https://magellan.ais.co.th
*/
#include "ClosedCube_HDC1080.h"
#include "Magellan_SIM7020E.h"
Magellan_SIM7020E magel;          

ClosedCube_HDC1080 hdc1080;

const int lightSensorPin=34; 
String payload;

void setup() 
{
  Serial.begin(115200);

  Serial.println("ClosedCube HDC1080");

  // Default settings:
  // - Heater off
  // - 14 bit Temperature and Humidity Measurement Resolutions
  hdc1080.begin(0x40);
   
  Serial.print("Manufacturer ID=0x");
  Serial.println(hdc1080.readManufacturerId(), HEX); // 0x5449 ID of Texas Instruments
  Serial.print("Device ID=0x");
  Serial.println(hdc1080.readDeviceId(), HEX);       // 0x1050 ID of the device
      
  magel.begin();                                     //Init Magellan LIB
}

void loop() 
{
   /*
    Example read sensor on board and report data to Magellan IoT platform
  */
  String temperature=String(hdc1080.readTemperature());
  String humidity=String(hdc1080.readHumidity());
  String Light=String(analogRead(lightSensorPin));
  
  payload="{\"temperature\":"+temperature+",\"humidity\":"+humidity+",\"light\":"+Light+"}";  //Please provide payload with json format
  magel.report(payload);                                                                      //Report sensor data
  delay(5000);                                                                                //Delay 5 second
}
