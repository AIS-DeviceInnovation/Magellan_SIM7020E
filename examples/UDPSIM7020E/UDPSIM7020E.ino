 /*
 Supported DEVIO NB-DEVKIT I Board 
    |  Do not use PIN   |
    |      16 TX        |
    |      17 RX        |
    |      4 EINT       |
    |   26 power key    |
    |     27 reset      |
*/

#include "AIS_SIM7020E_API.h"
String address      = "";    // Your Server IP
String serverPort   = "";    // Your Server Port
String payload = "HelloWorld!";
String data_return;

const long interval = 20000;  //millisecond 
unsigned long previousMillis = 0;
int cnt=0;

AIS_SIM7020E_API nb;
void setup() {
    
  Serial.begin(115200);
  nb.begin(address,serverPort);
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
      cnt++;    
      // Send data in String 
      nb.sendMsgSTR(address,serverPort,payload+String(cnt));
      
      // Send data in HexString     
      //nb.sendMsgHEX(address,serverPort,payload+String(cnt));
      previousMillis = currentMillis;  
  }
  nb.waitResponse(data_return,address);
  if(data_return!="")Serial.println("# Receive : "+data_return);
}
