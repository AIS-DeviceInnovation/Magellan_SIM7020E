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

#include "AT_SIM7020E.h"
#include "board.h"

/****************************************/
/**        Initialization Module       **/
/****************************************/
AT_SIM7020E::AT_SIM7020E() {}

void AT_SIM7020E::setupModule(String address, String port, String apn) {

  startTime = millis();

  // Set up hardware reset pin
  pinMode(hwResetPin, OUTPUT);

  // Reboot the module
  Serial.print(F("# Rebooting ."));
  reboot_module();
  Serial.println(F("..Done"));

  // Set up serial port
  if (serialConfig) {
    serialPort.begin(buadrate, configParam, rxPin, txPin);
    _Serial = &serialPort;
  } else {
    serialPort.begin(buadrate);
    _Serial = &serialPort;
  }

  // Check module readiness and configure settings
  Serial.print(F("# Setup "));
  check_module_ready();
  _Serial->println("AT+CMEE=1");    // set report error
  setAPN(apn);
  syncLocalTime();    // sync local time
  _serial_flush();
  Serial.println(F("...Done"));

  // Print module information
  Serial.print(F("# ICCID  : "));
  Serial.println(getICCID());
  Serial.print(F("# IMSI   : "));
  Serial.println(getIMSI());
  Serial.print(F("# IMEI   : "));
  Serial.println(getIMEI());
  Serial.print(F("# FW ver : "));
  Serial.println(getFirmwareVersion());

  // Print PSM mode if debug flag is set
  if (debug) {
    Serial.print(F("# PSM mode : "));
    Serial.println(checkPSMmode());
  }

  // Print signal strength
  delay(500);
  Serial.print(F("# Signal : "));
  Serial.print(getSignal());
  Serial.println(F(" dBm"));
  _serial_flush();

  // Attempt to attach to network and create UDP socket
  Serial.print(F("# Connecting..."));
  if (attachNetwork()) {
    if (address != "" && port != "") {
      if (!createUDPSocket(address, port)) {
        Serial.println(F("# Cannot create socket"));
      }
    }
    Serial.println(F("Done"));
    Serial.print(F("# APN   : "));
    Serial.println(getAPN());
    _serial_flush();
    Serial.println(F("-------------------------------"));
  } else {
    // Network attachment failed, restart
    Serial.println(F("Failed"));
    Serial.println(F("-------------------------------"));
    ESP.restart();
  }
}

void AT_SIM7020E::check_module_ready() {
  _Serial->println(F("AT"));
  while (true) {
    if (_Serial->available()) {
      data_input = _Serial->readStringUntil('\n');
      if (data_input.indexOf(F("OK")) != -1) {
        hw_connected = true;
        echoOff();
        powerSavingMode(2);
        break;
      }
    } else {

      if (millis() - startTime > MAXTIME) {
        startTime    = millis();
        hw_connected = false;
        attemptCount++;
        if (attemptCount > 5) {
          Serial.print(F("\nError to connect NB Module, rebooting..."));
          delay(200);
          ESP.restart();
        }

      } else {
        _Serial->println(F("AT"));
        delay(200);
      }
    }
  }
  delay(1000);
  attemptCount = 0;
}

//  Reboot module with hardware pin.
void AT_SIM7020E::reboot_module() {
  digitalWrite(hwResetPin, LOW);
  delay(1000);
  digitalWrite(hwResetPin, HIGH);
  delay(2000);
}

bool AT_SIM7020E::attachNetwork() {
  ConnectionState state = IDLE;
  startTime             = millis();
  while (millis() - startTime < timeout_ms) {
    switch (state) {
    case IDLE:
      connectNetwork();
      state = CONNECTING;
      break;
    case CONNECTING:
      if (checkNetworkConnection()) {
        state = CONNECTED;
      }
      break;
    case CONNECTED:
      _serial_flush();
      _Serial->flush();
      return true;
    case ERROR:
      return false;
    }
  }
  return false;
}

// Check network connecting status : 1 connected, 0 not connected
bool AT_SIM7020E::checkNetworkConnection() {
  bool networkStatus = false;
  _serial_flush();
  _Serial->println(F("AT+CGATT?"));
  delay(800);
  startTime = millis();
  while (millis() - startTime < timeout_ms) {
    if (_Serial->available()) {
      data_input += _Serial->readStringUntil('\n');
      if (data_input.indexOf(F("+CGATT: 1")) != -1 && data_input.indexOf(F("OK")) != -1) {
        networkStatus = true;
        break;
      } else if (data_input.indexOf(F("+CGATT: 0")) != -1 || data_input.indexOf(F("ERROR")) != -1) {
        networkStatus = false;
        break;
      }
    }
  }
  data_input = "";
  return networkStatus;
}

