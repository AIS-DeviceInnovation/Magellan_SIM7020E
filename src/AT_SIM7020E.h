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

AT Command Dictionary for SIMCOM SIM7020E version 1.3.1
support SIMCOM SIM7020E
NB-IoT with AT command

Author: Device Innovation team  
Create Date: 2 January 2020. 
Modified: 30 April 2020.
*/

#include <Arduino.h>
#include <Stream.h>

struct pingRESP{
	bool status;
	String addr;
	String ttl;
	String rtt;
};

struct radio{
	String pci="";
	String rsrp="";
	String rsrq="";
	String snr="";
};

typedef void (*MQTTClientCallback)(String &topic, String &payload, String &QoS, String &retained);
typedef void (*reponseCallback)(String &datax);

class AT_SIM7020E{
public:
	AT_SIM7020E();
	bool debug;	
	//--------- Parameter config ---------------
	const unsigned int msgLenMul=2;
	const unsigned int mqttCmdTimeout=1200;
	const unsigned int mqttBuffSize=1024;
	//=========Initialization Module=======
	void setupModule(String port="",String address="");
	void check_module_ready();	
	void reboot_module();
	pingRESP pingIP(String IP);
	bool closeUDPSocket();
	bool NBstatus();
	bool attachNetwork();
	void powerSavingMode(unsigned int psm);

	//==========Get Parameter Value=========
	String getFirmwareVersion();
	String getIMEI();
	String getICCID();
	String getIMSI();
	String getDeviceIP();
	String getSignal();    
	String getAPN();
	String getNetworkStatus();
	radio getRadioStat();
	bool checkPSMmode();
	bool MQTTstatus();
	//==========Data send/rec.===============
	void waitResponse(String &retdata,String server);
	void _Serial_print(String address,String port,unsigned int len);
	void _Serial_print(String input);
	void _Serial_print(unsigned int data);
	void _Serial_print(char*);
	void _Serial_println();
	//===============Utility=================
	void _serial_flush();	
	//================MQTT===================
	void disconnectMQTT();
	bool newMQTT(String server, String port);
	bool sendMQTTconnectionPacket(String clientID,String username,String password,int keepalive, int version,int cleansession, int willflag, String willOption);
	void publish(String topic, String payload, unsigned int qos, unsigned int retained, unsigned int dup);
	bool subscribe(String topic, unsigned int qos);
	void unsubscribe(String topic);
	unsigned int MQTTresponse();
	String retTopic;
  	String retPayload;
  	String retQoS;
  	String retRetained;
  	int setCallback(MQTTClientCallback callbackFunc);
  	//============ callback ==================
	reponseCallback callback_p;
	MQTTClientCallback MQcallback_p;
    
private:
	//==============Buffer====================
	String data_input;
	String data_buffer;
	//==============Flag======================
	bool hw_connected=false;
	bool end=false;
	//==============Parameter=================
	unsigned int previous_check=0;
	//============Counter value===============
	byte k=0;
	//==============Function==================
	void echoOff();
	bool setPhoneFunction();
	void connectNetwork();
	bool createUDPSocket(String address,String port);
	void manageResponse(String &retdata,String server);
	bool enterPIN();
	void printHEX(char *str);
	

protected:
	Stream *_Serial;	
};
