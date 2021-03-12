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

AIS_SIM7020E_API v1.1.0 NB-IoT.
support SIMCOM SIM7020E
NB-IoT with AT command
 
Author: Device Innovation team     
Create Date: 2 January 2020. 
Modified: 30 April 2020.

Released for private usage.
*/

#include "AIS_SIM7020E_API.h"

AT_SIM7020E atcmd;
void event_null(char *data){}

/****************************************/
/**          Initialization            **/
/****************************************/

AIS_SIM7020E_API::AIS_SIM7020E_API(){
  Event_debug =  event_null;
}

void AIS_SIM7020E_API:: begin(String addressI,String serverdesport){
  atcmd.debug = debug;
  atcmd.setupModule(addressI,serverdesport);
}

pingRESP AIS_SIM7020E_API::pingIP(String IP){
  return atcmd.pingIP(IP);
}

/****************************************/
/**          Send UDP Message          **/
/****************************************/
/*
  - sendMsgHEX
      - Send messege in HEX string. Limit of is 1024.
  - sendMsgSTR
      - Send messege in ASCII string. Limit of is 512.
  - send_msg
      - Send messege to the server.
*/
void AIS_SIM7020E_API::sendMsgHEX(String address,String desport,String payload){
  if(payload.length()>1024){
    Serial.println(F("Warning payload size exceed the limit. [Limit of HEX is 1024]"));
  }
  else send_msg(address,desport,payload.length(),payload);
  
}

void AIS_SIM7020E_API::sendMsgSTR(String address,String desport,String payload){
  if(payload.length()>512){
    Serial.println(F("Warning payload size exceed the limit. [Limit of String is 512]"));
  }
  else send_msg(address,desport,0,"\""+payload+"\"");
}

