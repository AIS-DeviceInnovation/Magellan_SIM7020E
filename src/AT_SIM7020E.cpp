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

AT Command Dictionary for SIMCOM SIM7020E version 1.4.1
support SIMCOM SIM7020E
NB-IoT with AT command

Author: Device Innovation team
Create Date: 2 January 2020.
Modified: 17 February 2021.
*/

#include "AT_SIM7020E.h"
#include "board.h"

/****************************************/
/**        Initialization Module       **/
/****************************************/
AT_SIM7020E::AT_SIM7020E() {
}

void AT_SIM7020E::setupModule(String address, String port) {

    previous_check = millis();

    pinMode(hwResetPin, OUTPUT); // set reset pin for hardware reboot

    Serial.print(F(">>Rebooting ."));
    reboot_module();
    Serial.println(F("..OK"));

    if (serialConfig == 1) {
        serialPort.begin(buadrate, configParam, rxPin, txPin);
        _Serial = &serialPort;
    }
    else {
        serialPort.begin(buadrate);
        _Serial = &serialPort;
    }

    Serial.print(F(">>Check module status "));
    check_module_ready();
    if (!hw_connected) {
        // pass
    }

    Serial.print(F(">>Setup "));

    _Serial->println("AT+CMEE=1"); // set report error
    syncLocalTime();               // sync local time
    _serial_flush();
    Serial.println(F("...OK"));

    Serial.print(F(">>IMSI   : "));
    Serial.println(getIMSI());

    Serial.print(F(">>ICCID  : "));
    Serial.println(getICCID());

    Serial.print(F(">>IMEI   : "));
    Serial.println(getIMEI());

    if (debug) {
        Serial.print(F(">>FW ver : "));
        Serial.println(getFirmwareVersion());
        Serial.print(F(">>PSM mode : "));
        Serial.println(checkPSMmode());
    }

    delay(500);
    Serial.print(F(">>Signal : "));
    Serial.print(getSignal());
    Serial.println(F(" dBm"));

    _serial_flush();
    Serial.print(F(">>Connecting "));

    if (attachNetwork()) {
        if ((address != "") && (port != "")) {
            if (!createUDPSocket(address, port)) {
                Serial.println(F(">> Cannot create socket"));
            }
        }
        Serial.println(F("OK"));
        Serial.println(F("---------- Connected ----------"));
    }
    else {
        Serial.println(F("FAILED"));
        Serial.println(F("-------- Disconnected ---------"));
        ESP.restart();
    }

    if (debug) {
        Serial.print(F(">>APN   : "));
        Serial.println(getAPN());
    }
}

void AT_SIM7020E::check_module_ready() {
    int count = 0;

    _Serial->println(F("AT"));
    delay(100);
    while (true) {
        if (_Serial->available() > 0) {
            data_input = _Serial->readStringUntil('\n');
            if (data_input.indexOf(F("OK")) != -1) {
                hw_connected = true;
                Serial.println(F("...OK"));
                break;
            }
        }
        else {
            unsigned int current_check = millis();
            if ((current_check - previous_check) > 5000U) {
                if (count > 5) {
                    Serial.print(F("\nError to connect NB Module, rebooting..."));
                }

                previous_check = current_check;
                hw_connected   = false;
                Serial.print(F("."));
                reboot_module();
                _Serial->println(F("AT"));
                delay(100);
                count++;
            }
            else {
                delay(500);
                _Serial->println(F("AT"));
                delay(100);
                Serial.print(F("."));
            }
        }
    }
    echoOff();
    delay(1000);
    powerSavingMode(0);
}

//  Reboot module with hardware pin.
void AT_SIM7020E::reboot_module() {
    digitalWrite(hwResetPin, LOW);
    delay(1000);
    digitalWrite(hwResetPin, HIGH);
    delay(2000);
}