// Set Phone Functionality : 1 Full functionality
bool AT_SIM7020E::setPhoneFunction() {
  bool status = false;
  _Serial->println(F("AT+CFUN=1"));
  startTime = millis();
  while (millis() - startTime < timeout_ms) {
    if (_Serial->available()) {
      data_input = _Serial->readStringUntil('\n');
      if (data_input.indexOf(F("OK")) != -1) {
        status = true;
        break;
      } else if (data_input.indexOf(F("ERROR")) != -1) {
        status = false;
        break;
      }
    }
  }
  Serial.print(F("."));
  return status;
}

// Attach network : 1 connected, 0 disconnected
void AT_SIM7020E::connectNetwork() {
  _Serial->print(F("AT+CGATT=1"));
  _Serial->println();
  for (int i = 0; i < 30; i++) {
    if (_Serial->available()) {
      data_input = _Serial->readStringUntil('\n');
      if (data_input.indexOf(F("OK")) != -1)
        break;
      else if (data_input.indexOf(F("ERROR")) != -1)
        break;
    }
  }
  Serial.print(F("."));
}

// Create a UDP socket and connect socket to remote address and port
bool AT_SIM7020E::createUDPSocket(String address, String port) {
  bool status = false;
  _Serial->print(F("AT+CSOC=1,2,1"));
  _Serial->println();
  delay(200);
  startTime = millis();
  while (millis() - startTime < timeout_ms) {
    if (_Serial->available()) {
      data_input = _Serial->readStringUntil('\n');
      if (data_input.indexOf(F("OK")) != -1) {
      } else if (data_input.indexOf(F("+CSOC: 0")) != -1) {
        status = true;
        break;
      } else if (data_input.indexOf(F("+CSOC: 1")) != -1) {
        status = false;
        closeUDPSocket();
        _Serial->println(F("AT+CSOC=1,2,1"));
      }
    }
  }

  if (status) {
    _Serial->print(F("AT+CSOCON=0,"));
    _Serial->print(port);
    _Serial->print(F(","));
    _Serial->print(address);
    _Serial->println();
    startTime = millis();
    while (millis() - startTime < timeout_ms) {
      if (_Serial->available()) {
        data_input = _Serial->readStringUntil('\n');
        if (data_input.indexOf(F("OK")) != -1) {
          break;
        } else if (data_input.indexOf(F("ERROR")) != -1) {
          status = false;
          break;
        }
      }
    }
  }
  return status;
}

// Close a UDP socket 0
void AT_SIM7020E::closeUDPSocket() {
  _Serial->print(F("AT+CSOCL=0"));
  _Serial->println();
  startTime = millis();
  while (millis() - startTime < timeout_ms) {
    if (_Serial->available()) {
      data_input = _Serial->readStringUntil('\n');
      if (data_input.indexOf(F("OK")) != -1) {
        break;
      }
    }
  }
}

// Set command echo mode off
void AT_SIM7020E::echoOff() {
  _Serial->println(F("ATE0"));
  startTime = millis();
  while (millis() - startTime < timeout_ms) {
    if (_Serial->available()) {
      data_input = _Serial->readStringUntil('\n');
      if (data_input.indexOf(F("OK")) != -1)
        break;
    }
  }
}

// Ping IP
pingRESP AT_SIM7020E::pingIP(String IP) {
  pingRESP pingr;
  String   data      = "";
  int      replytime = 0;
  int      ttl       = 0;
  _Serial->println("AT+CIPPING=" + IP + ",1");
  startTime = millis();
  while (millis() - startTime < timeout_ms) {
    if (_Serial->available()) {
      data_input = _Serial->readStringUntil('\n');
      if (data_input.indexOf(F("ERROR")) != -1) {
        break;
      } else if (data_input.indexOf(F("+CIPPING: ")) != -1) {
        data = data_input;

        byte index  = data.indexOf(F(","));
        byte index2 = data.indexOf(F(","), index + 1);
        byte index3 = data.indexOf(F(","), index2 + 1);

        pingr.addr = data.substring(index + 1, index2);
        replytime += data.substring(index2 + 1, index3).toInt();

        ttl += data.substring(index3 + 1, data.length()).toInt();
        break;
      }
    }
  }

  if (data != "") {
    pingr.ttl = String(ttl);
    pingr.rtt = String(replytime * 100);
    blankChk(pingr.ttl);
    blankChk(pingr.rtt);
    Serial.println(">>Ping IP : " + pingr.addr + ", ttl= " + pingr.ttl +
                   ", replyTime= " + pingr.rtt + "ms");
    pingr.status = true;
  } else {
    Serial.println(F(">>Ping Failed"));
    pingr.status = false;
  }
  _serial_flush();
  data_input = "";

  return pingr;
}

