/*
  Supported DEVIO NB-DEVKIT I Board 
    |  Do not use PIN   |
    |      16 TX        |
    |      17 RX        |
    |      4 EINT       |
    |   26 power key    |
    |     27 reset      |
  This is an example for DEVIO NB-DEVKIT I. That has get swich data from Magellan IoT Platform for control LED on board.
  
  Please login and enjoy with https://magellan.ais.co.th
  
  If you have any questions, please see more details at https://magellan.ais.co.th
*/
#include "Magellan_SIM7020E.h"
Magellan_SIM7020E magel;          

String sw_state;
String sw;
String payload;
const int ledPin = 2;      //LED on board Pin 2

void setup() 
{
  Serial.begin(115200);
  magel.begin();           //Init Magellan LIB
  payload="{\"sw\":0}";    //Please provide payload with json format
  magel.report(payload);   //Initial switch data to Magellan 
  pinMode(ledPin,OUTPUT);
}

void loop() 
{
  /*
    Example get swich data from Magellan IoT platform for control LED on board
  */
  sw_state=magel.getControl("sw");  //Get switch data from Magellan IoT platform

  if(sw_state.indexOf("40300")==-1) //Response data is not 40300
  {
    sw=sw_state;
  }

  Serial.print("sw ");
  Serial.println(sw);

  if(sw=="1")
  {
    digitalWrite(ledPin,1); //Turn on LED
  }
  else
  {
    digitalWrite(ledPin,0); //Turn off LED
  }
}
