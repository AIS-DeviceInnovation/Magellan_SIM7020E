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

#include "Magellan_SIM7020E.h"

AT_SIM7020E at_udp;

String serverIP = "119.31.104.1";            
String port = "5683";                       

Magellan_SIM7020E::Magellan_SIM7020E(){
}

/****************************************/
/**          Magellan API             **/
/****************************************/
/*  - thingsRegister
        - Register Device using CoAP Protocol with AIS SIM/eSIM
        - POST register/sim/v1/{IMSI}/{Device IPAddress}
    - report          
        - Send Data using CoAP Protocol with AIS SIM/eSIM
        - POST report/sim/v1/{Token}/{Device IP Address}
    - getConfig       
        - Get Configuration which specified at Magellan using CoAP Protocol
        - GET config/sim/v1/{Token}/{Device IP Address}
    - getControl
        - Get Data from Magellan using CoAP Protocol
        - GET delta/sim/v1/{Token}/{IPAddress}
*/
String Magellan_SIM7020E::thingsRegister()
{
  Serial.println(F("--------registerThings---------"));
  option coapoption[5];
  unsigned int totaloption=5;

  String stropt[5];
  
  stropt[0]=F("register");
  coapoption[0].stroption=stropt[0];
  coapoption[0].optlen=stropt[0].length();
  coapoption[0].optionnum=11;

  stropt[1]=F("sim");
  coapoption[1].stroption=stropt[1];
  coapoption[1].optlen=stropt[1].length();
  coapoption[1].optionnum=11;

  stropt[2]=F("v1");
  coapoption[2].stroption=stropt[2];
  coapoption[2].optlen=stropt[2].length();
  coapoption[2].optionnum=11;

  if (imsi.indexOf(F("52003"))!=-1 and imsi.length()>15)
  {
    int indexst=imsi.indexOf(F("52003"));
    imsi=imsi.substring(indexst,16);
  }
  stropt[3]=imsi;
  
  coapoption[3].stroption=stropt[3];
  coapoption[3].optlen=stropt[3].length();
  coapoption[3].optionnum=11;

  stropt[4]=deviceIP;
  coapoption[4].stroption=stropt[4];
  coapoption[4].optlen=stropt[4].length();
  coapoption[4].optionnum=11;

  Token=postData("",coapoption,totaloption);

  if(Token.length()==36)
  {
    token_error_report=true;
    token_error_config=true;
  }

  return Token;
}

String Magellan_SIM7020E::report(String payload,unsigned int qos)
{
  String Response_Report="";
  if (Token=="" || Token.length()<36){
    Serial.println(F("Miss Token, Trying register..."));
    while(true){
      thingsRegister();
      if(success&&token_error_report){
        break;
      }
    }
  }else if(payload.length()>300){
    Serial.println(F("Warning payload size exceed the limit. [Limit 300 characters]"));
  }
  else{
    option coapoption[5];
    unsigned int totaloption=5;

    String stropt[5];

    stropt[0]=F("report");
    coapoption[0].stroption=stropt[0];
    coapoption[0].optlen=stropt[0].length();
    coapoption[0].optionnum=11;

    stropt[1]=F("sim");
    coapoption[1].stroption=stropt[1];
    coapoption[1].optlen=stropt[1].length();
    coapoption[1].optionnum=11;

    stropt[2]=F("v1");
    coapoption[2].stroption=stropt[2];
    coapoption[2].optlen=stropt[2].length();
    coapoption[2].optionnum=11;

    stropt[3]=Token;
    coapoption[3].stroption=stropt[3];
    coapoption[3].optlen=stropt[3].length();
    coapoption[3].optionnum=11;

    stropt[4]=deviceIP;
    coapoption[4].stroption=stropt[4];
    coapoption[4].optlen=stropt[4].length();
    coapoption[4].optionnum=11;

    if(qos==0)
    {
      Response_Report=postData(payload,coapoption,totaloption);
    }

    if(qos>=1){
      while(true){
        Response_Report=postData(payload,coapoption,totaloption);
        if(success&&token_error_report){
          break;
        }
      }
    }
    
  }
  return Response_Report; 
}