// Set powerSavingMode :
// 0 Disable the use of PSM
// 1 Enable the use of PSM
// 2 Disable the use of PSM and discard all parameters for PSM or, if available reset to the
// manufacturer specific default values.
void AT_SIM7020E::powerSavingMode(unsigned int psm, String Requested_PeriodicTAU,
                                  String Requested_Active_Time) {
  // AT+CPSMS=<mode>[,<Requested_Periodic-RAU>[,<Requested_GPRSREADYtimer>[,<Requested_PeriodicTAU>
  // [,<Requested_Active-Time>]]]]
  _Serial->print("AT+CPSMS=");
  _Serial->print(psm);
  if (Requested_PeriodicTAU != "" || Requested_Active_Time != "") {
    _Serial->print(",,,");
    _Serial->print(Requested_PeriodicTAU);
    _Serial->print(",");
    _Serial->print(Requested_Active_Time);
  }
  _Serial->println();
  _serial_flush();
}

// Check if SIM/eSIM need PIN or not.
bool AT_SIM7020E::enterPIN() {
  bool status = false;
  _Serial->println(F("AT+CPIN?"));
  startTime = millis();
  while (millis() - startTime < timeout_ms) {
    if (_Serial->available()) {
      data_input = _Serial->readStringUntil('\n');
      if (data_input.indexOf(F("+CPIN:")) != -1) {
        if (data_input.indexOf(F("READY")) != -1) {
          status = true;
          break;
        }
      }
      if (data_input.indexOf(F("OK")) != -1)
        break;
    }
  }
  _serial_flush();
  return status;
}

/****************************************/
/**          Get Parameter Value       **/
/****************************************/
String AT_SIM7020E::getIMSI() {
  String imsi = "";
  _Serial->println(F("AT+CIMI"));
  startTime = millis();
  while (millis() - startTime < timeout_ms) {
    if (_Serial->available()) {
      data_input = _Serial->readStringUntil('\n');
      if (data_input.indexOf(F("OK")) != -1 && imsi.indexOf(F("52003")) != -1) {
        imsi.replace(F("OK"), "");
        return imsi;
      } else if (data_input.indexOf(F("ERROR")) != -1 || data_input.indexOf(F("+CLTS:")) != -1) {
        setPhoneFunction();
        _Serial->println(F("AT+CIMI"));
      } else
        imsi += data_input;
    }
  }
  return "N/A";
}

String AT_SIM7020E::getICCID() {
  String iccid = "";
  _serial_flush();
  _Serial->println(F("AT+CCID"));
  delay(50);
  startTime = millis();
  while (millis() - startTime < timeout_ms) {
    if (_Serial->available()) {
      data_input += _Serial->readStringUntil('\n');
      if (data_input.indexOf(F("OK")) != -1 && data_input.indexOf(F("896")) != -1) {
        iccid = data_input;
        break;
      }
    }
  }
  iccid.replace(F("OK"), "");
  iccid.trim();
  blankChk(iccid);
  return iccid;
}

String AT_SIM7020E::getIMEI() {
  String imei;
  _Serial->print(F("AT+CGSN=1"));
  _Serial->println();
  startTime = millis();
  while (millis() - startTime < timeout_ms) {
    if (_Serial->available()) {
      data_input = _Serial->readStringUntil('\n');
      if (data_input.indexOf(F("+CGSN:")) != -1) {
        data_input.replace(F("+CGSN: "), "");
        imei = data_input;
      } else if (data_input.indexOf(F("OK")) != -1 && imei != "")
        break;
    }
  }

  blankChk(imei);
  _serial_flush();
  return imei;
}

String AT_SIM7020E::getDeviceIP() {
  _serial_flush();
  String deviceIP;
  _Serial->println(F("AT+CGPADDR=1"));
  bool chk  = false;
  startTime = millis();
  while (millis() - startTime < timeout_ms) {
    if (_Serial->available()) {
      data_input = _Serial->readStringUntil('\n');
      if (data_input.indexOf(F("+CGPADDR")) != -1) {
        chk         = true;
        byte index  = data_input.indexOf(F(":"));
        byte index2 = data_input.indexOf(F(","));
        deviceIP    = data_input.substring(index2 + 1, data_input.length());
      } else if (data_input.indexOf(F("OK")) != -1 && chk) {
        break;
      }
    }
  }
  deviceIP.replace(F("\""), "");
  deviceIP.trim();

  blankChk(deviceIP);
  data_input = "";
  return deviceIP;
}

