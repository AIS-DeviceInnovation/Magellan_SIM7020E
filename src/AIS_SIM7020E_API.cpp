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

AIS_SIM7020E_API v1.5.1 NB-IoT.
support SIMCOM SIM7020E
NB-IoT with AT command

Author: Device Innovation team
Create Date: 2 January 2020.
Modified: 30 March 2023.

Released for private usage.
*/

#include "AIS_SIM7020E_API.h"

AT_SIM7020E atcmd;
void        event_null(char *data) {}

/****************************************/
/**          Initialization            **/
/****************************************/

AIS_SIM7020E_API::AIS_SIM7020E_API() {
  Event_debug = event_null;
}

void AIS_SIM7020E_API::begin(String addressI, String serverdesport, String apn) {
  atcmd.debug = debug;
  Serial.print(F("               "));
  Serial.println(LIBRARY_VERSION);
  atcmd.setupModule(addressI, serverdesport, apn);
}

pingRESP AIS_SIM7020E_API::pingIP(String IP) {
  return atcmd.pingIP(IP);
}

/****************************************/
/**          Send UDP Message          **/
/****************************************/
/*
  - sendMsgHEX
      - Send message in HEX string. Limit of is 1024.
  - sendMsgSTR
      - Send message in ASCII string. Limit of is 512.
  - send_msg
      - Send message to the server.
*/
void AIS_SIM7020E_API::sendMsgHEX(String address, String desport, String payload) {
  if (payload.length() > 1024) {
    Serial.println(F("Warning payload size exceed the limit. [Limit of HEX is 1024]"));
  } else
    send_msg(address, desport, payload.length(), payload);
}

void AIS_SIM7020E_API::sendMsgSTR(String address, String desport, String payload) {
  if (payload.length() > 512) {
    Serial.println(F("Warning payload size exceed the limit. [Limit of String is 512]"));
  } else
    send_msg(address, desport, 0, "\"" + payload + "\"");
}

void AIS_SIM7020E_API::send_msg(String address, String desport, unsigned int len, String payload) {
  Serial.println(F("-------------------------------"));
  Serial.print(F("# Sending Data : "));
  Serial.println(payload);
  Serial.print(F("# IP : "));
  Serial.println(address);
  Serial.print(F("# Port : "));
  Serial.println(desport);

  atcmd.sendCmd(address, desport, len);
  atcmd.sendCmd(payload);
  atcmd.endCmd();
}

/****************************************/
/**         Receive UDP Message        **/
/****************************************/

void AIS_SIM7020E_API::waitResponse(String &retdata, String server) {
  atcmd.waitResponse(retdata, server);
}

/****************************************/
/**          Get Parameter Value       **/
/****************************************/
/*
  - getSignal
      - Get NB-IoT signal
  - getRadioStat
      - Get radio information for troubleshooting.
  - getDeviceIP
      - Get device ip after connected to network.
  - getIMSI
      - Get IMSI from eSim on board.
  - powerSavingMode
      - Set powerSavingMode : 0 turn off, 1 turn on.
  - checkPSMmode
      - Check if powerSavingMode is enable or not.
  - NBstatus
      - Check NB connecting status.
  - MQTTstatus
      - Check MQTT connecting status.
*/
String AIS_SIM7020E_API::getSignal() {
  return atcmd.getSignal();
}

radio AIS_SIM7020E_API::getRadioStat() {
  return atcmd.getRadioStat();
}

String AIS_SIM7020E_API::getDeviceIP() {
  return atcmd.getDeviceIP();
}

String AIS_SIM7020E_API::getIMSI() {
  return atcmd.getIMSI();
}

void AIS_SIM7020E_API::powerSavingMode(unsigned int psm, String Requested_PeriodicTAU,
                                       String Requested_Active_Time) {
  atcmd.powerSavingMode(psm, Requested_PeriodicTAU, Requested_Active_Time);
}

bool AIS_SIM7020E_API::checkPSMmode() {
  return atcmd.checkPSMmode();
}

bool AIS_SIM7020E_API::checkNetworkConnection() {
  return atcmd.checkNetworkConnection();
}

bool AIS_SIM7020E_API::MQTTstatus() {
  return atcmd.MQTTstatus();
}

dateTime AIS_SIM7020E_API::getClock(unsigned int timezone) {
  return atcmd.getClock(timezone);
}