String Magellan_SIM7020E::getConfig(String Resource,unsigned int qos)
{
  String Response_Config="";
  if (Token=="" || Token.length()<36){
    Serial.println(F("Miss Token, Trying register..."));
    while(true){
      thingsRegister();
      if(success&&token_error_report){
        break;
      }
    }
  }
  else{
    option coapoption[6];
    unsigned int totaloption=6;

    String stropt[6];

    stropt[0]=F("config");
    coapoption[0].stroption=stropt[0];
    coapoption[0].optlen=stropt[0].length();
    coapoption[0].optionnum=11;

    stropt[1]=F("sim");
    coapoption[1].stroption=stropt[1];
    coapoption[1].optlen=stropt[1].length();
    coapoption[1].optionnum=11;

    stropt[2]=F("v1");
    coapoption[2].stroption=stropt[2];
    coapoption[2].optlen=stropt[2].length();
    coapoption[2].optionnum=11;

    stropt[3]=Token;
    coapoption[3].stroption=stropt[3];
    coapoption[3].optlen=stropt[3].length();
    coapoption[3].optionnum=11;

    stropt[4]=deviceIP;
    coapoption[4].stroption=stropt[4];
    coapoption[4].optlen=stropt[4].length();
    coapoption[4].optionnum=11;

    stropt[5]=Resource;
    coapoption[5].stroption=stropt[5];
    coapoption[5].optlen=stropt[5].length();
    coapoption[5].optionnum=15;

    Response_Config=getData(coapoption,totaloption,"");
    
  }

  return Response_Config;

  }

String Magellan_SIM7020E::getControl(String Resource,unsigned int qos)
{
  String Response_Control="";
  if (Token=="" || Token.length()<36){
    Serial.println(F("Miss Token, Trying register..."));
    while(true){
      thingsRegister();
      if(success&&token_error_report){
        break;
      }
    }
  }
  else{
    option coapoption[6];
    unsigned int totaloption=6;

    String stropt[6];

    stropt[0]=F("delta");
    coapoption[0].stroption=stropt[0];
    coapoption[0].optlen=stropt[0].length();
    coapoption[0].optionnum=11;

    stropt[1]=F("sim");
    coapoption[1].stroption=stropt[1];
    coapoption[1].optlen=stropt[1].length();
    coapoption[1].optionnum=11;

    stropt[2]=F("v1");
    coapoption[2].stroption=stropt[2];
    coapoption[2].optlen=stropt[2].length();
    coapoption[2].optionnum=11;

    stropt[3]=Token;
    coapoption[3].stroption=stropt[3];
    coapoption[3].optlen=stropt[3].length();
    coapoption[3].optionnum=11;

    stropt[4]=deviceIP;
    coapoption[4].stroption=stropt[4];
    coapoption[4].optlen=stropt[4].length();
    coapoption[4].optionnum=11;

    stropt[5]=Resource;
    coapoption[5].stroption=stropt[5];
    coapoption[5].optlen=stropt[5].length();
    coapoption[5].optionnum=15;

    Response_Control=getData(coapoption,totaloption,"");
    
  }
  return Response_Control;
}


/****************************************/
/**          Initialization            **/
/****************************************/

bool Magellan_SIM7020E::begin(){
  //bool created=true;
  if(debug) at_udp.debug=true;
  bool created=false;  
  token_error_report=true;
  token_error_config=true;
  Serial.println();
  Serial.println(F("               AIS NB-IoT Magellan_SIM7020E V1.4.0"));

  at_udp.setupModule(serverIP,port);

  imsi.reserve(15);
  imsi = at_udp.getIMSI();

  iccid.reserve(19);
  iccid = at_udp.getICCID();

  at_udp.getIMEI();

  deviceIP = at_udp.getDeviceIP();

  while(true)
  {
    thingsRegister();
    if(success&&token_error_report)
    {
      break;
    }
  }

  return created;
}

