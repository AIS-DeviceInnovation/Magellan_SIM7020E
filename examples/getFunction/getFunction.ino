/*
  Supported DEVIO NB-DEVKIT I Board 
    |  Do not use PIN   |
    |      16 TX        |
    |      17 RX        |
    |      4 EINT       |
    |   26 power key    |
    |     27 reset      |
  This is an example for DEVIO NB-DEVKIT I.
  
  Please login and enjoy with https://magellan.ais.co.th
  
  If you have any questions, please see more details at https://www.facebook.com/AISDEVIO
*/
#include "Magellan_SIM7020E.h"
Magellan_SIM7020E magel; 

void setup() {
  Serial.begin(115200);
  magel.begin();           //Init Magellan LIB
}

void loop() {
  Serial.println("---------- Clock ------------"); // Date,Time
  dateTime timeClock = magel.getClock();
  Serial.println("date : "+timeClock.date);
  Serial.println("time : "+timeClock.time);
  Serial.println("------------ End ------------");
  delay(1000);
  Serial.println("---------- Radio ------------"); // Radio Network State
  radio radioStat = magel.getRadioStat();
  Serial.println("pci : "+radioStat.pci);
  Serial.println("rsrq : "+radioStat.rsrq);
  Serial.println("rsrp : "+radioStat.rsrp);
  Serial.println("snr : "+radioStat.snr);
  Serial.println("------------ End ------------");
  delay(1000);
  Serial.println("---------- Signal -----------"); // Signal Quality Report 
  Serial.println("Signal : "+magel.getSignal());
  Serial.println("------------ End ------------");
  delay(1000);
  Serial.println("----------- Ping ------------"); // Test IP Network Connectivity to A Remote Host
  String address  = "8.8.8.8"; 
  pingRESP pingIP = magel.pingIP(address);
  Serial.print("Status : ");
  Serial.println(pingIP.status);
  Serial.println("Addr : "+pingIP.addr);
  Serial.println("ttl : "+pingIP.ttl);
  Serial.println("rtt : "+pingIP.rtt);
  Serial.println("------------ End ------------");
  delay(1000);
}