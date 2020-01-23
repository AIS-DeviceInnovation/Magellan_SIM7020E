#include "AIS_NB_SIM7020E.h"
String address = "";    // Your Server IP
String desport = "";    // Your Server Port
String payload = "Hello World";
String data_return;

const long interval = 20000;  //millisecond 
unsigned long previousMillis = 0;

AIS_NB_SIM7020E nb;
void setup() {
    
  Serial.begin(9600);
  nb.setupDevice(address,desport);
  Serial.println(F("-------------BEGIN-------------"));
  Serial.print(F(">>DeviceIP: "));
  Serial.println(nb.getDeviceIP());
  nb.pingIP(address);
  previousMillis = millis();

}

void loop() {
  data_return="";
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval){     
          
      // Send data in String 
      nb.sendMsgSTR(address,desport,payload);
      
      // Send data in HexString     
      //nb.sendMsgHEX(address,desport,payload);
      previousMillis = currentMillis;  
  }
  nb.waitResponse(data_return,address);
  if(data_return!="")Serial.println("# Receive : "+data_return);
}
