/*  
  v1.1.0
  Supported DEVIO NB-DEVKIT I Board
    |  Do not use PIN   |
    |      16 TX        |
    |      17 RX        |
    |      4 EINT       |
    |   26 power key    |
    |     27 reset      |
  Example for get config data from Magellan IoT Platform
  
  Please login and enjoy with https://magellan.ais.co.th
*/
#include "Magellan_SIM7020E.h"
Magellan_SIM7020E magel;          

String threshold;

void setup() 
{
  Serial.begin(9600);
  magel.begin();           //Init Magellan LIB
}

void loop() 
{
  /*
    Example get config data from Magellan IoT platform
  */
  threshold = magel.getConfig("threshold"); //Get configuration data from Magellan
  Serial.print("threshold = ");
  
  if(threshold.indexOf("40300")!=-1)        //Response data is 40300
  {
    Serial.println("Error Get Config");
  }
  else
  {
    Serial.println(threshold);              //Config data from Magellan IoT platform
  }
  Serial.println();
}