void AIS_SIM7020E_API::send_msg(String address,String desport,unsigned int len,String payload){
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

void AIS_SIM7020E_API::waitResponse(String &retdata,String server){
    atcmd.waitResponse(retdata,server);   
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
String AIS_SIM7020E_API::getSignal(){
  return atcmd.getSignal();
}

radio AIS_SIM7020E_API::getRadioStat(){
  return atcmd.getRadioStat();
}

String AIS_SIM7020E_API::getDeviceIP(){
  return atcmd.getDeviceIP();
}

String AIS_SIM7020E_API::getIMSI(){
  return atcmd.getIMSI();
}

void AIS_SIM7020E_API::powerSavingMode(unsigned int psm){
  atcmd.powerSavingMode(psm);
}

bool AIS_SIM7020E_API::checkPSMmode(){
  return atcmd.checkPSMmode();
}

bool AIS_SIM7020E_API::NBstatus(){
  return atcmd.NBstatus();
}

bool AIS_SIM7020E_API::MQTTstatus(){
  return atcmd.MQTTstatus();
}

dateTime AIS_SIM7020E_API::getClock(unsigned int timezone){
  return atcmd.getClock(timezone);
}

/****************************************/
/**                MQTT                **/
/****************************************/
/*
  - setupMQTT
      - setup module to use MQTT include serverIP, port, clientID, username, password, keep alive interval, will messege.
  - connectMQTT
      - setup module to use MQTT include serverIP, port, clientID, username, password
  - connectAdvanceMQTT
      - setup module to use MQTT include serverIP, port, clientID, username, password, keep alive interval, will messege. This function doesn't have default value as 0.
  - newMQTT
      - connect device to MQTT server and port
  - sendMQTTconnectionPacket
      - connect device to MQTT with configuration value
  - willConfig
      - create payload for will messege
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
bool AIS_SIM7020E_API::setupMQTT(String server,String port,String clientID,String username,String password,int keepalive, int version,int cleansession, int willflag, String willOption){
  bool conStatus=false;
  if(username.length() > 100 || password.length() > 100){
    Serial.println(F("Username/Password is over 100."));
  }
  else if(username=="" && password!=""){
    Serial.println(F("Username is missing."));
  }
  else if(clientID.length() > 120 || clientID==""&&cleansession!=1){
    Serial.println(F("ClientID is over 120 or ClientID is missing."));
  }
  else if(server==""||port==""){
    Serial.println(F("Address or port is missing."));
  }
  else if(version > 4 || version < 3){
    Serial.println(F("Version must be 3 (MQTT 3.1) or 4 (MQTT 3.1.1)"));
  }
  else if(willflag==1&&willOption==""){
    Serial.println(F("Missing will option."));
  }
  else{
    Serial.print(F("# ServerIP : "));
    Serial.println(server);
    Serial.print(F("# Port : "));
    Serial.println(port);
    Serial.print(F("# ClientID : "));
    Serial.println(clientID);

    if(MQTTstatus()) atcmd.disconnectMQTT();

    if(newMQTT(server, port)){
      if(atcmd.sendMQTTconnectionPacket(clientID,username,password,keepalive,version,cleansession,willflag,willOption)){
        flag_mqtt_connect=true;
      }
      else {
        Serial.println(F("Please check your parameter again."));
      }
    }
    else {
      Serial.println(F("Please check your server/port."));
    } 
  }
  atcmd._serial_flush();
  return flag_mqtt_connect;
}

bool AIS_SIM7020E_API::connectMQTT(String server,String port,String clientID,String username,String password){
  return setupMQTT(server,port,clientID,username,password,60,3,1,0,"");
}

bool AIS_SIM7020E_API::connectAdvanceMQTT(String server,String port,String clientID,String username,String password,int keepalive, int version,int cleansession, int willflag, String willOption){
  return setupMQTT(server,port,clientID,username,password,keepalive,version,cleansession,willflag,willOption);
}

bool  AIS_SIM7020E_API::newMQTT(String server, String port){
  return atcmd.newMQTT(server, port);
}

bool  AIS_SIM7020E_API::sendMQTTconnectionPacket(String clientID,String username,String password,int keepalive, int version,int cleansession, int willflag, String willOption){
  return atcmd.sendMQTTconnectionPacket(clientID,username,password,keepalive,version,cleansession,willflag,willOption);
}

String AIS_SIM7020E_API::willConfig(String will_topic, unsigned int will_qos,unsigned int will_retain,String will_msg){
  char data[will_msg.length()+1];
  memset(data,'\0',will_msg.length());
  will_msg.toCharArray(data,will_msg.length()+1);
  int len = will_msg.length()*atcmd.msgLenMul;
  String msg;

  char *hstr;
  hstr=data;
  char out[3];
  memset(out,'\0',2);
  bool flag=false;
  while(*hstr){
    flag=itoa((int)*hstr,out,16);    
    if(flag){
      msg+=out; 
    }
    hstr++;
  }
  return "\"topic="+will_topic+",QoS="+String(will_qos)+",retained="+String(will_retain)+",message_len="+String(len)+",message="+msg+"\"";
}

bool AIS_SIM7020E_API::publish(String topic, String payload, unsigned int pubQoS, unsigned int pubRetained, unsigned int pubDup){
  if(topic==""){
    Serial.println(F("Topic is missing."));
    return false;
  }
  if(payload.length()*atcmd.msgLenMul>1000){
    Serial.println(F("Payload hex string is over 1000."));
    return false;
  }

  Serial.println(F("-------------------------------"));
  Serial.print(F("# Publish : "));
  Serial.println(payload);
  Serial.print(F("# Topic   : "));
  Serial.println(topic);

  atcmd.publish(topic, payload, pubQoS, pubRetained, pubDup);
  while(1){
  unsigned int a = atcmd.MQTTresponse();
    if(a==2){
      return true;
    }
    else if(a==3){
      return false;
    }
  }  
}

bool AIS_SIM7020E_API::subscribe(String topic, unsigned int subQoS){
  
  if(topic==""){
    Serial.println(F("Topic is missing."));
    return false;
  }

  Serial.println(F("-------------------------------"));
  Serial.println(F("# Subscribe "));
  Serial.print(F("# Topic : "));
  Serial.println(topic);  
  
  atcmd._serial_flush();
  atcmd.subscribe(topic, subQoS);
  byte c=0;
  while(1){
    delay(80);
    unsigned int a = atcmd.MQTTresponse();
    if(a==2){
      return true;
    }
    else if(a==3){
      if(c>2) return false;
      else{ 
        atcmd.subscribe(topic, subQoS);
        c++;
      }
    }
  }
  atcmd._serial_flush(); 
}

void AIS_SIM7020E_API::unsubscribe(String topic){
  if(topic==""){
    Serial.println(F("Topic is missing."));
  }
  else{
    atcmd.unsubscribe(topic);
  }
}

void AIS_SIM7020E_API::MQTTresponse(){
  atcmd.MQTTresponse();
}

int AIS_SIM7020E_API::setCallback(MQTTClientCallback callbackFunc){
  return atcmd.setCallback(callbackFunc);
}

/****************************************/
/**               Utility              **/
/****************************************/
/*
  - toString 
      - change hex to string
  - char_to_byte
      - use in function toString
*/
String AIS_SIM7020E_API::toString(String dat){
  String str="";
  for(byte x=0;x<dat.length();x+=2){
      char c =  char_to_byte(dat[x])<<4 | char_to_byte(dat[x+1]);
    str += c;
  }
  return(str);
}

char AIS_SIM7020E_API:: char_to_byte(char c){
  if((c>='0')&&(c<='9')){
    return(c-0x30);
  }
  if((c>='A')&&(c<='F')){
    return(c-55);
  }
}
