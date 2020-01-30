/*
AIS_NB_SIM7020E v1.0.0 NB-IoT.
support SIMCOM SIM7020E
NB-IoT with AT command
 
Author: Device Innovation team     
Create Date: 2 January 2020. 
Modified: 21 January 2020.

Released for private usage.
*/
#include "AIS_NB_SIM7020E.h"

AT_SIM7020E atcmd;
void event_null(char *data){}

/****************************************/
/**          Initialization            **/
/****************************************/

AIS_NB_SIM7020E::AIS_NB_SIM7020E(){
	Event_debug =  event_null;
}

void AIS_NB_SIM7020E:: setupDevice(String addressI,String serverdesport){
	atcmd.debug = debug;
	atcmd.setupModule(addressI,serverdesport);
}

void AIS_NB_SIM7020E::pingIP(String IP){
	atcmd.pingIP(IP);
}

/****************************************/
/**          Send UDP Message          **/
/****************************************/

void AIS_NB_SIM7020E::sendMsgHEX(String address,String desport,String payload){
	if(payload.length()>1024){
		Serial.println(F("Warning payload size exceed the limit. [Limit of HEX is 1024]"));
	}
	else send_msg(address,desport,payload.length(),payload);
	
}

void AIS_NB_SIM7020E::sendMsgSTR(String address,String desport,String payload){
	if(payload.length()>512){
		Serial.println(F("Warning payload size exceed the limit. [Limit of String is 512]"));
	}
	else send_msg(address,desport,0,"\""+payload+"\"");
}

void AIS_NB_SIM7020E::send_msg(String address,String desport,unsigned int len,String payload){
	Serial.println(F("-------------------------------"));
    Serial.print(F("# Sending Data : "));
    Serial.println(payload);
    Serial.print(F("# IP : "));
    Serial.println(address);
    Serial.print(F("# Port : "));
    Serial.println(desport);

    atcmd._Serial_print(address,desport,len);
    atcmd._Serial_print(payload);
    atcmd._Serial_println();
}

/****************************************/
/**         Receive UDP Message        **/
/****************************************/

void AIS_NB_SIM7020E::waitResponse(String &retdata,String server){
  	atcmd.waitResponse(retdata,server);  	
}

/****************************************/
/**          Get Parameter Value       **/
/****************************************/
/*
  - getSignal
        - Get NB-IoT signal
  - getDeviceIP
  		- Get device ip after connected to network.
*/
String AIS_NB_SIM7020E::getSignal(){
	return atcmd.getSignal();
}

String AIS_NB_SIM7020E::getDeviceIP(){
	return atcmd.getDeviceIP();
}

void AIS_NB_SIM7020E::powerSavingMode(unsigned int psm){
  atcmd.powerSavingMode(psm);
}

bool  AIS_NB_SIM7020E::checkPSMmode(){
  return atcmd.checkPSMmode();
}