/*************************************************/
/**Message management and CoAP message construct**/
/*************************************************/
void Magellan_SIM7020E::printHEX(char *str){
  char *hstr;
  hstr=str;
  char out[3];
  memset(out,'\0',2);
  int i=0;
  bool flag=false;
  while(*hstr){
    flag=itoa((int)*hstr,out,16);
    
    if(flag){
      at_udp._Serial_print(out);

      if(debug){
        Serial.print(out);
      }      
    }
    hstr++;
  }
}

void Magellan_SIM7020E::printMsgID(unsigned int messageID){
  char Msg_ID[3];
  
  utoa(highByte(messageID),Msg_ID,16);
  if(highByte(messageID)<16){
    if(debug) Serial.print(F("0"));
    at_udp._Serial_print(F("0"));
    if(debug) Serial.print(Msg_ID);
    at_udp._Serial_print(Msg_ID);
  }
  else{
    at_udp._Serial_print(Msg_ID);
    if(debug)  Serial.print(Msg_ID);
  }

  utoa(lowByte(messageID),Msg_ID,16);
  if(lowByte(messageID)<16){
    if(debug)  Serial.print(F("0"));
    at_udp._Serial_print(F("0"));
    if(debug)  Serial.print(Msg_ID);
    at_udp._Serial_print(Msg_ID);
  }
  else{
    at_udp._Serial_print(Msg_ID);
    if(debug)  Serial.print(Msg_ID);
  }
}

void Magellan_SIM7020E::printPathlen(unsigned int path_len,String init_str)
{   
    unsigned int extend_len=0;
    if(path_len>=13){
      extend_len=path_len-13;

      char extend_L[3];
      itoa(lowByte(extend_len),extend_L,16);
      at_udp._Serial_print(init_str);
      at_udp._Serial_print(F("d"));

      if(debug) Serial.print(init_str);
      if(debug) Serial.print(F("d"));

      if(extend_len<=15){
        at_udp._Serial_print(F("0"));
        at_udp._Serial_print(extend_L);


        if(debug) Serial.print(F("0"));
        if(debug) Serial.print(extend_L);
      }
      else{
        at_udp._Serial_print(extend_L);
        if(debug) Serial.print(extend_L);
      }
      

    }
    else{
      if(path_len<=9)
      {
        char hexpath_len[2];
        memset(hexpath_len,'\0',1);   
        sprintf(hexpath_len,"%i",path_len);
        at_udp._Serial_print(init_str);
        at_udp._Serial_print(hexpath_len);
        if(debug) Serial.print(init_str);
        if(debug) Serial.print(hexpath_len);

      }
      else
      {
        if(path_len==10) 
        {
          at_udp._Serial_print(init_str);
          at_udp._Serial_print(F("a"));
          if(debug) Serial.print(init_str);
          if(debug) Serial.print(F("a"));
        }
        if(path_len==11)
        {
          at_udp._Serial_print(init_str);
          at_udp._Serial_print(F("b"));
          if(debug) Serial.print(init_str);
          if(debug) Serial.print(F("b"));
        } 
        if(path_len==12)
        {
          at_udp._Serial_print(init_str);
          at_udp._Serial_print(F("c"));
          if(debug) Serial.print(init_str);
          if(debug) Serial.print(F("c"));
        } 
        if(path_len==13)
        {
          at_udp._Serial_print(init_str);
          at_udp._Serial_print(F("d"));
          if(debug) Serial.print(init_str);
          if(debug) Serial.print(F("d"));
        } 
        
      }
   }  
}

void Magellan_SIM7020E::printUriPath(String uripath,String optnum)
{
    unsigned int uripathlen=uripath.length();

    if (uripathlen>0)
      {
          printPathlen(uripathlen,optnum);
          
          char data[uripath.length()+1];
          memset(data,'\0',uripath.length());
          uripath.toCharArray(data,uripath.length()+1);

          printHEX(data);
      }
}