String AT_SIM7020E::getSignal() {
  _serial_flush();
  int    rssi           = 0;
  int    count          = 0;
  String signalStrength = "";
  data_input            = "";
  do {
    _Serial->println(F("AT+CSQ"));
    delay(800);
    while (true) {
      if (_Serial->available()) {
        data_input = _Serial->readStringUntil('\n');
        if (data_input.indexOf(F("OK")) != -1) {
          break;
        } else {
          if (data_input.indexOf(F("+CSQ")) != -1) {
            byte start_index = data_input.indexOf(F(":"));
            byte stop_index  = data_input.indexOf(F(","));
            signalStrength   = data_input.substring(start_index + 1, stop_index);

            rssi           = signalStrength.toInt();
            rssi           = (2 * rssi) - 113;
            signalStrength = String(rssi);
          }
        }
      }
    }
    if (rssi == -113)
      count++;

  } while (rssi == -113 && count <= 10 || rssi == 85 && count <= 10);
  if (rssi == -113 || rssi == 85) {
    signalStrength = "-113";
    count          = 0;
  }
  return signalStrength;
}

String AT_SIM7020E::getAPN() {
  String out = "";
  _Serial->println(F("AT*MCGDEFCONT?"));
  startTime = millis();
  while (millis() - startTime < timeout_ms) {
    if (_Serial->available()) {
      data_input += _Serial->readStringUntil('\n');
      if (data_input.indexOf(F("*MCGDEFCONT:")) != -1 && data_input.indexOf("OK") != -1) {
        data_input.replace("OK", "");
        data_input.replace("\"", "");
        byte index = data_input.indexOf(F(","));
        out        = data_input.substring(index + 1, data_input.length() - 1);
        if (out == ",,") {
          out = "";
        }
        break;
      }
    }
  }

  blankChk(out);
  _serial_flush();
  return out;
}

String AT_SIM7020E::getFirmwareVersion() {
  String fw = "";
  _Serial->println(F("AT+CGMR"));
  startTime = millis();
  while (millis() - startTime < timeout_ms) {
    if (_Serial->available()) {
      data_input += _Serial->readStringUntil('\n');
      if (data_input.indexOf(F("OK")) != -1) {
        fw = data_input;
        break;
      }
    }
  }
  fw.replace(F("OK"), "");
  fw.trim();
  blankChk(fw);
  return fw;
}

String AT_SIM7020E::getNetworkStatus() {
  String out   = "";
  String data  = "";
  byte   count = 0;

  _Serial->println(F("AT+CEREG=2"));
  delay(200);
  _serial_flush();
  delay(500);
  _Serial->println(F("AT+CEREG?"));

  startTime = millis();
  while (millis() - startTime < timeout_ms) {
    if (_Serial->available()) {
      data_input = _Serial->readStringUntil('\n');
      if (data_input.indexOf(F("+CEREG")) != -1) {
        count++;
        if (count < 10 && data_input.indexOf(F(",2")) != -1) {
          _serial_flush();
          _Serial->println(F("AT+CEREG?"));
        } else {
          data        = data_input;
          byte index  = data.indexOf(F(": "));
          byte index2 = data.indexOf(F(","));
          byte index3 = data.indexOf(F(","), index2 + 1);
          out         = data.substring(index2 + 1, index2 + 2);
          if (out == F("1"))
            out = F("Registered");
          else if (out == "0")
            out = F("Not Registered");
          else if (out == "2")
            out = F("Trying");
        }
      } else if (data_input.indexOf(F("OK")) != -1)
        break;
    }
  }
  blankChk(out);
  return (out);
}

// Get radio stat.
radio AT_SIM7020E::getRadioStat() {
  _serial_flush();
  radio  value;
  String out = "";
  _Serial->println(F("AT+CENG?"));
  startTime = millis();
  while (millis() - startTime < timeout_ms) {
    if (_Serial->available()) {
      data_input = _Serial->readStringUntil('\n');
      if (data_input.indexOf(F("+CENG:")) != -1) {
        byte index  = data_input.indexOf(F(","));
        byte index2 = data_input.indexOf(F(","), index + 1);
        byte index3 = data_input.indexOf(F(","), index2 + 1);
        value.pci   = data_input.substring(index2 + 1, index3);

        index      = data_input.indexOf(F(","), index3 + 1);
        index2     = data_input.indexOf(F(","), index + 1);
        value.rsrp = data_input.substring(index + 1, index2);

        index3     = data_input.indexOf(F(","), index2 + 1);
        value.rsrq = data_input.substring(index2 + 1, index3);

        index     = data_input.indexOf(F(","), index3 + 1);
        index2    = data_input.indexOf(F(","), index + 1);
        value.snr = data_input.substring(index + 1, index2);
      } else if (data_input.indexOf(F("OK")) != -1) {
        break;
      }
    }
  }
  blankChk(value.pci);
  blankChk(value.rsrp);
  blankChk(value.rsrq);
  blankChk(value.snr);
  return value;
}

