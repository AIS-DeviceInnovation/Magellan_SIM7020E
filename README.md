# AIS Magellan Library
AIS Magellan Library, a software development kit used on arduino platform, have been developed for 
Magellan IoT Platform.  

# Example for use the Magellan SDK
### Call the Magellan library:
```cpp
#include "Magellan_SIM7020E.h"
Magellan_SIM7020E magel;
```
### Initial Magellan Library:
```cpp
magel.begin();           //init Magellan LIB
```
### Payload Data: 
Please use the payload in JSON format 

**Example**\
{"Temperature":25,"Humidity":90}

```cpp
payload="{\"Temperature\":"+Temperature+",\"Humidity\":"+Humidity+"}";
```
### Report Data:
The example code report payload data to Magellan IoT Platform.
```cpp
magel.report(payload);
```
### Example Magellan payload format
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
# Quick Started
  1. Connect `DEVIO NB-DEVKIT I` to your computer.
  2. Open the Magellan IoT platform and see the data on your account.

**Note** In this case, the device has already preload code then you just plug and play the development kit. You don't need to upload the example code to the device.