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

#ifndef AIS_SIM7020E_API_h
#define AIS_SIM7020E_API_h

#include "AT_SIM7020E.h"
#include <Arduino.h>

#define LIBRARY_VERSION "AIS_SIM7020E_API v1.5.1"
#define DEFAULT_QOS 0
#define KEEPALIVE 900
#define DEFAULT_TIMEZONE 7

enum certificateType { ROOT_CERTIFICATE, CLIENT_CERTIFICATE, CLIENT_PRIVATEKEY, PSKID, PSK };

class AIS_SIM7020E_API {
public:
  AIS_SIM7020E_API();
  bool debug;

  void (*Event_debug)(char *data);

  void     begin(String serverdesport = "", String addressI = "", String apn = "");
  String   getDeviceIP();
  String   getSignal();
  String   getIMSI();
  radio    getRadioStat();
  pingRESP pingIP(String IP);
  dateTime getClock(unsigned int timezone = DEFAULT_TIMEZONE);
  void     powerSavingMode(unsigned int psm, String Requested_PeriodicTAU = "",
                           String Requested_Active_Time = "");
  bool     checkNetworkConnection();
  bool     checkPSMmode();
  bool     MQTTstatus();

  void sendMsgHEX(String address, String desport, String payload);
  void sendMsgSTR(String address, String desport, String payload);

  void waitResponse(String &retdata, String server);

  bool   connectMQTT(String server, String port, String clientID, String username = "",
                     String password = "");
  bool   connectMQTT(String server, String port, String clientID, String username, String password,
                     int keepalive, int version, int cleansession, int willflag, String willOption);
  bool   newMQTT(String server, String port);
  bool   newMQTTs(String server, String port);
  bool   sendMQTTconnectionPacket(String clientID, String username, String password, int keepalive,
                                  int version, int cleansession, int willflag, String willOption);
  bool   publish(String topic, String payload, unsigned int PubQoS = DEFAULT_QOS,
                 unsigned int PubRetained = DEFAULT_QOS, unsigned int PubDup = DEFAULT_QOS);
  bool   subscribe(String topic, unsigned int SubQoS = DEFAULT_QOS);
  void   unsubscribe(String topic);
  void   MQTTresponse();
  bool   setCallback(MQTTClientCallback callbackFunc);
  bool   manageSSL(String rootCertificate, String clientCertificate, String clientPrivateKey);
  bool   setPSK(String PSK);
  bool   setPSKID(String PSKID);
  String willConfig(String will_topic, unsigned int will_qos, unsigned int will_retain,
                    String will_msg);
  String toString(String dat);

private:
  unsigned long       startTime         = 0;
  unsigned long       timePassed        = 0;
  const unsigned long timeout_ms        = 5000;
  int                 attemptCount      = 0;
  bool                flag_mqtt_connect = false;
  bool                isMQTTs           = false;

  void send_msg(String address, String desport, unsigned int len, String payload);
  char char_to_byte(char c);
  bool setupMQTT(String server, String port, String clientID, String username, String password,
                 int keepalive, int version, int cleansession, int willflag, String willOption);
  void addNewline(String &str);
  bool setCertificate(certificateType type, String CA);

protected:
  Stream *_Serial;
};

#endif