void AT_SIM7020E::blankChk(String &val) {
  if (val == "") {
    val = "N/A";
  }
}

bool AT_SIM7020E::checkPSMmode() {
  bool status = false;
  _Serial->print(F("AT+CPSMS?"));
  _Serial->println();

  startTime = millis();
  while (millis() - startTime < timeout_ms) {
    if (_Serial->available()) {
      data_input = _Serial->readStringUntil('\n');
      if (data_input.indexOf(F("+CPSMS: ")) != -1) {
        if (data_input.indexOf(F("1")) != -1)
          status = true;
        else
          status = false;
      }
      if (data_input.indexOf(F("OK")) != -1) {
        break;
      }
    }
  }
  return status;
}

/****************************************/
/**          Send UDP Message          **/
/****************************************/
// Send AT command to send UDP message
void AT_SIM7020E::sendCmd(String address, String port, unsigned int len) {
  if (debug)
    Serial.println("Send to " + address + "," + port);
  _Serial->print(F("AT+CSOSEND=0,"));
  _Serial->print(len);
  _Serial->print(F(","));
}

// Send message type String
void AT_SIM7020E::sendCmd(String msg) {
  _Serial->print(msg);
}

// Send message type unsigned int
void AT_SIM7020E::sendCmd(unsigned int msg) {
  _Serial->print(msg);
}

// Send message type char *
void AT_SIM7020E::sendCmd(char *msg) {
  _Serial->print(msg);
}

// Send '\r\n'
void AT_SIM7020E::endCmd() {
  _Serial->println();
}

/****************************************/
/**        Receive UDP Message         **/
/****************************************/
// Receive incoming message : +CSONMI: <socket_id>,<data_len>,<data>
void AT_SIM7020E::waitResponse(String &retdata, String server) {

  if (_Serial->available()) {
    char data = (char)_Serial->read();
    if (data == '\n' || data == '\r') {
      if (k > 1) {
        end = true;
        k   = 0;
      }
      k++;
    } else {
      data_input += data;
    }
  }
  if (end) {
    manageResponse(retdata, server);
  }
}

// Split data from incoming message
void AT_SIM7020E::manageResponse(String &retdata, String server) {
  if (end) {
    end = false;

    if (data_input.indexOf(F("+CSONMI:")) != -1) {
      String left_buffer = "";

      // pack data to char array
      char buf[data_input.length() + 1];
      memset(buf, '\0', data_input.length());
      data_input.toCharArray(buf, sizeof(buf));

      char *p = buf;
      char *str;
      byte  i = 0;
      byte  j = 0;
      while ((str = strtok_r(p, ",", &p)) != NULL) {    // delimiter is the comma
        j = 2;                                          // number of comma

        if (i == j) {
          retdata = str;
        }
        if (i == j + 1) {
          left_buffer = str;
        }
        i++;
      }
      data_input = F("");
    }
  }
}

/****************************************/
/**          Utility                   **/
/****************************************/
// print char * to hex
void AT_SIM7020E::printHEX(char *str) {
  char *hstr;
  hstr = str;
  char out[3];
  memset(out, '\0', 2);
  bool flag = false;
  while (*hstr) {
    flag = itoa((int)*hstr, out, 16);

    if (flag) {
      _Serial->print(out);
    }
    hstr++;
  }
}

// Flush unwanted message from serial
void AT_SIM7020E::_serial_flush() {
  while (_Serial->available()) {
    _Serial->readStringUntil('\n');
  }
  _Serial->flush();
  data_input = "";
}

