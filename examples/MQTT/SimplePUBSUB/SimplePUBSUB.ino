/*
 Supported DEVIO NB-DEVKIT I Board
    |  Do not use PIN   |
    |      16 TX        |
    |      17 RX        |
    |      4 EINT       |
    |   26 power key    |
    |     27 reset      |

    If you have any questions, please contact us on https://www.facebook.com/AISDEVIO
*/
#include "AIS_SIM7020E_API.h"
AIS_SIM7020E_API nb;

String       host         = "";               // Your IPaddress or mqtt server url
String       port         = "";               // Your server port
String       clientID     = "";               // Your client id < 120 characters
String       topic        = "";               // Your topic     < 128 characters
String       payload      = "HelloWorld!";    // Your payload   < 500 characters
String       username     = "";    // username for mqtt server, username <= 100 characters
String       password     = "";    // password for mqtt server, password <= 100 characters
unsigned int subQoS       = 0;     // QoS = 0, 1, or 2
unsigned int pubQoS       = 0;     // QoS = 0, 1, or 2
unsigned int pubRetained  = 0;     // retained = 0 or 1
unsigned int pubDuplicate = 0;     // duplicate = 0 or 1

const long    interval       = 10000;    // time in millisecond
unsigned long previousMillis = 0;
int           cnt            = 0;

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
    nb.publish(topic, payload + String(cnt));
    //      nb.publish(topic, payload, pubQoS, pubRetained, pubDuplicate);      //QoS = 0, 1, or 2,
    //      retained = 0 or 1, dup = 0 or 1
    previousMillis = currentMillis;
  }
}

//=========== MQTT Function ================
void setupMQTT() {
  if (nb.connectMQTT(host, port, clientID, username, password)) {
    nb.subscribe(topic, subQoS);
  }

  //  nb.unsubscribe(topic);
}
void connectStatus() {
  if (!nb.MQTTstatus()) {
    if (!nb.checkNetworkConnection()) {
      Serial.println("reconnectNB ");
      nb.begin();
    }
    Serial.println("reconnectMQ ");
    setupMQTT();
  }
}
void callback(String &topic, String &QoS, String &retained, int &msgLen,
                                   String &payload) {
  Serial.println("-------------------------------");
  Serial.println("# Message from Topic \"" + topic + "\" : " + nb.toString(payload));
  Serial.println("# QoS = " + QoS);
  if (retained.indexOf(F("1")) != -1) {
    Serial.println("# Retained = " + retained);
  }
}