void Magellan_SIM7020E::msgPost(String payload,option *coapOption,unsigned int totaloption)
{
  at_udp._serial_flush();
  option *stroption1;
  stroption1=coapOption;

  option *stroption2;
  stroption2=coapOption;

  char data[payload.length()+1];
  memset(data,'\0',payload.length());
  payload.toCharArray(data,payload.length()+1);
  
  unsigned int headerLen=2;
  unsigned int tokenLen=2;
  unsigned int msgIdLen=2;
  unsigned int paylodMakerLen=2;

  if(en_post){

      if(printstate) Serial.print(F(">> post: Msg_ID "));
      if(printstate) Serial.print(Msg_ID);
      if(printstate) Serial.print(F(" "));
      if(printstate) Serial.print(payload);

      unsigned int buff_len=headerLen+tokenLen+msgIdLen+paylodMakerLen; //header(2) + token(2) + msgID(2) +payloadmaker(2) 

      buff_len+=payload.length(); // add payload lenght

      if (payload.length()>0)
      {
        buff_len+=1;
      }

      for(unsigned int i=0;i<totaloption;i++)
      {
        buff_len+=stroption1->optlen+1;
        if(stroption1->optlen>13)
        {
          buff_len+=1;
        }
        stroption1++;
      }

      at_udp._Serial_print(serverIP,port,buff_len*at_udp.msgLenMul);

      if(debug) Serial.print(buff_len*at_udp.msgLenMul);
      if(debug) Serial.print(F(",4202"));

      at_udp._Serial_print(F("4202"));

      printMsgID(Msg_ID);                         // 2
      printMsgID(post_token);                     //print token 2
      
      unsigned int lastopt=0;
      unsigned int optnum=0;
      unsigned int outopt=0;

      for(unsigned int i=0;i<totaloption;i++)
      { 
        optnum=stroption2->optionnum;
        outopt=optnum-lastopt;
        String init_opt;

        lastopt=optnum;        

        switch(outopt)
        {
          case 10:
            init_opt=F("a");
            break;

          case 11:
            init_opt=F("b");
            break;

          case 12:
            init_opt=F("c");
            break;

          case 13:
            init_opt=F("d");
            break;

          case 14:
            init_opt=F("e");
            break;

          case 15:
            init_opt=F("f");
            break;

          default:
                init_opt=String(outopt);
          
        }

        printUriPath(stroption2->stroption,init_opt);        

        stroption2++;
      }

      at_udp._Serial_print(F("1132"));                 //content-type json 2
      if(debug) Serial.print(F("1132"));               //content-type json 2
      if (payload.length()>0)
      {
        at_udp._Serial_print(F("ff")); 
        if(debug) Serial.print(F("ff"));             
        printHEX(data);  
      }
      at_udp._Serial_println();
      if(printstate) Serial.println();
  }   
}