dateTime AT_SIM7020E::getClock(unsigned int timezone) {
  dateTime dateTime;
  _Serial->print(F("AT+CCLK?"));
  _Serial->println();

  unsigned long startTime = millis();
  while (millis() - startTime < timeout_ms) {
    data_input = _Serial->readStringUntil('\n');
    if (data_input.indexOf(F("+CCLK:")) != -1) {
      byte index    = data_input.indexOf(F(":"));
      byte index2   = data_input.indexOf(F(","), index + 1);
      byte index3   = data_input.indexOf(F("+"), index2 + 1);
      dateTime.date = data_input.substring(index + 1, index2);    // YY/MM/DD
      dateTime.time =
          data_input.substring(index2 + 1, index3);    // GMT time without adding timezone
    }
    if (data_input.indexOf(F("OK")) != -1) {
      break;
    }
  }
  if (dateTime.time != "" && dateTime.date != "") {
    byte         index  = dateTime.date.indexOf(F("/"));
    byte         index2 = dateTime.date.indexOf(F("/"), index + 1);
    unsigned int yy     = ("20" + dateTime.date.substring(0, index)).toInt();
    unsigned int mm     = dateTime.date.substring(index + 1, index2).toInt();
    unsigned int dd     = dateTime.date.substring(index2 + 1, dateTime.date.length()).toInt();

    index           = dateTime.time.indexOf(F(":"));
    unsigned int hr = dateTime.time.substring(0, index).toInt() + timezone;

    if (hr >= 24) {
      hr -= 24;
      dd += 1;
      if (mm == 2) {
        if (((yy % 4 == 0) && (yy % 100 != 0) || (yy % 400 == 0)) && (dd > 29)) {
          dd = 1;
          mm += 1;
        } else if (dd > 28) {
          dd = 1;
          mm += 1;
        }
      } else if ((mm == 1 || mm == 3 || mm == 5 || mm == 7 || mm == 8 || mm == 10 || mm == 12) &&
                 (dd > 31)) {
        dd = 1;
        mm += 1;
      } else if (dd > 30) {
        dd = 1;
        mm += 1;
      }
    }
    dateTime.time = String(hr) + dateTime.time.substring(index, dateTime.time.length());
    dateTime.date = String(dd) + "/" + String(mm) + "/" + String(yy);

    dateTime.time.trim();
    dateTime.date.trim();
  }
  blankChk(dateTime.time);
  blankChk(dateTime.date);
  return dateTime;
}

void AT_SIM7020E::syncLocalTime() {
  _Serial->print(F("AT+CLTS=1"));
  _Serial->println();
  delay(50);
}

void AT_SIM7020E::setAPN(String apn) {
  if (apn == "") {
    apn = APN;
  }
  _Serial->print("AT*MCGDEFCONT=\"IP\",\"");
  _Serial->print(apn);
  _Serial->print("\"");
  _Serial->println();
  waitForResponse("OK", "ERROR", timeout_ms);
  _serial_flush();
}

int AT_SIM7020E::waitForResponse(const String &success, const String &error, int timeout) {
  int status = -1;
  startTime  = millis();
  while (millis() - startTime < timeout) {
    if (_Serial->available()) {
      data_input += _Serial->readStringUntil('\n');
      if (data_input.indexOf(success) != -1) {
        status = 1;
        break;
      } else if (data_input.indexOf(error) != -1) {
        status = 0;
        break;
      }
    }
  }
  _serial_flush();
  return status;
}

/****************************************/
/**                MQTT                **/
/****************************************/

bool AT_SIM7020E::newMQTT(const String &server, const String &port) {
  _serial_flush();
  _Serial->print(F("AT+CMQNEW="));
  _Serial->print(F("\""));
  _Serial->print(server);
  _Serial->print(F("\""));
  _Serial->print(F(","));
  _Serial->print(F("\""));
  _Serial->print(port);
  _Serial->print(F("\""));
  _Serial->print(F(","));
  _Serial->print(mqttCmdTimeout);    // command_timeout_ms
  _Serial->print(F(","));
  _Serial->println(mqttBuffSize);    // buff size

  startTime = millis();
  while (millis() - startTime < timeout_ms) {
    if (_Serial->available()) {
      data_input += _Serial->readStringUntil('\n');
      if (data_input.indexOf(F("+CMQNEW:")) != -1 && data_input.indexOf(F("OK")) != -1) {
        return true;
      } else {
        if (data_input.indexOf(F("ERROR")) != -1) {
          return false;
        }
      }
    }
  }
  return false;
}

bool AT_SIM7020E::sendMQTTconnectionPacket(String clientID, String username, String password,
                                           int keepalive, int version, int cleansession,
                                           int willflag, String willOption) {
  _serial_flush();
  _Serial->print(F("AT+CMQCON=0,"));
  _Serial->print(version);    //<version> : 3 > 3.1, 4 > 3.1.1
  _Serial->print(F(","));
  _Serial->print(F("\""));
  _Serial->print(clientID);    //<client_id> : should be unique.Max length is 120
  _Serial->print(F("\""));
  _Serial->print(F(","));
  _Serial->print(keepalive);    //<keepalive_interval> : 0 - 64800
  _Serial->print(F(","));
  _Serial->print(cleansession);    //<cleansession>
  _Serial->print(F(","));
  _Serial->print(willflag);    //<will_flag>  >> if 1, must include will_option
  if (willflag == 1) {
    _Serial->print(F(","));
    _Serial->print(willOption);    //"topic=xxx,QoS=xxx,retained=xxx,message_len=xxx,message=xxx"
  }

  if (username.length() > 0) {
    _Serial->print(F(","));
    _Serial->print(F("\""));
    _Serial->print(username);    //<username> String, user name (option). Max length is 100
    _Serial->print(F("\""));
    _Serial->print(F(","));
    _Serial->print(F("\""));
    _Serial->print(password);    //<password> String, password (option). Max length is 100
    _Serial->print(F("\""));
  }
  _Serial->println();

  startTime = millis();
  while (millis() - startTime < timeout_ms) {
    data_input = _Serial->readStringUntil('\n');
    if (data_input.indexOf(F("OK")) != -1) {
      return true;
    } else if (data_input.indexOf(F("ERROR")) != -1 || data_input.indexOf(F("+CMQDISCON")) != -1) {
      return false;
    }
  }
  return false;
}

