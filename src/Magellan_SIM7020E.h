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
    void powerSavingMode(unsigned int psm);

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