void Magellan_SIM7020E::msgGet(option *coapOption,unsigned int totaloption,String Proxy)
{
 
  option *stroption1;
  stroption1=coapOption;

  option *stroption2;
  stroption2=coapOption;

  String Resource="";

  char data[Resource.length()+1];
  memset(data,'\0',Resource.length());
  Resource.toCharArray(data,Resource.length()+1); 
  GETCONTENT=false;
  ACK=false;

  unsigned int headerLen=2;
  unsigned int tokenLen=2;
  unsigned int msgIdLen=2;
  unsigned int paylodMakerLen=2;

  if(printstate) Serial.print(F(">> GET data : Msg_ID "));
  if(printstate) Serial.print(Msg_ID);
  if(printstate) Serial.print(F(" "));
  if(printstate) Serial.print(Resource); 

  unsigned int path_len=Resource.length();
  unsigned int buff_len=headerLen+tokenLen+msgIdLen+paylodMakerLen; //header(2) + token(2) + msgID(2) +payloadmaker(2)

  for(unsigned int i=0;i<totaloption;i++)
  {
    buff_len+=stroption1->optlen+1;
    if(stroption1->optlen>13)
    {
      buff_len+=1;
    }
    stroption1++;
  }

  at_udp._Serial_print(serverIP,port,buff_len*at_udp.msgLenMul);

  if (debug) Serial.print(buff_len*at_udp.msgLenMul);
  if(debug) Serial.print(F(",4201"));
  at_udp._Serial_print(F("4201"));
  
  printMsgID(Msg_ID);                        //send msg ID to connectivity module
  printMsgID(get_token);                     //send msg token to connectivity module

  unsigned int lastopt=0;
  unsigned int optnum=0;
  unsigned int outopt=0;

  for(unsigned int i=0;i<totaloption;i++)
  { 
    optnum=stroption2->optionnum;
    outopt=optnum-lastopt;
    String init_opt;

    lastopt=optnum;

    switch(outopt)
    {
      case 10:
        init_opt=F("a");
        break;

      case 11:
        init_opt=F("b");
        break;

      case 12:
        init_opt=F("c");
        break;

      case 13:
        init_opt=F("d");
        break;

      case 14:
        init_opt=F("e");
        break;

      case 15:
        init_opt=F("f");
        break;

      default:
            init_opt=String(outopt);
      
    }

    printUriPath(stroption2->stroption,init_opt);

    stroption2++;
  }

  //at_udp._Serial_print(F("8104"));          //Block size 256 with CoAP Header
  //if(debug) Serial.print(F("8104"));          

  //at_udp._Serial_print(F("8105"));        //Block size 512
  //if(debug) Serial.print(F("8105"));
  at_udp._Serial_print(F("8106"));        //Block size 1024
  if(debug) Serial.print(F("8106"));
  

  at_udp._Serial_println();

  at_udp._serial_flush();

  if(printstate) Serial.println();
  sendget=true;
}
/****************************************/
/**      CoAP Sequence management      **/
/****************************************/
String Magellan_SIM7020E::postData(String payload,option *coapOption,unsigned int totaloption)
{
  unsigned int timeout[5]={12000,14000,18000,26000,42000};
  rcvdata=""; //recieve response from server variable
  data_buffer=""; 
  String server=serverIP;
  if(!get_process && en_post){
    
    previous_send=millis();
    ACK=false;
    success=false;
    
    token=random(0,32767);   //random message token
    post_token=token;

    if(debug) Serial.println(F("Load new payload"));
    
    Msg_ID=random(0,65535);                  //random message ID
    post_ID=Msg_ID;
    
    for (byte i = 0; i <= maxretrans; ++i)
    {
      
      post_process=true;
      
      msgPost(payload,coapOption,totaloption);  // Construct CoAP message

      while(true)
      {
        at_udp.waitResponse(data_resp,server);
        manageResponse(data_resp);

        unsigned int currenttime=millis();
        if (currenttime-previous_send>timeout[i] || success)
        {
          previous_send=currenttime;
          en_post=true;
          en_get=true;
          post_process=false;
          break;
        }
      }
      if (success)
      {
        break;
      }
      else{
            if(i+1<5){
              if(printstate) Serial.print(F(">> Retransmit"));
              if(printstate) Serial.println(i+1);
              if(printstate) Serial.println(timeout[i+1]);
            }

      }

     }
     if (!success)
     {
        Serial.print(F("Post timeout : "));
        data_input="";
        count_post_timeout++;
        Serial.println(count_post_timeout);
      if(printstate) Serial.println();

        if(count_post_timeout>=3)
         {
          count_post_timeout = 0;
          ESP.restart();
         }

     }
    }  

  printErrCode(rcvdata);

  if(rcvdata.indexOf(F("20000"))!=-1) count_error_token_post=true;

  if(rcvdata.indexOf(F("40300"))!=-1||rcvdata.indexOf(F("50010"))!=-1)
  {
    token_error_report=false;
    count_error_token_post++;
    if(count_error_token_post>=10)
     {
      count_error_token_post = 0;
      ESP.restart();
      token_error_report=true;
     }
  } 
 
  post_process=false;
  at_udp._serial_flush();
  return rcvdata;
}

