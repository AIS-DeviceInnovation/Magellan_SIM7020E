//AT Command Dictionary for SIMCOM SIM7020E version 1.1.1
#include "AT_SIM7020E.h"  
#include "board.h"

/****************************************/
/**        Initialization Module       **/
/****************************************/
AT_SIM7020E::AT_SIM7020E(){}

void AT_SIM7020E::setupModule(String address,String port){
  previous_check=millis();

	pinMode(hwResetPin, OUTPUT);   // set reset pin for hardware reboot
  
  Serial.print(F(">>Rebooting ."));                               
  reboot_module();
  Serial.println(F("..OK"));  

  if (serialConfig){
    serialPort.begin(buadrate,configParam,rxPin,txPin);
    _Serial = &serialPort;  
  }
  else{
    serialPort.begin(buadrate);
    _Serial = &serialPort;
  }

  Serial.print(F(">>Check module status "));
  check_module_ready();
  if (!hw_connected){
  }
  
  Serial.print(F(">>Setup "));

  _Serial->println("AT+CMEE=1");                    // set report error
  _serial_flush();

  Serial.println(F("...OK"));

  Serial.print(F(">>IMSI   : "));
  Serial.println(getIMSI());

  Serial.print(F(">>ICCID  : "));
  Serial.println(getICCID());

  Serial.print(F(">>IMEI   : "));
  Serial.println(getIMEI());

  if(debug)Serial.print(F(">>FW ver : "));
  if(debug)Serial.println(getFirmwareVersion());

  if(debug)Serial.print(F(">>PSM mode : "));
  if(debug)Serial.println(checkPSMmode());

  delay(1000);
  Serial.print(F(">>Signal : "));
  Serial.print(getSignal());
  Serial.println(F(" dBm"));

  delay(800);
  _serial_flush();
  Serial.print(F(">>Connecting "));

  if(attachNetwork(address, port)){  
    if(!createUDPSocket(address,port)){
      Serial.println(">> Cannot create socket");
    }
    Serial.println(F("OK"));
    Serial.println(F("---------- Connected ----------"));
  }
  else {
    Serial.println(F("FAILED"));
    Serial.println(F("-------- Disconnected ---------"));
  }
  
}