void AT_SIM7020E::disconnectMQTT() {
  _Serial->print(F("AT+CMQDISCON=0"));
  _Serial->println();
  startTime = millis();
  while (millis() - startTime < MAXTIME) {
    if (_Serial->available()) {
      data_input = _Serial->readStringUntil('\n');
      if (data_input.indexOf(F("OK")) != -1) {
        break;
      }
      if (data_input.indexOf(F("ERROR")) != -1) {
        break;
      }
    }
  }
  data_input = F("");
}

bool AT_SIM7020E::MQTTstatus() {
  _serial_flush();
  _Serial->print(F("AT+CMQCON?"));
  _Serial->println();
  String inp = "";
  startTime  = millis();
  while (millis() - startTime < timeout_ms) {
    if (_Serial->available()) {
      data_input = _Serial->readStringUntil('\n');
      if (data_input.indexOf(F("+CMQCON:")) != -1) {
        inp = data_input;
      } else if (data_input.indexOf(F("OK")) != -1) {
        break;
      } else if (data_input.indexOf(F("ERROR")) != -1) {
        return false;
      }
    }
  }
  data_input = F("");
  if (inp != "") {
    //+CMQCON: <mqtt_id>,<connected_state>,<server>
    byte index  = inp.indexOf(F(","));
    byte index2 = inp.indexOf(F(","), index + 1);
    if (inp.substring(index + 1, index2).indexOf(F("1")) != -1) {
      return true;
    } else if (inp.substring(index + 1, index2).indexOf(F("0")) != -1) {
      return false;
    }
  }
  return false;
}

void AT_SIM7020E::publish(String topic, String payload, unsigned int qos, unsigned int retained,
                          unsigned int dup) {
  _serial_flush();
  data_input = F("");
  char data[payload.length() + 1];
  memset(data, '\0', payload.length());
  payload.toCharArray(data, payload.length() + 1);

  _Serial->print(F("AT+CMQPUB=0,\""));
  _Serial->print(topic);    //<topic> String, topic of publish message. Max length is 128
  _Serial->print(F("\""));
  _Serial->print(F(","));
  _Serial->print(qos);    //<Qos> Integer, message QoS, can be 0, 1 or 2.
  _Serial->print(F(","));
  _Serial->print(retained);    //<retained> Integer, retained flag, can be 0 or 1.
  _Serial->print(F(","));
  _Serial->print(dup);    //<dup> Integer, duplicate flag, can be 0 or 1.
  _Serial->print(F(","));
  _Serial->print(payload.length() *
                 msgLenMul);    //<message_len> Integer, length of publish message,can be
                                // from 2 to 1000. If message is HEX data streaming,then
                                // <message_len> should be even.
  _Serial->print(F(",\""));
  printHEX(data);
  _Serial->print(F("\""));
  _Serial->println();
}

bool AT_SIM7020E::subscribe(String topic, unsigned int qos) {
  _serial_flush();
  _Serial->print(F("AT+CMQSUB=0,\""));
  _Serial->print(topic);    //<topic> String, topic of subscribe message. Max length is
                            // 128.
  _Serial->print(F("\","));
  _Serial->print(qos);    //<Qos> Integer, message QoS, can be 0, 1 or 2.
  _Serial->println();

  return waitForResponse("OK", "ERROR", timeout_ms);
}

void AT_SIM7020E::unsubscribe(String topic) {
  _serial_flush();
  _Serial->print(F("AT+CMQUNSUB=0,\""));
  _Serial->print(topic);
  _Serial->print(F("\""));
  _Serial->println();
  startTime = millis();
  while (millis() - startTime < timeout_ms) {
    if (_Serial->available()) {
      data_input = _Serial->readStringUntil('\n');
      if (data_input.indexOf(F("OK")) != -1) {
        if (debug)
          Serial.print(F("Unsubscribe topic :"));
        if (debug)
          Serial.println(topic);
        break;
      } else if (data_input.indexOf(F("ERROR")) != -1) {
        break;
      }
    }
  }
  data_input = F("");
}