String Magellan_SIM7020E::getData(option *coapoption,unsigned int totaloption,String Proxy)
{
  int timeout[5]={4000,8000,16000,32000,64000};
  rcvdata="";
  data_buffer="";
  String server=serverIP;
  if(!post_process && en_get){
    previous_get =millis();
    Msg_ID=random(0,65535); 
    get_ID=Msg_ID;
    token=random(0,32767);
    get_token=token;
    success=false;
    for (byte i = 0; i <=maxretrans; ++i)
      {
        get_process=true;
        data_buffer="";
        msgGet(coapoption,totaloption,Proxy);       

        while(true)
        {
          unsigned int current_time=millis();            

          if(current_time-previous_get>timeout[i] || success || ACK || NOTFOUND){
            previous_get=current_time;
            if(i==maxretrans){
              if(printstate) Serial.println(F("Get timeout"));
              get_process=false;
              ESP.restart();
            }
            break;
          }
          at_udp.waitResponse(data_resp,server);
          manageResponse(data_resp);
        }

        if((rcvdata.length()>0 && GETCONTENT) || success || NOTFOUND){
          get_process=false;
          break;
        }
        else{
            if(printstate) Serial.print(F(">> Retransmit"));
            if(printstate) Serial.println(i+1);
            if(printstate) Serial.println(timeout[i]);
          }

      }
  }
  printErrCode(rcvdata);
  if(rcvdata.indexOf(F("40300"))!=-1 || rcvdata.indexOf(F("50010"))!=-1)
  {
    token_error_config=false;
    //count_error_token_get++;
  } 

  at_udp._serial_flush();
  return rcvdata;
}

