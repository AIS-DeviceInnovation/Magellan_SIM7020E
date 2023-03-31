

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

AT Command Dictionary for SIMCOM SIM7020E version 1.5.1
support SIMCOM SIM7020E
NB-IoT with AT command

Author: Device Innovation team
Create Date: 2 January 2020.
Modified: 30 March 2023.
*/
#ifndef AT_SIM7020E_h
#define AT_SIM7020E_h

#include <Arduino.h>
#include <Stream.h>

#define APN "DEVKIT.NB"

struct pingRESP {
  bool   status;
  String addr;
  String ttl;
  String rtt;
};

struct radio {
  String pci  = "";
  String rsrp = "";
  String rsrq = "";
  String snr  = "";
};

struct dateTime {
  String date = "";
  String time = "";
};

enum ConnectionState { IDLE, CONNECTING, CONNECTED, ERROR };

typedef void (*MQTTClientCallback)(String &topic, String &QoS, String &retained, int &msgLen,
                                   String &payload);

class AT_SIM7020E {
public:
  AT_SIM7020E();
  bool debug;
  //==============Parameter=================
  const unsigned int  msgLenMul      = 2;
  const unsigned int  mqttCmdTimeout = 1200;
  const unsigned int  mqttBuffSize   = 1024;
  const unsigned int  MAXTIME        = 60000;
  unsigned long       startTime      = 0;
  unsigned long       timePassed     = 0;
  const unsigned long timeout_ms     = 10000;
  int                 attemptCount   = 0;
  const unsigned int  mqttsCertType  = 0;    // 0 String encoding, 1 HEX Encoding
  //=========Initialization Module=======
  void     setupModule(String port = "", String address = "", String apn = "");
  void     check_module_ready();
  void     reboot_module();
  pingRESP pingIP(String IP);
  void     closeUDPSocket();
  bool     checkNetworkConnection();
  bool     attachNetwork();
  void     powerSavingMode(unsigned int psm, String Requested_PeriodicTAU = "",
                           String Requested_Active_Time = "");
  void     syncLocalTime();
  //==========Get Parameter Value=========
  String   getFirmwareVersion();
  String   getIMEI();
  String   getICCID();
  String   getIMSI();
  String   getDeviceIP();
  String   getSignal();
  String   getAPN();
  void     setAPN(String apn);
  String   getNetworkStatus();
  radio    getRadioStat();
  bool     checkPSMmode();
  bool     MQTTstatus();
  dateTime getClock(unsigned int timezone);
  //==========Data send/rec.===============
  void waitResponse(String &retdata, String server);
  void sendCmd(String address, String port, unsigned int len);
  void sendCmd(String input);
  void sendCmd(unsigned int data);
  void sendCmd(char *);
  void endCmd();
  //===============Utility=================
  void _serial_flush();
  int  waitForResponse(const String &success, const String &error, int timeout);
  //================MQTT===================
  String retTopic;
  String retPayload;
  String retQoS;
  String retRetained;
  void   disconnectMQTT();
  bool   newMQTT(const String &server, const String &port);
  bool   newMQTTs(const String &server, const String &port);
  bool   setCertificate(byte cerType, int cerLength, byte isEnd, String CA);
  bool   verifyCertificateLengths(const int &root_ca_len, const int &client_ca_len,
                                  const int &private_key_len);
  bool   sendMQTTconnectionPacket(String clientID, String username, String password, int keepalive,
                                  int version, int cleansession, int willflag, String willOption);
  void   publish(String topic, String payload, unsigned int qos, unsigned int retained,
                 unsigned int dup);
  bool   subscribe(String topic, unsigned int qos);
  void   unsubscribe(String topic);
  int    MQTTresponse();

  //============ callback ==================
  MQTTClientCallback MQcallback_p;
  bool               setCallback(MQTTClientCallback callbackFunc);

private:
  //==============Buffer====================
  String data_input;
  String socket;
  //==============Flag======================
  bool hw_connected = false;
  bool end          = false;
  //============Counter value===============
  byte k = 0;
  //==============Function==================
  void echoOff();
  bool setPhoneFunction();
  void connectNetwork();
  bool createUDPSocket(String address, String port);
  void manageResponse(String &retdata, String server);
  bool enterPIN();
  void printHEX(char *str);
  void blankChk(String &val);

protected:
  Stream *_Serial;
};
#endif