bool AT_SIM7020E::attachNetwork() {
    bool status = false;

    if (!NBstatus()) {
        for (byte i = 0; i < 60; i++) {
            (void) setPhoneFunction();
            connectNetwork();
            delay(1000);
            if (NBstatus()) {
                status = true;
                break;
            }
            Serial.print(F("."));
        }
    }
    else {
        status = true;
    }

    _serial_flush();
    _Serial->flush();

    return status;
}

// Check network connecting status : 1 connected, 0 not connected
bool AT_SIM7020E::NBstatus() {
    bool status = false;

    _serial_flush();
    _Serial->println(F("AT+CGATT?"));
    delay(800);
    for (byte i = 0U; i < 60U; i++) {
        if (_Serial->available() > 0) {
            data_input = _Serial->readStringUntil('\n');
            if (data_input.indexOf(F("+CGATT: 1")) != -1) {
                status = true;
            }
            else if (data_input.indexOf(F("+CGATT: 0")) != -1) {
                status = false;
            }
            else if (data_input.indexOf(F("OK")) != -1) {
                break;
            }
            else if (data_input.indexOf(F("ERROR")) != -1) {
                break;
            }
            else {
                // pass
            }
        }
    }
    data_input = "";

    return status;
}
// Set Phone Functionality : 1 Full functionality
bool AT_SIM7020E::setPhoneFunction() {
    bool status = false;

    _Serial->println(F("AT+CFUN=1"));
    while (true) {
        if (_Serial->available() > 0) {
            data_input = _Serial->readStringUntil('\n');
            if (data_input.indexOf(F("OK")) != -1) {
                status = true;
                break;
            }
            else {
                if (data_input.indexOf(F("ERROR")) != -1) {
                    status = false;
                    break;
                }
            }
        }
    }
    Serial.print(F("."));

    return status;
}

// Attach network : 1 connected, 0 disconnected
void AT_SIM7020E::connectNetwork() {
    _Serial->println(F("AT+CGATT=1"));
    for (int i = 0; i < 30; i++) {
        if (_Serial->available() > 0) {
            data_input = _Serial->readStringUntil('\n');
            if (data_input.indexOf(F("OK")) != -1) {
                break;
            }
            else {
                if (data_input.indexOf(F("ERROR")) != -1) {
                    break;
                }
            }
        }
    }
    Serial.print(F("."));
}

// Create a UDP socket and connect socket to remote address and port
bool AT_SIM7020E::createUDPSocket(String address, String port) {
    bool status = false;

    _Serial->println(F("AT+CSOC=1,2,1"));
    delay(200);
    while (true) {
        if (_Serial->available() > 0) {
            data_input = _Serial->readStringUntil('\n');
            if (data_input.indexOf(F("OK")) != -1) {
            }
            else if (data_input.indexOf(F("+CSOC: 0")) != -1) {
                status = true;
                break;
            }
            else {
                if (data_input.indexOf(F("+CSOC: 1")) != -1) {
                    status = false;
                    (void) closeUDPSocket();
                    _Serial->println(F("AT+CSOC=1,2,1"));
                }
            }
        }
    }

    if (status) {
        _Serial->print(F("AT+CSOCON=0,"));
        _Serial->print(port);
        _Serial->print(F(","));
        _Serial->println(address);
        while (true) {
            if (_Serial->available() > 0) {
                data_input = _Serial->readStringUntil('\n');
                if (data_input.indexOf(F("OK")) != -1) {
                    break;
                }
                else {
                    if (data_input.indexOf(F("ERROR")) != -1) {
                        status = false;
                        break;
                    }
                }
            }
        }
    }

    return status;
}

