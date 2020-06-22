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

Magellan_SIM7020E v1.4.0 NB-IoT Magellan Platform .
support SIMCOM SIM7020E
NB-IoT with AT command

Library/SDK has developed with CoAP protocol. 
reference with 
https://tools.ietf.org/html/rfc7252

support post and get method
and supported only Magellan IoT Platform 

*** the payload has limit with the dynamic memory of your board   
 
Author: Device Innovation team     
Create Date: 2 January 2020. 
Modified: 1 April 2020.

Released for private usage.
*/

#ifndef Magellan_SIM7020E_h
#define Magellan_SIM7020E_h

#include <Arduino.h>
#include "AT_SIM7020E.h"

  const byte maxretrans=4;	

  //-------Msg Type------------
  const char con[]="40";
  const char contk[]="42";
  const char non_con[]="50";
  const char ack[]="60";
  const char acktk[]="62";
  const char rst[]="70";

  //-------Method Type---------
  //const char EMPTY[]="00";
  const char GET[]="01";
  const char POST[]="02";
  const char PUT[]="03";
  const char DELETE[]="04";


enum rspcode {
	EMPTY=00,
	CREATED=65,
	DELETED=66,
	VALID=67,
	CHANGED=68,
	CONTENT=69,
	CONTINUE=95,
	BAD_REQUEST=128,
	FORBIDDEN=131,
	NOT_FOUND=132,
	METHOD_NOT_ALLOWED=133,
	NOT_ACCEPTABLE=134,
	REQUEST_ENTITY_INCOMPLETE=136,
	PRECONDITION_FAILED=140,
	REQUEST_ENTITY_TOO_LARGE=141,
	UNSUPPORTED_CONTENT_FORMAT=143,
	INTERNAL_SERVER_ERROR=160,
	NOT_IMPLEMENTED=161,
	BAD_GATEWAY=162,
	SERVICE_UNAVAILABLE=163,
	GATEWAY_TIMEOUT=164,
	PROXY_NOT_SUPPORTED=165
};

typedef struct option {
    String stroption;
    unsigned int optlen;
    unsigned int optionnum;
} option; 

class Magellan_SIM7020E
{


public:
	Magellan_SIM7020E();

	bool debug;
	bool printstate=true;
    //-------------------- Magellan Platform -------------------------
    bool begin();
    String getSignal();
    radio getRadioStat();
    void powerSavingMode(unsigned int psm);
    pingRESP pingIP(String IP);

    String thingsRegister();
    String report(String payload,unsigned int qos=0);
    String getConfig(String Resource,unsigned int qos=0);
    String getControl(String Resource,unsigned int qos=0); 

private:

	unsigned int Msg_ID=0;
	//------------------------- Buffer --------------------------
	String data_input="";
	String rcvdata="";
	String imei="";
	String imsi="";
	String iccid="";
	String deviceIP;
	String model;
	String data_buffer="";
	String data_resp="";
	String Token="";
	//--------------------- counter value ---------------------
	byte k=0;
	//------------------------- flag --------------------------
	bool breboot_flag=false;
	bool end=false;
	bool flag_rcv=true;
	bool en_get=true;
	bool en_post=true;
	bool en_send=true;
	bool getpayload=false;
	bool sendget=false;
	bool NOTFOUND=false;
	bool GETCONTENT=false;
	bool RCVRSP=false;
	bool success=false;
	bool connected=false;
	bool get_process=false;
	bool post_process=false;
	bool ACK=false;
	bool EMP=false;
	bool token_error_report=true;
	bool token_error_config=true;
	//------------- message token and ID --------------
	unsigned int resp_msgID=0;
	unsigned int post_ID=0;
	unsigned int post_token=0;
	unsigned int get_ID=0;
	unsigned int get_token=0;
	//-------------------- timer ----------------------
	unsigned int previous_send=0;
	unsigned int previous_get=0;
	unsigned int token=0;
	unsigned int rsptoken=0;
	byte count_post_timeout=0;
	byte count_get_timeout=0;
	byte count_error_token_post=0;
	byte count_error_token_get=0;

	//------------- Diagnostic & Report ---------------
	byte cnt_cmdgetrsp=0;
	//---------------------- Function -------------------
    void printHEX(char *str);
    void printMsgID(unsigned int messageID);
	void printRspHeader(String Msgstr);
	void printRspType(String Msgstr,unsigned int msgID);
	void printPathlen(unsigned int path_len,String init_str);
	void printUriPath(String uripath,String optnum);
	void printErrCode(String errcode);

	//---------------------- Response Management --------
	void manageResponse(String rx);
	void rspPrintOut(String rx);

	//--------------------- Message management ----------
	void msgPost(String payload,option *coapOption,unsigned int totaloption);
	void msgGet(option *coapOption,unsigned int totaloption,String Proxy);
	String postData(String payload,option *coapOption,unsigned int totaloption);
	String getData(option *coapoption,unsigned int totaloption,String Proxy);

protected:
	 Stream *_Serial;
};

#endif