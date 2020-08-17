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
String address    = "";               //Your IPaddress or mqtt server url
String serverPort = "";               //Your server port
String clientID   = "";               //Your client id < 120 characters
String topic      = "";               //Your topic     < 128 characters
String payload    = "HelloWorld!";    //Your payload   < 500 characters
String username   = "";               //username for mqtt server, username <= 100 characters
String password   = "";               //password for mqtt server, password <= 100 characters 
int keepalive     = 60;               //keepalive time (second)
int version       = 3;                //MQTT veresion 3(3.1), 4(3.1.1)
int cleansession  = 1;                //cleanssion : 0, 1
int willflag      = 1;                //willflag : 0, 1
unsigned int subQoS       = 0;
unsigned int pubQoS       = 0;
unsigned int will_qos     = 0;
unsigned int will_retain  = 0;
unsigned int pubRetained  = 0;
unsigned int pubDuplicate = 0;

const long interval = 5000;           //time in millisecond 
unsigned long previousMillis = 0;

AIS_SIM7020E_API nb;
  /*  This part is for setupAdvanceMQTT.
   *  parameter for will option (Last will and Testament)
   *  will_topic : String
   *  will_qos   : unsinged int : 0, 1, 2
   *  will_retain: unsinged int : 0, 1
   *  will_msg   : String */
String willOption = nb.willConfig("will_topic",will_qos,will_retain,"will_msg");
int cnt = 0;

void setup() {
  Serial.begin(115200);
  nb.begin();
  setupMQTT();
  nb.setCallback(callback); 
  previousMillis = millis();                
}
void loop() {   
  nb.MQTTresponse();
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
        cnt++;
        connectStatus();
        nb.publish(topic, payload+String(cnt), pubQoS, pubRetained, pubDuplicate);      //QoS = 0, 1, or 2, retained = 0 or 1, dup = 0 or 1
        previousMillis = currentMillis;  
  }
}

//=========== MQTT Function ================
void setupMQTT(){
  /*  parameter for setup advance MQTT
   *  version    : MQTT veresion 3(3.1), 4(3.1.1)
   *  cleanssion : 0, 1
   *  willflag   : 0, 1
   *  **PLEASE ADD PARAMETER IN willOption** */
   if(!nb.connectAdvanceMQTT(address,serverPort,clientID,username,password,keepalive,version,cleansession,willflag,willOption)){
     Serial.println("\nconnectMQTT");
    }
    nb.subscribe(topic,subQoS);
//    nb.unsubscribe(topic); 
}
void connectStatus(){
    if(!nb.MQTTstatus()){
        if(!nb.NBstatus()){
           Serial.println("reconnectNB ");
           nb.begin();
        }
       Serial.println("reconnectMQ ");
       setupMQTT();
    }   
}
void callback(String &topic,String &payload, String &QoS,String &retained){
  Serial.println("-------------------------------");
  Serial.println("# Message from Topic \""+topic+"\" : "+nb.toString(payload));
  Serial.println("# QoS = "+QoS);
  if(retained.indexOf(F("1"))!=-1){
    Serial.println("# Retained = "+retained);
  }
}