void AT_SIM7020E::check_module_ready(){
  int count = 0;
  
  _Serial->println(F("AT"));
  delay(100);
  while(1){
    if(_Serial->available()){
      data_input = _Serial->readStringUntil('\n');
      if(data_input.indexOf(F("OK"))!=-1){
        hw_connected=true;
        Serial.println(F("...OK"));
        break;
      }
    }
    else{
      unsigned int current_check=millis();
      if (current_check-previous_check>5000){
        if(count > 5) Serial.print(F("\nError to connect NB Module, rebooting..."));
        previous_check=current_check;
        hw_connected=false;
        Serial.print(F("."));
        reboot_module();
        _Serial->println(F("AT"));
        delay(100);
        count++;
       
      }
      else{
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
//    - blink first time  : start reboot 
//    - blink second time : finish reboot
void AT_SIM7020E::reboot_module(){
    digitalWrite(hwResetPin, LOW);
    delay(1000);
    digitalWrite(hwResetPin, HIGH);
    delay(2000);  
}

bool AT_SIM7020E::attachNetwork(String address,String port){
  bool status=false;
  if(!checkNetworkConnection()){
    for(int i=0;i<60;i++){
      setPhoneFunction();
      connectNetwork();
      delay(2000);
      if(checkNetworkConnection()){ 
        status=true;
        break;
      }
      Serial.print(F("."));
    }
  }
  else status=true;
    
  _serial_flush();
  _Serial->flush();
  return status;
}

// Check network connecting status : 1 connected, 0 not connected
bool AT_SIM7020E::checkNetworkConnection(){
  bool status=false;
  _serial_flush();
  _Serial->println(F("AT+CGATT?"));
  delay(800);
  for(int i=0;i<60;i++){
    if(_Serial->available()){
      data_input=_Serial->readStringUntil('\n');
      if(data_input.indexOf(F("+CGATT: 1"))!=-1){
        status=true;
      }
      else if(data_input.indexOf(F("+CGATT: 0"))!=-1){
        status=false;
      }
      else if(data_input.indexOf(F("OK"))!=-1) {
        break;        
      }
      else if(data_input.indexOf(F("ERROR"))!=-1) {
        break;        
      }
    }
  }
  data_input="";
  return status;
}
// Set Phone Functionality : 1 Full functionality
bool AT_SIM7020E::setPhoneFunction(){
  bool status=false;
  _Serial->println(F("AT+CFUN=1"));
   while(1){
    if(_Serial->available()){
      data_input=_Serial->readStringUntil('\n');
      if(data_input.indexOf(F("OK"))!=-1){
        status=true;
        break;
      }
      else if(data_input.indexOf(F("ERROR"))!=-1){
        status=false;
        break;
      }
    }
  }
  Serial.print(".");
  return status;
}

// Attach network : 1 connected, 0 disconnected
void AT_SIM7020E::connectNetwork(){  
  _Serial->println(F("AT+CGATT=1"));
  delay(1000);
  for(int i=0;i<30;i++){
    if(_Serial->available()){
      data_input =  _Serial->readStringUntil('\n');
      if(data_input.indexOf(F("OK"))!=-1) break;
      else if(data_input.indexOf(F("ERROR"))!=-1) break;
    }
  }
  Serial.print(".");
}

// Create a UDP socket and connect socket to remote address and port
bool AT_SIM7020E::createUDPSocket(String address,String port){
  bool status=false;
  _Serial->println(F("AT+CSOC=1,2,1"));
  delay(500);
  while(1){
    if(_Serial->available()){
      data_input=_Serial->readStringUntil('\n');
      if(data_input.indexOf(F("OK"))!=-1){
        //break;
      }
      else if(data_input.indexOf(F("+CSOC: 0"))!=-1){
        status=true;
        break;
      }
      else if(data_input.indexOf(F("+CSOC: 1"))!=-1){
        status=false;
        closeUDPSocket();
        _Serial->println(F("AT+CSOC=1,2,1"));
      }
    }
  }

  if(status){
    _Serial->print(F("AT+CSOCON=0,"));
    _Serial->print(port);
    _Serial->print(F(","));
    _Serial->println(address);
    while(1){
      if(_Serial->available()){
        data_input=_Serial->readStringUntil('\n');
        if(data_input.indexOf(F("OK"))!=-1){
          break;
        }
        else if(data_input.indexOf(F("ERROR"))!=-1) {
          status = false;
          break;
        }
      }
    }
    //if(debug) Serial.println("Create socket success");
  }
  delay(1000);
  return status;
}

// Close a UDP socket 0
bool AT_SIM7020E::closeUDPSocket(){
  _Serial->println(F("AT+CSOCL=0"));
  while(1){
    if(_Serial->available()){
      data_input=_Serial->readStringUntil('\n');
      if(data_input.indexOf(F("OK"))!=-1){
        break;
      }
    }
  }
}

// Set command echo mode off
void AT_SIM7020E::echoOff(){
  _Serial->println(F("ATE0"));
  while(1){
    if(_Serial->available()){
      data_input=_Serial->readStringUntil('\n');
      if(data_input.indexOf(F("OK"))!=-1) break;
    }
  }
}

// Ping IP
void AT_SIM7020E::pingIP(String IP){
  pingRESP pingr;
  String data = "";
  _Serial->println("AT+CIPPING="+IP);

  while(1){
    if(_Serial->available()){
      data_input=_Serial->readStringUntil('\n');
      if(data_input.indexOf(F("ERROR"))!=-1){
        break;
      }
      else if(data_input.indexOf(F("+CIPPING: 4"))!=-1){
        data=data_input;
        break;
      }
    }
  }

  if(data!=""){
    int index = data.indexOf(F(","));
    int index2 = data.indexOf(F(","),index+1);
    int index3 = data.indexOf(F(","),index2+1);
    pingr.status = true;
    pingr.addr = data.substring(index+1,index2);
    pingr.rtt = data.substring(index2+1,index3);
    pingr.ttl = data.substring(index3+1,data.length());
    //Serial.println("# Ping Success");
    Serial.println(">>Ping IP : "+pingr.addr + ", ttl= " + pingr.ttl + ", replyTime= " + pingr.rtt);

  }else { Serial.println(">>Ping Failed");}
  _serial_flush();
  data_input="";

}

// Set powerSavingMode : 0 turn off, 1 turn on
void AT_SIM7020E::powerSavingMode(unsigned int psm){
  _Serial->print(F("AT+CPSMS="));
  _Serial->println(psm);
  _serial_flush();
}

// Check if SIM/eSIM need PIN or not.
bool AT_SIM7020E::enterPIN(){
  bool status=false;
  _Serial->println(F("AT+CPIN?"));
  while(1){
    if(_Serial->available()){
      data_input = _Serial->readStringUntil('\n');
      if(data_input.indexOf(F("+CPIN:"))!=-1){
        if(data_input.indexOf(F("READY"))!=-1) {
          status=true;
          break;
        }
      }
      if(data_input.indexOf(F("OK"))!=-1) break;
    }
  }
  _serial_flush();
  return status;
}

/****************************************/
/**          Get Parameter Value       **/
/****************************************/
String AT_SIM7020E::getIMSI(){
  String imsi="";
  _Serial->println(F("AT+CIMI"));
  while(1){
    if(_Serial->available()){
      data_input=_Serial->readStringUntil('\n');
      if(data_input.indexOf(F("OK"))!=-1 && imsi.indexOf(F("52003"))!=-1) break;
      else if(data_input.indexOf(F("ERROR"))!=-1) _Serial->println(F("AT+CIMI"));
      else imsi+=data_input;
    }
  }
  imsi.replace(F("+CPIN: READY"),"");
  imsi.replace(F("OK"),"");  
  imsi.trim();
  //Serial.print(F(">>IMSI : "));
  //Serial.println(imsi); 
  return imsi;
}

String AT_SIM7020E::getICCID(){
  String iccid="";
  _Serial->println(F("AT+CCID"));
  while(1){
    if(_Serial->available()){
      data_input=_Serial->readStringUntil('\n');
      if(data_input.indexOf(F("OK"))!=-1) break;
      else iccid+=data_input;
    }
  }
  iccid.replace(F("OK"),"");
  iccid.trim();
  //Serial.print(F(">>ICCID : "));
  //Serial.println(iccid); 
  return iccid;
}

String AT_SIM7020E::getIMEI(){
  String imei;
  _Serial->println(F("AT+CGSN=1"));
  while(1){
    if(_Serial->available()){
      data_input=_Serial->readStringUntil('\n');
      if(data_input.indexOf(F("+CGSN:"))!=-1){
        data_input.replace(F("+CGSN: "),"");
        imei = data_input;
      }
      else if(data_input.indexOf(F("OK"))!=-1 && imei!="") break;
    }
  }
  //Serial.print(F(">>IMEI : "));
  //Serial.println(imei);
  return imei;
}

String AT_SIM7020E::getDeviceIP(){
  _serial_flush();
  String deviceIP;
  _Serial->println(F("AT+CGPADDR=1"));
  while(1){
    if(_Serial->available()){
      data_input=_Serial->readStringUntil('\n');
      if(data_input.indexOf(F("+CGPADDR"))!=-1){
        int index = data_input.indexOf(F(":"));
        int index2 = data_input.indexOf(F(","));
        deviceIP = data_input.substring(index2+1,data_input.length());
      }
      else if(data_input.indexOf(F("OK"))!=-1) break;
    }
  }
  deviceIP.replace(F("\""),"");
  deviceIP.trim();
  // Serial.print(F(">>Device IP : "));
  // Serial.println(deviceIP);
  return deviceIP;
}

String AT_SIM7020E::getSignal(){
  _serial_flush();
  int rssi = 0;
  int count = 0;
  String data_csq = "";
  data_input = "";
  do
  {
    delay(200);
    _Serial->println(F("AT+CSQ"));
    delay(500);
    while(1)  {    
      if(_Serial->available()){
        data_input = _Serial->readStringUntil('\n');
        if(data_input.indexOf(F("OK"))!=-1){
         break;
        }
        else{
          if(data_input.indexOf(F("+CSQ"))!=-1){
            int start_index = data_input.indexOf(F(":"));
            int stop_index  = data_input.indexOf(F(","));
            data_csq = data_input.substring(start_index+1,stop_index);
            if (data_csq == "99"){
              data_csq = "N/A";
            }
            else{
              rssi = data_csq.toInt();
              rssi = (2*rssi)-113;
              data_csq = String(rssi);
            }
          }
        }
      }
    }
  if(rssi==-113)count++;

  }while(rssi==-113&&count<=10);
  if(rssi==-113)
  {
    data_csq = "-113";
    count= 0;
  }
  return data_csq;
}

String AT_SIM7020E:: getAPN(){
  String out="";
  _Serial->println(F("AT+CGDCONT?"));

  while(1){
    if(_Serial->available()){
      data_input = _Serial->readStringUntil('\n');
      if(data_input.indexOf(F("+CGDCONT: 1"))!=-1){
        int index=0;
        int index2=0;
        index = data_input.indexOf(F(":"));
        index2 = data_input.indexOf(F(","));

        index = data_input.indexOf(F(","),index2+1);
        index2 = data_input.indexOf(F(","),index+1);
        out = data_input.substring(index+2,index2-1);
      }
      if(data_input.indexOf(F("OK"))!=-1){
        break;
      }
    }
  }
  _serial_flush();
  data_input="";
  return out;
}

String AT_SIM7020E::getFirmwareVersion(){
  String fw="";
  _Serial->println(F("AT+CGMR"));
  while(1){
    if(_Serial->available()){
      data_input=_Serial->readStringUntil('\n');
      if(data_input.indexOf(F("OK"))!=-1) break;
      else fw+=data_input;
    }
  }
  fw.replace(F("OK"),"");
  fw.trim();
  //Serial.print(F(">>FW Ver: "));
  //Serial.println(fw); 
  return fw;
}

String AT_SIM7020E::getNetworkStatus(){
  String out = "";
  String data = "";
  int count=0;

  _Serial->println(F("AT+CEREG=2"));
  delay(500);
  _serial_flush();
  delay(1000);
  _Serial->println(F("AT+CEREG?"));
  while(1){
    if(_Serial->available()){
      data_input = _Serial->readStringUntil('\n');
      if(data_input.indexOf(F("+CEREG"))!=-1){
        count++;
        if(count<10 && data_input.indexOf(F(",2"))!=-1){
          _serial_flush();
          _Serial->println(F("AT+CEREG?"));
        }
        else {
          data=data_input;
          int index = data.indexOf(F(": "));
          int index2 = data.indexOf(F(","));
          int index3 = data.indexOf(F(","),index2+1);
          out = data.substring(index2+1,index2+2);
          if (out == F("1")) out = F("Registered");
          else if (out == "0") out = F("Not Registered");
          else if (out == "2") out = F("Trying");
        }
      }
      else if(data_input.indexOf(F("OK"))!=-1) break;

    }
  }
  return(out);
}

bool AT_SIM7020E::checkPSMmode(){
  bool status = false;
  _Serial->println(F("AT+CPSMS?"));
  while(1){
    if(_Serial->available()){
      data_input=_Serial->readStringUntil('\n');
      if(data_input.indexOf(F("+CPSMS: "))!=-1){
        if(data_input.indexOf(F("1"))!=-1) status = true;
        else status = false;
      }
      if(data_input.indexOf(F("OK"))!=-1){
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
void AT_SIM7020E::_Serial_print(String address,String port,unsigned int len){
  if(debug) Serial.println("Send to "+address+","+port);
  _Serial->print(F("AT+CSOSEND=0,"));
  _Serial->print(len);
  _Serial->print(F(","));
}

// Send message type String
void AT_SIM7020E::_Serial_print(String msg){
  _Serial->print(msg);
}

// Send message type unsigned int
void AT_SIM7020E::_Serial_print(unsigned int msg){
  _Serial->print(msg);
  //if(debug)Serial.print(msg);
}

// Send message type char *
void AT_SIM7020E::_Serial_print(char *msg){
  _Serial->print(msg);
  //if(debug)Serial.print(msg);
}

// Send '\r\n'
void AT_SIM7020E::_Serial_println(){
  _Serial->println();
  //if(debug)Serial.println();
}

/****************************************/
/**        Receive UDP Message         **/
/****************************************/
// Receive incoming message : +CSONMI: <socket_id>,<data_len>,<data>
void AT_SIM7020E:: waitResponse(String &retdata,String server){ 

  if(_Serial->available()){
    char data=(char)_Serial->read();
    if(data=='\n' || data=='\r'){
      if(k>1){
        end=true;
        k=0;
      }
      k++;
    }
    else{
      data_input+=data;
      //Serial.println(data_input);
    }
  }
  if (end){
    manageResponse(retdata,server);   
  }
  
}

// Split data from incoming message
void AT_SIM7020E:: manageResponse(String &retdata,String server){ 
  if(end){  
    end=false;

    if(data_input.indexOf(F("+CSONMI:"))!=-1){
      String left_buffer="";

      //pack data to char array
      char buf[data_input.length()+1];
      memset(buf,'\0',data_input.length());
      data_input.toCharArray(buf, sizeof(buf));

      char *p = buf;
      char *str;
      int i=0;
      int j=0;
      while ((str = strtok_r(p, ",", &p)) != NULL){   // delimiter is the comma
        j=2;  // number of comma

        if(i==j){
          retdata=str;
        }
        if(i==j+1){
          left_buffer=str;
        }
        i++;
      }              
        data_input=F("");
      }       
    
  }

}

/****************************************/
/**          Utility                   **/
/****************************************/
// print char * to hex
void AT_SIM7020E::printHEX(char *str){
  char *hstr;
  hstr=str;
  char out[3];
  memset(out,'\0',2);
  int i=0;
  bool flag=false;
  while(*hstr){
    flag=itoa((int)*hstr,out,16);
    
    if(flag){
      _Serial_print(out);

      if(debug){
        Serial.print(out);
      }      
    }
    hstr++;
  }
}

// Flush unwanted message from serial
void AT_SIM7020E::_serial_flush(){
  while(1){
    if(_Serial->available()){
      data_input=_Serial->readStringUntil('\n');
    }
    else{
      data_input="";
      break;
    }
  }
  _Serial->flush();
}



