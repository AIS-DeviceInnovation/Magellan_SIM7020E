![Library Version](https://img.shields.io/badge/Version-1.5.1-green)

# AIS DEVIO NB-DEVKIT I Library

This library only supports the [DEVIO NB-DEVKIT I](https://aisplayground.ais.co.th/marketplace/products/WqTKYdlwnhl). If you have any questions or concerns, please reach out to us on [DEVIO FB Fanpage](https://www.facebook.com/AISDEVIO).

## AIS SIM7020E API

The AIS SIM7020E API Library is an Arduino IDE library that has been developed to support platforms that use UDP, MQTT, and MQTTs.

### AIS SIM7020E API Example Code

#### Calling the AIS SIM7020E API Library:
```cpp
#include "AIS_SIM7020E_API.h"
AIS_SIM7020E_API nb;
```

### Initializing the AIS SIM7020E API Library:

**UDP**
```cpp
nb.begin(serverIP,serverPort);
```

**MQTT**
```cpp
nb.begin();
setupMQTT();
//nb.connectMQTT(serverIP,serverPort,clientID);
nb.setCallback(callback);
```
#### Sending Data:

**UDP**
```cpp
nb.sendMsgSTR(serverIP,serverPort,payload);  // Send data in String 
// or
nb.sendMsgHEX(serverIP,serverPort,payload);  // Send data in HexString   
```

**MQTT**
```cpp
nb.publish(topic,payload);  
```
## AIS Magellan Library

The AIS Magellan Library is an SDK for use with the Magellan IoT Platform and the Arduino IDE.

### Magellan SDK Example Code
#### Calling the Magellan Library:
```cpp
#include "Magellan_SIM7020E.h"
Magellan_SIM7020E magel;
```
#### Initializing the Magellan Library:
```cpp
magel.begin();           //init Magellan LIB
```
#### Payload Data:
Please ensure that the payload is in JSON format, for example:
```cpp
payload="{\"Temperature\":"+Temperature+",\"Humidity\":"+Humidity+"}";
```

#### Reporting Data:

Please ensure that the payload is in JSON format, for example:
```cpp
magel.report(payload);
```
#### Magellan Payload Format Examples

**Location**
```cpp
payload="{\"Location\":"Latitude,Longitude"}";
```
**Battery Status on Dashboard**\
Battery must be in the range of 0-100 %
```cpp
payload="{\"Battery\":100}"; 
```
**Lamp Status**\
Send status using 0 or 1.
```cpp
payload="{\"Lamp\":0}";
payload="{\"Lamp\":1}";
```
**Note** For more examples, please refer to the example code included in the Arduino IDE.