/****************************************/
/**      CoAP Response management      **/
/****************************************/
/*-------------------------------------
    Response Message management

    Example Message
    Type: ACK
    MID: 000001
    Code: Created
    Payload:20000
  -------------------------------------
*/
void Magellan_SIM7020E::printRspHeader(String Msgstr)
{

  if(debug) Serial.println(Msgstr);

  resp_msgID = (unsigned int) strtol( &Msgstr.substring(4,8)[0], NULL, 16);
  printRspType(Msgstr.substring(0,2),resp_msgID);

  bool en_print=(post_process && resp_msgID==post_ID) || (get_process && resp_msgID==get_ID);

  switch((int) strtol(&Msgstr.substring(2,4)[0], NULL, 16))
   {
      case EMPTY:
                    EMP=true;
                    Msgstr.remove(0, 8);
                    break;
      case CREATED: 
                    EMP=false;
                    NOTFOUND=false;
                    GETCONTENT=false;
                    RCVRSP=true;

                    if (Msgstr.length()/2>4)
                    {
                      rsptoken=(unsigned int) strtol( &Msgstr.substring(8,12)[0], NULL, 16);
                      if (post_process && post_token==rsptoken)
                      {
                        if(debug) Serial.println(F("match token"));
                        if(debug) Serial.print(rsptoken);
                        success=true;
                        
                      }

                      Msgstr.remove(0, 12);
                    }
                    else{
                      Msgstr.remove(0, 8);
                    }

                    if(printstate && en_print) Serial.println(F("2.01 CREATED")); 
                    break;
      case DELETED: //if(printstate && en_print) Serial.println(F("2.02 DELETED")); 
                    break;
      case VALID: //if(printstate && en_print) Serial.println(F("2.03 VALID"));
                  break;
      case CHANGED: //if(printstate && en_print) Serial.println(F("2.04 CHANGED"));
                  break;
      case CONTENT: 
                    EMP=false;
                    NOTFOUND=false;
                    GETCONTENT=true;
                    RCVRSP=false;
                    if(get_process){

                      if (Msgstr.length()/2>4)
                      {
                        rsptoken=(unsigned int) strtol( &Msgstr.substring(8,12)[0], NULL, 16);
                        if (get_process && get_token==rsptoken)
                        {
                          if(debug) Serial.println(F("match token get"));
                          if(debug) Serial.print(rsptoken);
                          success=true;
                        }
                      }
                      Msgstr.remove(0, 8);
                    }
                    

                    if(post_process){

                      if (Msgstr.length()/2>4)
                      {
                        rsptoken=(unsigned int) strtol( &Msgstr.substring(8,12)[0], NULL, 16);
                        if (post_process && post_token==rsptoken)
                        {
                          if(debug) Serial.println(F("match token post"));
                          if(debug) Serial.print(rsptoken);
                          success=true;                          
                        }
                        Msgstr.remove(0, 12);
                      }
                      else{
                        Msgstr.remove(0, 8);
                      }
                    }

                    if(printstate && en_print) Serial.println(F("2.05 CONTENT")); 
                    break;
      case CONTINUE: //if(printstate && en_print) Serial.println(F("2.31 CONTINUE"));
                    Msgstr.remove(0, 8); 
                    break;
      //case BAD_REQUEST: if(printstate && en_print) Serial.println(F("4.00 BAD_REQUEST"));
                    //Msgstr.remove(0, 8); 
                    //break;
      //case FORBIDDEN: if(printstate && en_print) Serial.println(F("4.03 FORBIDDEN"));
                    //Msgstr.remove(0, 8); 
                    //break;
      case NOT_FOUND: 
                    if(printstate && en_print) Serial.println(F("4.04 NOT_FOUND"));
                    GETCONTENT=false; 
                    NOTFOUND=true;
                    RCVRSP=false;
                    break;
      //case METHOD_NOT_ALLOWED: 
                    //RCVRSP=false;
                    //if(printstate && en_print) Serial.println(F("4.05 METHOD_NOT_ALLOWED")); 
                    //break;
      //case NOT_ACCEPTABLE: if(printstate && en_print) Serial.println(F("4.06 NOT_ACCEPTABLE")); 
                    //break;
      case REQUEST_ENTITY_INCOMPLETE: //if(printstate && en_print) Serial.println(F("4.08 REQUEST_ENTITY_INCOMPLETE")); 
                    break;
      case PRECONDITION_FAILED: //if(printstate && en_print) Serial.println(F("4.12 PRECONDITION_FAILED")); 
                    break;
      case REQUEST_ENTITY_TOO_LARGE: //if(printstate && en_print) Serial.println(F("4.13 REQUEST_ENTITY_TOO_LARGE")); 
                    break;
      //case UNSUPPORTED_CONTENT_FORMAT: if(printstate && en_print) Serial.println(F("4.15 UNSUPPORTED_CONTENT_FORMAT")); 
                    //break;
      case INTERNAL_SERVER_ERROR: if(printstate && en_print) Serial.println(F("5.00 INTERNAL_SERVER_ERROR")); 
                    break;
      case NOT_IMPLEMENTED: //if(printstate && en_print) Serial.println(F("5.01 NOT_IMPLEMENTED")); 
                    break;
      //case BAD_GATEWAY: if(printstate && en_print) Serial.println(F("5.02 BAD_GATEWAY")); 
                    //break;
      //case SERVICE_UNAVAILABLE: if(printstate && en_print) Serial.println(F("5.03 SERVICE_UNAVAILABLE")); 
                    //break;
      //case GATEWAY_TIMEOUT: if(printstate && en_print) Serial.println(F("5.04 GATEWAY_TIMEOUT")); 
                    //break;
      //case PROXY_NOT_SUPPORTED: if(printstate && en_print) Serial.println(F("5.05 PROXY_NOT_SUPPORTED")); 
                    //break;

      default : //Optional
                GETCONTENT=false;
   }

   if(printstate && en_print) Serial.print(F("   Msg_ID "));   
   if(printstate && en_print) Serial.println(resp_msgID);
}

void Magellan_SIM7020E::printRspType(String Msgstr,unsigned int msgID)
{
  bool en_print=(post_process && resp_msgID==post_ID) || (get_process && resp_msgID==get_ID);

  if(Msgstr.indexOf(ack)!=-1 || Msgstr.indexOf(acktk)!=-1)
  {
    
    if(printstate && en_print) Serial.print(F("<< ACK: "));
    if((resp_msgID==get_ID || resp_msgID==post_ID) && !EMP){
            ACK=true;
    }
    
    flag_rcv=true;
    en_post=true;
    en_get=true;
    
    //send_ACK=false;
    cnt_cmdgetrsp=0;
  }
  if(Msgstr.indexOf(rst)!=-1)
  {
    if(printstate && en_print) Serial.print(F("<< RST: "));
    flag_rcv=true;
    ACK=false;
    cnt_cmdgetrsp=0;
  }
  if(Msgstr.indexOf(non_con)!=-1)
  {
    if(printstate && en_print) Serial.print(F("<< Non-Con: "));
    flag_rcv=true;
    ACK=false;
    cnt_cmdgetrsp=0;
  }
}
/*------------------
  Get response data  
  ------------------ 
*/
void Magellan_SIM7020E::manageResponse(String rx)
{
  if(rx.indexOf(F("FF"))!=-1 || rx.indexOf(F("ff"))!=-1)
  {
    rspPrintOut(rx);
    data_resp="";
  }
}