/****************************************/
/**              MQTT(s)               **/
/****************************************/
/*
  - setupMQTT
      - setup module to use MQTT include serverIP, port, clientID, username,
  password, keep alive interval, will message.
  - connectMQTT
      - setup module to use MQTT include serverIP, port, clientID, username,
  password
  - connectMQTT
      - setup module to use MQTT include serverIP, port, clientID, username,
  password, keep alive interval, will message.
      - connect device to MQTT server and port
  - sendMQTTconnectionPacket
      - connect device to MQTT with configuration value
  - willConfig
      - create payload for will message
  - publish
      - publish payload within  1000 characters.
  - subscribe
      - subscribe to the topic to receive data
  - unsubscribe
      - unsubscribe to the topic
  - MQTTresponse
      - receive response from server
  - RegisMQCallback
      - receive response from server
*/
bool AIS_SIM7020E_API::setupMQTT(String server, String port, String clientID, String username,
                                 String password, int keepalive, int version, int cleansession,
                                 int willflag, String willOption) {
  if (username.length() > 100 || password.length() > 100) {
    Serial.println(F("Username/Password is over 100."));
  } else if (username == "" && password != "") {
    Serial.println(F("Username is missing."));
  } else if (clientID.length() > 120 || clientID == "" && cleansession != 1) {
    Serial.println(F("ClientID is over 120 or ClientID is missing."));
  } else if (server == "" || port == "") {
    Serial.println(F("Address or port is missing."));
  } else if (version > 4 || version < 3) {
    Serial.println(F("Version must be 3 (MQTT 3.1) or 4 (MQTT 3.1.1)"));
  } else if (willflag == 1 && willOption == "") {
    Serial.println(F("Missing will option."));
  } else {
    if (MQTTstatus()) {
      atcmd.disconnectMQTT();
      delay(200);
    }
    if (isMQTTs) {
      Serial.print("# Connecting to MQTT server over SSL/TLS...");
      flag_mqtt_connect =
          newMQTTs(server, port) &&
          atcmd.sendMQTTconnectionPacket(clientID, username, password, keepalive, version,
                                         cleansession, willflag, willOption);
    } else {
      Serial.print("# Connecting to MQTT server...");
      flag_mqtt_connect = newMQTT(server, port) && atcmd.sendMQTTconnectionPacket(
                                                       clientID, username, password, keepalive,
                                                       version, cleansession, willflag, willOption);
    }
    if (flag_mqtt_connect) {
      Serial.println("Done");
      Serial.print(F("# ServerIP : "));
      Serial.println(server);
      Serial.print(F("# Port : "));
      Serial.println(port);
      Serial.print(F("# ClientID : "));
      Serial.println(clientID);
    } else {
      Serial.println("Failed");
      Serial.println(F("Please check your parameter again."));
      Serial.println(F("Restart Board..."));
      ESP.restart();
    }
  }
  atcmd._serial_flush();
  return flag_mqtt_connect;
}

bool AIS_SIM7020E_API::newMQTT(String server, String port) {
  return atcmd.newMQTT(server, port);
}

bool AIS_SIM7020E_API::newMQTTs(String server, String port) {
  return atcmd.newMQTTs(server, port);
}

bool AIS_SIM7020E_API::connectMQTT(String server, String port, String clientID, String username,
                                   String password) {
  return setupMQTT(server, port, clientID, username, password, KEEPALIVE, 3, 1, 0, "");
}

bool AIS_SIM7020E_API::connectMQTT(String server, String port, String clientID, String username,
                                   String password, int keepalive, int version, int cleansession,
                                   int willflag, String willOption) {
  return setupMQTT(server, port, clientID, username, password, keepalive, version, cleansession,
                   willflag, willOption);
}

bool AIS_SIM7020E_API::sendMQTTconnectionPacket(String clientID, String username, String password,
                                                int keepalive, int version, int cleansession,
                                                int willflag, String willOption) {
  return atcmd.sendMQTTconnectionPacket(clientID, username, password, keepalive, version,
                                        cleansession, willflag, willOption);
}

String AIS_SIM7020E_API::willConfig(String will_topic, unsigned int will_qos,
                                    unsigned int will_retain, String will_msg) {
  char data[will_msg.length() + 1];
  memset(data, '\0', will_msg.length());
  will_msg.toCharArray(data, will_msg.length() + 1);
  int    len = will_msg.length() * atcmd.msgLenMul;
  String msg;

  char *hstr;
  hstr = data;
  char out[3];
  memset(out, '\0', 2);
  bool flag = false;
  while (*hstr) {
    flag = itoa((int)*hstr, out, 16);
    if (flag) {
      msg += out;
    }
    hstr++;
  }
  return "\"topic=" + will_topic + ",QoS=" + String(will_qos) + ",retained=" + String(will_retain) +
         ",message_len=" + String(len) + ",message=" + msg + "\"";
}

