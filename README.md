![Library Version](https://img.shields.io/badge/Version-1.4.0-green)

# AIS Library

## AIS SIM7020E API
AIS SIM7020E API Library, used on arduino, have been developed for 
any platform. This library include API such as UDP, MQTT.
### Example for use the AIS SIM7020E API
#### Call the AIS SIM7020E API library:
```cpp
#include "AIS_SIM7020E_API.h"
AIS_SIM7020E_API nb;
```
#### Initial AIS SIM7020E API Library:
**for UDP**
```cpp
nb.begin(serverIP,serverPort);    
```
**for MQTT**
```cpp
nb.begin();    
nb.connectMQTT(serverIP,serverPort,clientID)
nb.RegisMQCallback(callback);     
```
#### Send Data:
**for UDP**
```cpp
nb.sendMsgSTR(serverIP,serverPort,payload);  // Send data in String 
// or
nb.sendMsgHEX(serverIP,serverPort,payload);  // Send data in HexString   
```
**for MQTT**
```cpp
nb.publish(topic,payload)  
``` 
 **Note** please see more in the example code  
 
## AIS Magellan Library
AIS Magellan Library, a software development kit used on arduino platform, have been developed for 
Magellan IoT Platform.  

### Example for use the Magellan SDK
#### Call the Magellan library:
```cpp
#include "Magellan_SIM7020E.h"
Magellan_SIM7020E magel;
```
#### Initial Magellan Library:
```cpp
magel.begin();           //init Magellan LIB
```
#### Payload Data: 
Please use the payload in JSON format 

**Example**\
{"Temperature":25,"Humidity":90}

```cpp
payload="{\"Temperature\":"+Temperature+",\"Humidity\":"+Humidity+"}";
```
#### Report Data:
The example code report payload data to Magellan IoT Platform.
```cpp
magel.report(payload);
```
#### Example Magellan payload format
Please the location payload data as below format.\
**Example**
```cpp
payload={"Location":"Latitude,Longitude"}
```
Show battery on dashboard\
Battery is range in 0-100 %.\
**Example**
```cpp
payload={"Battery":100}   
```
Show Lamp status on dashbord\
please use 0 or 1 to send status\
**Example**
```cpp
payload={"Lamp":0} 
payload={"Lamp":1}
```
**Note** please see more in the example code 
## Quick Started
  1. Connect `DEVIO NB-DEVKIT I` to your computer.
  2. Open the Magellan IoT platform and see the data on your account.

**Note** In this case, the device has already preload code then you just plug and play the development kit. You don't need to upload the example code to the device.