void Magellan_SIM7020E::rspPrintOut(String rx)
{  
  printRspHeader(rx); 

  bool en_print=(post_process && resp_msgID==post_ID) || (get_process && resp_msgID==get_ID);

  String payload_rx=rx.substring(12,rx.length());
  String data_payload="";
  unsigned int indexff=0;
  
  indexff=payload_rx.indexOf(F("FF"));  

  if(payload_rx.indexOf(F("FFF"))!=-1)
  {
      data_payload=payload_rx.substring(indexff+3,payload_rx.length());

      if(printstate && en_print) Serial.print(F("   RSP:"));
      data_buffer="";
      for(unsigned int k=2;k<data_payload.length()+1;k+=2)
      {
        char str=(char) strtol(&data_payload.substring(k-2,k)[0], NULL, 16);
        if(printstate && en_print) Serial.print(str);

        if(GETCONTENT or RCVRSP){
            if (post_process && post_token==rsptoken || get_process && get_token==rsptoken){
              data_buffer+=str;
            }
          
        }
      }
      if(GETCONTENT)
      {
        rcvdata+=data_buffer;
        data_buffer="";
        getpayload=true;
      } 
      if(printstate && en_print) Serial.println(F(""));
  }
  else
  {
      data_payload=payload_rx.substring(indexff+2,payload_rx.length());
      if(printstate && en_print) Serial.print(F("   RSP:"));
      data_buffer="";                                       //clr buffer
      for(unsigned int k=2;k<data_payload.length()+1;k+=2)
      {
        char str=(char) strtol(&data_payload.substring(k-2,k)[0], NULL, 16);
        if(printstate && en_print) Serial.print(str);

        if(GETCONTENT or RCVRSP){
          if (post_process && post_token==rsptoken || get_process && get_token==rsptoken){
            data_buffer+=str;
          }
        }
      }
      if(GETCONTENT) {
        rcvdata+=data_buffer;
        data_buffer="";
        getpayload=true;
      } 
      if(printstate && en_print) Serial.println(F(""));     
  }

  if(success)
  { 
    if(printstate && en_print) Serial.println(F("------------ End ------------"));
  }

}

/****************************************/
/**       Additional function          **/
/****************************************/
/*
  - getSignal
        - Get NB-IoT signal
  - getRadioStat
        - Get information about radio for troubleshooting
  - powerSavingMode
        - Set PSM mode for module
  - pingIP
        - ping to check network status      
*/
String Magellan_SIM7020E::getSignal(){
  return at_udp.getSignal();
}

radio Magellan_SIM7020E::getRadioStat(){
  return at_udp.getRadioStat();
}

void Magellan_SIM7020E::powerSavingMode(unsigned int psm){
  at_udp.powerSavingMode(psm);
}

pingRESP Magellan_SIM7020E::pingIP(String IP){
  return at_udp.pingIP(IP);
}

void Magellan_SIM7020E::printErrCode(String errcode){
  // switch(errcode.toInt()){
  //   case 40010:
  //       Serial.println(F("Invalid payload. The payload must be json.\n"));
  //       break;
  //   case 40300:
  //       Serial.println(F("Device has not registered to the Magellan Platform or Invalid Token."));  //--> chk each 40300   
  //       break;
  //   case 40400:
  //       Serial.println(F("Parameter not found, please check your thing in Magellan."));             // chk control+config --> err code same?
  //       break;
  //   case 50010:
  //       Serial.println(F("Device has not registered to the Magellan Platform."));
  //       break;
  //   case 40105:
  //       Serial.println(F("Account expire."));
  //       break;
  //   case 50099:
  //       Serial.println(F("Unknown error."));
  //       break;
  //   default:
  //       break;
  // }
}