bool AIS_SIM7020E_API::publish(String topic, String payload, unsigned int pubQoS,
                               unsigned int pubRetained, unsigned int pubDup) {
  if (topic == "") {
    Serial.println(F("Topic is missing."));
    return false;
  }
  if (payload.length() * atcmd.msgLenMul > 1000) {
    Serial.println(F("Payload hex string is over 1000."));
    return false;
  }

  Serial.println(F("-------------------------------"));
  Serial.print(F("# Publish : "));
  Serial.println(payload);
  Serial.print(F("# Topic   : "));
  Serial.println(topic);

  atcmd.publish(topic, payload, pubQoS, pubRetained, pubDup);
  while (true) {
    int respCode = atcmd.MQTTresponse();

    if (!respCode) {
      return false;
    } else if (respCode == 1) {
      return true;
    }
  }
}

bool AIS_SIM7020E_API::subscribe(String topic, unsigned int subQoS) {
  if (flag_mqtt_connect) {
    if (topic == "") {
      Serial.println(F("Topic is missing."));
      return false;
    }

    atcmd._serial_flush();
    startTime = millis();
    while (millis() - startTime < timeout_ms) {
      if (atcmd.subscribe(topic, subQoS)) {
        Serial.println(F("-------------------------------"));
        Serial.println(F("# Subscribe "));
        Serial.print(F("# Topic : "));
        Serial.println(topic);
        return true;
      }
      delay(20);
    }
    atcmd._serial_flush();
  }
  return false;
}

void AIS_SIM7020E_API::unsubscribe(String topic) {
  if (topic == "") {
    Serial.println(F("Topic is missing."));
  } else {
    atcmd.unsubscribe(topic);
  }
}

void AIS_SIM7020E_API::MQTTresponse() {
  atcmd.MQTTresponse();
}

bool AIS_SIM7020E_API::setCallback(MQTTClientCallback callbackFunc) {
  return atcmd.setCallback(callbackFunc);
}

bool AIS_SIM7020E_API::manageSSL(String root_cert, String client_cert, String client_key) {
  Serial.print("# Certificate Setup (Please wait 1-2 minutes)...");
  isMQTTs = true;
  if (atcmd.verifyCertificateLengths(root_cert.length() + 2, client_cert.length() + 2,
                                     client_key.length() + 2)) {
    Serial.println("Success");
    return true;
  }
  if (!setCertificate(ROOT_CERTIFICATE, root_cert)) {
    Serial.println("Failed to set root certificate");
    return false;
  }

  if (!setCertificate(CLIENT_CERTIFICATE, client_cert)) {
    Serial.println("Failed to set client certificate");
    return false;
  }

  if (!setCertificate(CLIENT_PRIVATEKEY, client_key)) {
    Serial.println("Failed to set client private key");
    return false;
  }

  if (atcmd.verifyCertificateLengths(root_cert.length() + 2, client_cert.length() + 2,
                                     client_key.length() + 2)) {
    Serial.println("Success");
    return true;
  } else {
    Serial.println("# Failed to verify certificate lengths");
    return false;
  }
}

bool AIS_SIM7020E_API::setCertificate(certificateType type, String CA) {
  addNewline(CA);
  int cerLength = CA.length();
  int index     = 0;
  int notEnd    = 1;

  while (index < cerLength) {
    int chunkLength = min(cerLength - index, 1000);
    if (cerLength - index < 1000) {
      notEnd = 0;
    }
    String chunk = CA.substring(index, index + chunkLength);
    if (!atcmd.setCertificate(type, cerLength, notEnd, chunk)) {
      return false;
    }
    index += chunkLength;
    delay(100);
  }
  return true;
}

bool AIS_SIM7020E_API::setPSK(String psk) {
  return setCertificate(PSK, psk);
}

bool AIS_SIM7020E_API::setPSKID(String pskID) {
  return setCertificate(PSKID, pskID);
}

/****************************************/
/**               Utility              **/
/****************************************/
/*
  - toString
      - change hex to string
  - char_to_byte
      - use in function toString
  - addNewline
      - add \r\n in certificate, use in MQTTs
*/
String AIS_SIM7020E_API::toString(String dat) {
  String str = "";
  for (int x = 0; x < dat.length(); x += 2) {
    char c = char_to_byte(dat[x]) << 4 | char_to_byte(dat[x + 1]);
    str += c;
  }
  return (str);
}

char AIS_SIM7020E_API::char_to_byte(char c) {
  if ((c >= '0') && (c <= '9')) {
    return (c - 0x30);
  }
  if ((c >= 'A') && (c <= 'F')) {
    return (c - 55);
  } else
    return c;
}

void AIS_SIM7020E_API::addNewline(String &str) {
  if (str.indexOf(F("\r\n")) == -1) {
    byte index  = str.indexOf("-----");
    byte index2 = str.indexOf(("-----"), index + 1);
    str         = str.substring(0, index2 + 5) + "\\r\\n" + str.substring(index2 + 5, str.length());
  }
}