int AT_SIM7020E::MQTTresponse() {    // clear buffer before this
  int status = 2;
  if (_Serial->available()) {
    char receivedData = (char)_Serial->read();
    if (receivedData == '\n' || receivedData == '\r') {
      end = true;
    } else {
      data_input += receivedData;
    }
  }
  if (end) {
    if (data_input.indexOf(F("+CMQPUB")) != -1 || data_input.indexOf(F("+CMQPUBEXT")) != -1) {
      String items[7];
      int    index = 0;
      int    nextIndex;
      for (int i = 0; i < 7; i++) {
        int nextIndex = data_input.indexOf(F(","), index);
        items[i]      = data_input.substring(index, nextIndex);
        index         = nextIndex + 1;
      }

      String retTopic    = items[1];
      String retQoS      = items[2];
      String retRetained = items[3];
      int    msgLen      = items[5].toInt() / 2;
      String retPayload  = items[6];

      retTopic.replace(F("\""), "");
      retPayload.replace(F("\""), "");

      if (MQcallback_p != NULL) {
        MQcallback_p(retTopic, retQoS, retRetained, msgLen, retPayload);
      }
    } else if (data_input.indexOf(F("OK")) != -1) {
      status = 1;
    } else if (data_input.indexOf(F("ERROR")) != -1) {
      status = 0;
    }
    data_input = F("");
    end        = false;
  }
  retPayload = "";
  return status;
}

bool AT_SIM7020E::setCallback(MQTTClientCallback callbackFunc) {
  if (MQcallback_p == NULL) {
    MQcallback_p = callbackFunc;
    return true;
  }
  return false;
}

/****************************************/
/**                MQTTs               **/
/****************************************/

bool AT_SIM7020E::newMQTTs(const String &server, const String &port) {
  _serial_flush();
  _Serial->print(F("AT+CMQTTSNEW="));
  _Serial->print(F("\""));
  _Serial->print(server);
  _Serial->print(F("\""));
  _Serial->print(F(","));
  _Serial->print(F("\""));
  _Serial->print(port);
  _Serial->print(F("\""));
  _Serial->print(F(","));
  _Serial->print(MAXTIME);    // command_timeout_ms
  _Serial->print(F(","));
  _Serial->print(mqttBuffSize);    // buff size
  _Serial->println();

  startTime = millis();
  while (millis() - startTime < MAXTIME) {
    if (_Serial->available()) {
      data_input += _Serial->readStringUntil('\n');
      if (data_input.indexOf(F("+CMQTTSNEW:")) != -1 && data_input.indexOf(F("OK")) != -1) {
        return true;
      } else {
        if (data_input.indexOf(F("ERROR")) != -1) {
          return false;
        }
      }
    }
  }
  return false;
}

bool AT_SIM7020E::setCertificate(byte cerType, int cerLength, byte isEnd, String CA) {
  _serial_flush();
  _Serial->print(F("AT+CSETCA="));
  _Serial->print(cerType);
  _Serial->print(F(","));
  _Serial->print(cerLength);
  _Serial->print(F(","));
  _Serial->print(isEnd);
  _Serial->print(F(","));
  _Serial->print(mqttsCertType);
  _Serial->print(F(",\""));
  _Serial->print(CA);
  _Serial->print(F("\""));
  _Serial->println();
  return waitForResponse(F("OK"), F("ERROR"), timeout_ms) == 1;
}

bool AT_SIM7020E::verifyCertificateLengths(const int &root_ca_len, const int &client_ca_len,
                                           const int &private_key_len) {
  _serial_flush();
  bool rootCA;
  bool clientCA;
  bool privateKey;

  _Serial->println(F("AT+CSETCA?"));
  startTime = millis();
  while (millis() - startTime < timeout_ms) {
    if (_Serial->available()) {
      data_input = _Serial->readStringUntil('\n');
      if (data_input.indexOf(F("Root CA:")) != -1) {
        rootCA = data_input.indexOf(String(root_ca_len)) != -1;
      }
      if (data_input.indexOf(F("Client CA:")) != -1) {
        clientCA = data_input.indexOf(String(client_ca_len)) != -1;
      }
      if (data_input.indexOf(F("Client Private Key:")) != -1) {
        privateKey = data_input.indexOf(String(private_key_len)) != -1;
      }
      if (data_input.indexOf(F("OK")) != -1) {
        break;
      }
    }
  }
  return rootCA && clientCA && privateKey;
}