// Close a UDP socket 0
bool AT_SIM7020E::closeUDPSocket() {
    _Serial->println(F("AT+CSOCL=0"));
    while (true) {
        if (_Serial->available() > 0) {
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
    while (true) {
        if (_Serial->available() > 0) {
            data_input = _Serial->readStringUntil('\n');
            if (data_input.indexOf(F("OK")) != -1) {
                break;
            }
        }
    }
}

// Ping IP
pingRESP AT_SIM7020E::pingIP(String IP) {
    pingRESP pingr;
    String   data = "";
    int      replytime;
    int      ttl;

    replytime = 0;
    ttl       = 0;
    _Serial->println("AT+CIPPING=" + IP);
    while (true) {
        if (_Serial->available() > 0) {
            data_input = _Serial->readStringUntil('\n');
            if (data_input.indexOf(F("ERROR")) != -1) {
                break;
            }
            else if (data_input.indexOf(F("+CIPPING: ")) != -1) {
                data = data_input;

                byte index  = data.indexOf(F(","));
                byte index2 = data.indexOf(F(","), index + 1);
                byte index3 = data.indexOf(F(","), index2 + 1);

                pingr.addr = data.substring(index + 1, index2);
                replytime += data.substring(index2 + 1, index3).toInt();

                ttl += data.substring(index3 + 1, data.length()).toInt();
            }
            else {
                // pass
            }

            if (data_input.indexOf(F("+CIPPING: 4")) != -1) {
                break;
            }
        }
    }

    if (data != "") {
        pingr.ttl = String(ttl / 4);
        pingr.rtt = String((replytime / 4.0) * 100);
        blankChk(pingr.ttl);
        blankChk(pingr.rtt);
        Serial.println(">>Ping IP : " + pingr.addr + ", ttl= " + pingr.ttl + ", replyTime= " + pingr.rtt + "ms");
        pingr.status = true;
    }
    else {
        Serial.println(F(">>Ping Failed"));
        pingr.status = false;
    }
    _serial_flush();
    data_input = "";

    return pingr;
}

// Set powerSavingMode : 0 turn off, 1 turn on
void AT_SIM7020E::powerSavingMode(unsigned int psm) {
    _Serial->print(F("AT+CPSMS="));
    _Serial->println(psm);
    _serial_flush();
}

// Check if SIM/eSIM need PIN or not.
bool AT_SIM7020E::enterPIN() {
    bool status = false;

    _Serial->println(F("AT+CPIN?"));
    while (true) {
        if (_Serial->available() > 0) {
            data_input = _Serial->readStringUntil('\n');
            if (data_input.indexOf(F("+CPIN:")) != -1) {
                if (data_input.indexOf(F("READY")) != -1) {
                    status = true;
                    break;
                }
            }

            if (data_input.indexOf(F("OK")) != -1) {
                break;
            }
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
    while (true) {
        if (_Serial->available() > 0) {
            data_input = _Serial->readStringUntil('\n');
            if ((data_input.indexOf(F("OK")) != -1) && (imsi.indexOf(F("52003")) != -1)) {
                break;
            }
            else if (data_input.indexOf(F("ERROR")) != -1) {
                _Serial->println(F("AT+CIMI"));
            }
            else {
                imsi += data_input;
            }
        }
    }
    imsi.replace(F("+CPIN: READY"), "");
    imsi.replace(F("OK"), "");
    imsi.trim();

    blankChk(imsi);
    // Serial.print(F(">>IMSI : "));
    // Serial.println(imsi);
    return imsi;
}

String AT_SIM7020E::getICCID() {
    String iccid = "";

    _Serial->println(F("AT+CCID"));
    while (true) {
        if (_Serial->available() > 0) {
            data_input = _Serial->readStringUntil('\n');
            if (data_input.indexOf(F("OK")) != -1) {
                break;
            }
            else {
                iccid += data_input;
            }
        }
    }
    iccid.replace(F("OK"), "");
    iccid.trim();

    blankChk(iccid);
    // Serial.print(F(">>ICCID : "));
    // Serial.println(iccid);
    return iccid;
}

String AT_SIM7020E::getIMEI() {
    String imei;

    _Serial->println(F("AT+CGSN=1"));
    while (true) {
        if (_Serial->available() > 0) {
            data_input = _Serial->readStringUntil('\n');
            if (data_input.indexOf(F("+CGSN:")) != -1) {
                data_input.replace(F("+CGSN: "), "");
                imei = data_input;
            }
            else {
                if ((data_input.indexOf(F("OK")) != -1) && (imei != "")) {
                    break;
                }
            }
        }
    }

    blankChk(imei);
    // Serial.print(F(">>IMEI : "));
    // Serial.println(imei);
    return imei;
}

String AT_SIM7020E::getDeviceIP() {
    String deviceIP;

    _serial_flush();
    _Serial->println(F("AT+CGPADDR=1"));
    bool chk = false;
    while (true) {
        if (_Serial->available() > 0) {
            data_input = _Serial->readStringUntil('\n');
            if (data_input.indexOf(F("+CGPADDR")) != -1) {
                chk         = true;
                byte index  = data_input.indexOf(F(":"));
                byte index2 = data_input.indexOf(F(","));
                deviceIP    = data_input.substring(index2 + 1, data_input.length());
            }
            else {
                if ((data_input.indexOf(F("OK")) != -1) && chk) {
                    break;
                }
            }
        }
    }
    deviceIP.replace(F("\""), "");
    deviceIP.trim();

    blankChk(deviceIP);
    data_input = "";
    // Serial.print(F(">>Device IP : "));
    // Serial.println(deviceIP);
    return deviceIP;
}

String AT_SIM7020E::getSignal() {
    _serial_flush();

    int    rssi     = 0;
    int    count    = 0;
    String data_csq = "";
    data_input      = "";
    do {
        _Serial->println(F("AT+CSQ"));
        delay(200);
        while (true) {
            if (_Serial->available() > 0) {
                data_input = _Serial->readStringUntil('\n');
                if (data_input.indexOf(F("OK")) != -1) {
                    break;
                }
                else {
                    if (data_input.indexOf(F("+CSQ")) != -1) {
                        byte start_index = data_input.indexOf(F(":"));
                        byte stop_index  = data_input.indexOf(F(","));
                        data_csq         = data_input.substring(start_index + 1, stop_index);

                        rssi = data_csq.toInt();
                        rssi = (2 * rssi) - 113;
                        data_csq = String(rssi);
                    }
                }
            }
        }

        if (rssi == -113) {
            count++;
        }
    } while (((rssi == -113) && (count <= 10)) || ((rssi == 85) && (count <= 10)));

    if ((rssi == -113) || (rssi == 85)) {
        data_csq = "-113";
        count    = 0;
    }

    return data_csq;
}

String AT_SIM7020E::getAPN() {
    String out = "";
    _Serial->println(F("AT+CGDCONT?"));

    while (true) {
        if (_Serial->available() > 0) {
            data_input = _Serial->readStringUntil('\n');
            if (data_input.indexOf(F("+CGDCONT: 1")) != -1) {
                byte index  = data_input.indexOf(F(":"));
                byte index2 = data_input.indexOf(F(","));

                index  = data_input.indexOf(F(","), index2 + 1);
                index2 = data_input.indexOf(F(","), index + 1);
                out    = data_input.substring(index + 2, index2 - 1);

                if (out == ",,") {
                    out = "";
                }
            }

            if (data_input.indexOf(F("OK")) != -1) {
                break;
            }
        }
    }
    _serial_flush();
    data_input = "";
    blankChk(out);
    return out;
}

String AT_SIM7020E::getFirmwareVersion() {
    String fw = "";

    _Serial->println(F("AT+CGMR"));
    while (true) {
        if (_Serial->available() > 0) {
            data_input = _Serial->readStringUntil('\n');
            if (data_input.indexOf(F("OK")) != -1) {
                break;
            }
            else {
                fw += data_input;
            }
        }
    }
    fw.replace(F("OK"), "");
    fw.trim();
    blankChk(fw);
    // Serial.print(F(">>FW Ver: "));
    // Serial.println(fw);
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
    while (true) {
        if (_Serial->available() > 0) {
            data_input = _Serial->readStringUntil('\n');
            if (data_input.indexOf(F("+CEREG")) != -1) {
                count++;
                if ((count < 10) && (data_input.indexOf(F(",2")) != -1)) {
                    _serial_flush();
                    _Serial->println(F("AT+CEREG?"));
                }
                else {
                    data        = data_input;
                    byte index  = data.indexOf(F(": "));
                    byte index2 = data.indexOf(F(","));
                    byte index3 = data.indexOf(F(","), index2 + 1);
                    out         = data.substring(index2 + 1, index2 + 2);
                    if (out == F("1")) {
                        out = F("Registered");
                    }
                    else if (out == "0") {
                        out = F("Not Registered");
                    }
                    else if (out == "2") {
                        out = F("Trying");
                    }
                    else {
                        // pass
                    }
                }
            }
            else {
                if (data_input.indexOf(F("OK")) != -1) {
                    break;
                }
            }
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
    while (true) {
        if (_Serial->available() > 0) {
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
            }
            else {
                if (data_input.indexOf(F("OK")) != -1) {
                    break;
                }
            }
        }
    }
    blankChk(value.pci);
    blankChk(value.rsrp);
    blankChk(value.rsrq);
    blankChk(value.snr);
    return value;
}

void AT_SIM7020E::blankChk(String& val) {
    if (val == "") {
        val = "N/A";
    }
}

bool AT_SIM7020E::checkPSMmode() {
    bool status = false;

    _Serial->println(F("AT+CPSMS?"));
    while (true) {
        if (_Serial->available() > 0) {
            data_input = _Serial->readStringUntil('\n');
            if (data_input.indexOf(F("+CPSMS: ")) != -1) {
                if (data_input.indexOf(F("1")) != -1) {
                    status = true;
                }
                else {
                    status = false;
                }
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
void AT_SIM7020E::_Serial_print(String address, String port, unsigned int len) {
    if (debug) {
        Serial.println("Send to " + address + "," + port);
    }
    _Serial->print(F("AT+CSOSEND=0,"));
    _Serial->print(len);
    _Serial->print(F(","));
}

// Send message type String
void AT_SIM7020E::_Serial_print(String msg) {
    _Serial->print(msg);
}

// Send message type unsigned int
void AT_SIM7020E::_Serial_print(unsigned int msg) {
    _Serial->print(msg);
}

// Send message type char *
void AT_SIM7020E::_Serial_print(char* msg) {
    _Serial->print(msg);
}

// Send '\r\n'
void AT_SIM7020E::_Serial_println() {
    _Serial->println();
}

/****************************************/
/**        Receive UDP Message         **/
/****************************************/
// Receive incoming message : +CSONMI: <socket_id>,<data_len>,<data>
void AT_SIM7020E::waitResponse(String& retdata, String server) {

    if (_Serial->available() > 0) {
        char data = (char)_Serial->read();
        if ((data == '\n') || (data == '\r')) {
            if (k > 1) {
                end = true;
                k   = 0;
            }
            k++;
        }
        else {
            data_input += data;
        }
    }

    if (end) {
        manageResponse(retdata, server);
    }
}

// Split data from incoming message
void AT_SIM7020E::manageResponse(String& retdata, String server) {
    if (end) {
        end = false;

        if (data_input.indexOf(F("+CSONMI:")) != -1) {
            String left_buffer = "";

            // pack data to char array
            char buf[data_input.length() + 1];
            char* p = buf;
            char* str;
            byte  i;
            byte  j;

            i = 0;
            j = 0;
            data_input.toCharArray(buf, sizeof(buf));
            while (true) {                                      // delimiter is the comma
                str = strtok_r(p, ",", &p);
                if (str == NULL) {
                    break;
                }

                j = 2;                                          // number of comma

                if (i == j) {
                    retdata = str;
                }

                if (i == (j + 1)) {
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
void AT_SIM7020E::printHEX(char* str) {
    char* hstr;
    char  out[3];

    hstr = str;
    while (*hstr != '\0') {
        (void) itoa((int)*hstr, out, 16);

        _Serial_print(out);

        hstr++;
    }
}

// Flush unwanted message from serial
void AT_SIM7020E::_serial_flush() {
    while (true) {
        if (_Serial->available() > 0) {
            data_input = _Serial->readStringUntil('\n');
        }
        else {
            data_input = "";
            break;
        }
    }
    _Serial->flush();
}

dateTime AT_SIM7020E::getClock(unsigned int timezone) {
    dateTime dateTime;
    _Serial->println(F("AT+CCLK?"));
    while (true) {
        if (_Serial->available() > 0) {
            data_input = _Serial->readStringUntil('\n');
            if (data_input.indexOf(F("+CCLK:")) != -1) {
                byte index    = data_input.indexOf(F(":"));
                byte index2   = data_input.indexOf(F(","), index + 1);
                byte index3   = data_input.indexOf(F("+"), index2 + 1);
                dateTime.date = data_input.substring(index + 2, index2);  // YY/MM/DD
                dateTime.time = data_input.substring(index2 + 1, index3); // UTC time without adding timezone
            }
            if (data_input.indexOf(F("OK")) != -1) {
                break;
            }
        }
    }

    if ((dateTime.time != "") && (dateTime.date != "")) {
        byte         index  = dateTime.date.indexOf(F("/"));
        byte         index2 = dateTime.date.indexOf(F("/"), index + 1);
        unsigned int yy     = ("20" + dateTime.date.substring(0, index)).toInt();
        unsigned int mm     = dateTime.date.substring(index + 1, index2).toInt();
        unsigned int dd     = dateTime.date.substring(index2 + 1, dateTime.date.length()).toInt();

        index           = dateTime.time.indexOf(F(":"));
        unsigned int hr = dateTime.time.substring(0, index).toInt() + timezone;

        if (hr >= 24U) {
            hr -= 24U;
            // date + 1
            dd += 1U;
            if (mm == 2U) {
                if ((((yy % 4U) == 0U) && ((yy % 100U) != 0U) || ((yy % 400U) == 0U))) {
                    if (dd > 29U) {
                        dd = 1U;
                        mm += 1U;
                    }
                }
                else {
                    if (dd > 28U) {
                        dd = 1U;
                        mm += 1U;
                    }
                }
            }
            else if (((mm == 1U) || (mm == 3U) || (mm == 5U) || (mm == 7U) || (mm == 8U) || (mm == 10U) || (mm == 12U)) && (dd > 31U)) {
                dd = 1U;
                mm += 1U;
            }
            else {
                if (dd > 30U) {
                    dd = 1U;
                    mm += 1U;
                }
            }
        }
        dateTime.time = String(hr) + dateTime.time.substring(index, dateTime.time.length());
        dateTime.date = String(dd) + "/" + String(mm) + "/" + String(yy);
    }

    blankChk(dateTime.time);
    blankChk(dateTime.date);

    return dateTime;
}

void AT_SIM7020E::syncLocalTime() {
    _Serial->println(F("AT+CLTS=1"));
    delay(50);
}

/****************************************/
/**                MQTT                **/
/****************************************/

bool AT_SIM7020E::newMQTT(String server, String port) {
    _Serial->print(F("AT+CMQNEW="));
    _Serial->print(F("\""));
    _Serial->print(server);
    _Serial->print(F("\""));
    _Serial->print(F(","));
    _Serial->print(F("\""));
    _Serial->print(port);
    _Serial->print(F("\""));
    _Serial->print(F(","));
    _Serial->print(mqttCmdTimeout); // command_timeout_ms
    _Serial->print(F(","));
    _Serial->print(mqttBuffSize); // buff size
    _Serial->println();

    while (true) {
        if (_Serial->available() > 0) {
            data_input += _Serial->readStringUntil('\n');
            if (data_input.indexOf(F("+CMQNEW:")) != -1 && data_input.indexOf(F("OK")) != -1) {
                return true;
            }
            else {
                if (data_input.indexOf(F("ERROR")) != -1) {
                    return false;
                }
            }
        }
    }
}

bool AT_SIM7020E::sendMQTTconnectionPacket(String clientID, String username, String password, int keepalive,
                                           int version, int cleansession, int willflag, String willOption) {
    // AT+CMQCON=<mqtt_id>,<version>,<client_id>,<keepalive_interval>,<cleansession>,<will_flag>[,<will_options>][,<username>,<password>]
    _Serial->print(F("AT+CMQCON=0,"));
    _Serial->print(version); //<version> : 3 > 3.1, 4 > 3.1.1
    _Serial->print(F(","));
    _Serial->print(F("\""));
    _Serial->print(clientID); //<client_id> : should be unique.Max length is 120
    _Serial->print(F("\""));
    _Serial->print(F(","));
    _Serial->print(keepalive); //<keepalive_interval> : 0 - 64800
    _Serial->print(F(","));
    _Serial->print(cleansession); //<cleansession>
    _Serial->print(F(","));
    _Serial->print(willflag); //<will_flag>  >> if 1 must include will_option

    if (willflag == 1) {
        _Serial->print(F(","));
        _Serial->print(willOption); //"topic=xxx,QoS=xxx,retained=xxx,message_len=xxx,message=xxx"
    }

    if (username.length() > 0) {
        _Serial->print(F(","));
        _Serial->print(F("\""));
        _Serial->print(username); //<username> String, user name (option). Max length is 100
        _Serial->print(F("\""));
        _Serial->print(F(","));
        _Serial->print(F("\""));
        _Serial->print(password); //<password> String, password (option). Max length is 100
        _Serial->print(F("\""));
    }
    _Serial->println();

    while (true) {
        data_input = _Serial->readStringUntil('\n');
        if (data_input.indexOf(F("OK")) != -1) {
            return true;
        }
        else {
            if ((data_input.indexOf(F("ERROR")) != -1) ||
                (data_input.indexOf(F("+CMQDISCON")) != -1)) {
                return false;
            }
        }
    }
}

void AT_SIM7020E::disconnectMQTT() {
    _Serial->println(F("AT+CMQDISCON=0"));
    while (true) {
        if (_Serial->available() > 0) {
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
    _Serial->println(F("AT+CMQCON?"));
    String inp = "";
    while (true) {
        if (_Serial->available() > 0) {
            data_input = _Serial->readStringUntil('\n');
            if (data_input.indexOf(F("+CMQCON:")) != -1) {
                inp = data_input;
            }
            else if (data_input.indexOf(F("OK")) != -1) {
                break;
            }
            else {
                if (data_input.indexOf(F("ERROR")) != -1) {
                    return false;
                }
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
        }
        else {
            if (inp.substring(index + 1, index2).indexOf(F("0")) != -1) {
                return false;
            }
        }
    }
}

void AT_SIM7020E::publish(String topic, String payload, unsigned int qos, unsigned int retained, unsigned int dup) {
    // AT+CMQPUB=<mqtt_id>,<topic>,<QoS>,<retained>,<dup>,<message_len>,<message>
    data_input = F("");
    char data[payload.length() + 1];

    payload.toCharArray(data, payload.length() + 1);
    _Serial->print(F("AT+CMQPUB=0,\""));
    _Serial->print(topic); //<topic> String, topic of publish message. Max length is 128
    _Serial->print(F("\""));
    _Serial->print(F(","));
    _Serial->print(qos); //<Qos> Integer, message QoS, can be 0, 1 or 2.
    _Serial->print(F(","));
    _Serial->print(retained); //<retained> Integer, retained flag, can be 0 or 1.
    _Serial->print(F(","));
    _Serial->print(dup); //<dup> Integer, duplicate flag, can be 0 or 1.
    _Serial->print(F(","));
    _Serial->print(payload.length() *
                   msgLenMul); //<message_len> Integer, length of publish message,can be from 2 to 1000.
                               // If message is HEX data streaming,then <message_len> should be even.
    _Serial->print(F(",\""));
    printHEX(data);
    _Serial->print(F("\""));
    _Serial->println();
}

bool AT_SIM7020E::subscribe(String topic, unsigned int qos) {
    // AT+CMQSUB=<mqtt_id>,<topic>,<QoS>
    _Serial->print(F("AT+CMQSUB=0,\""));
    _Serial->print(topic); //<topic> String, topic of subscribe message. Max length is 128.
    _Serial->print(F("\","));
    _Serial->println(qos); //<Qos> Integer, message QoS, can be 0, 1 or 2.
}

void AT_SIM7020E::unsubscribe(String topic) {
    _Serial->print(F("AT+CMQUNSUB=0,\""));
    _Serial->print(topic);
    _Serial->println(F("\""));

    while (true) {
        if (_Serial->available() > 0) {
            data_input = _Serial->readStringUntil('\n');
            if (data_input.indexOf(F("OK")) != -1) {
                if (debug) {
                    Serial.print(F("Unsubscribe topic :"));
                    Serial.println(topic);
                }
                break;
            }
            else {
                if (data_input.indexOf(F("ERROR")) != -1) {
                    break;
                }
            }
        }
    }

    data_input = F("");
}

unsigned int AT_SIM7020E::MQTTresponse() { // clear buffer before this
    unsigned int ret = 0;

    if (_Serial->available() > 0) {
        char data = (char)_Serial->read();
        if ((data == '\n') || (data == '\r')) {
            end = true;
        }
        else {
            data_input += data;
        }
    }

    if (end) {
        if ((data_input.indexOf(F("+CMQPUB")) != -1) || (data_input.indexOf(F("+CMQPUBEXT")) != -1)) {
            //+CMQPUB: <mqtt_id>,<topic>,<QoS>,<retained>,<dup>,<message_len>,<message>
            byte index  = data_input.indexOf(F(","));
            byte index2 = data_input.indexOf(F(","), index + 1);
            index       = data_input.indexOf(F(","), index2 + 1);
            index2      = data_input.indexOf(F(","), index + 1);
            index       = data_input.indexOf(F(","), index2 + 1);
            index2      = data_input.indexOf(F(","), index + 1);
            int msgLen  = data_input.substring(index + 1, index2).toInt();

            char buf[data_input.length() + 1];
            char* p = buf;
            char* str;
            byte  i = 0;
            byte  j = 0;

            data_input.toCharArray(buf, sizeof(buf));
            while (true) {
                str = strtok_r(p, ",", &p);
                if (str == NULL) {
                    break;
                }

                // delimiter is the comma
                if (i == 1) {
                    retTopic     = str;
                    int topiclen = retTopic.length();
                    retTopic.replace(F("\""), "");
                }

                if (i == 2) {
                    retQoS = str;
                }

                if (i == 3) {
                    retRetained = str;
                }

                if (((i == 6) && (data_input.indexOf(F("+CMQPUB:")) != -1)) ||
                    ((i == 8) && (data_input.indexOf(F("+CMQPUBEXT:")) != -1))) {
                    retPayload = str;
                    if (msgLen > 500) {
                        Serial.println(F("Data incoming overload. [Max 250 characters]."));
                    }
                    else if ((msgLen < 500) && (data_input.indexOf(F("+CMQPUBEXT:")) != -1)) {
                        // Do nothing
                    }
                    else {
                        retPayload.replace(F("\""), "");
                        if (MQcallback_p != NULL) {
                            MQcallback_p(retTopic, retPayload, retQoS, retRetained);
                        }
                    }
                }
                i++;
            }
            ret = 1;
        }
        else if (data_input.indexOf(F("OK")) != -1) {
            ret = 2;
        }
        else if (data_input.indexOf(F("ERROR")) != -1) {
            ret = 3;
        }
        else {
            // pass
        }

        data_input = F("");
        end        = false;
    }
    retPayload = "";
    return ret;
}

int AT_SIM7020E::setCallback(MQTTClientCallback callbackFunc) {
    int r = -1;

    if (MQcallback_p == NULL) {
        MQcallback_p = callbackFunc;
        r            = 0;
    }

    return r;
}
