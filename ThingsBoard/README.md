# ThingsBoard configurations

## Basics
  
This directory contains the instructions to configure an MQTT Broker in ThingsBoard. It also has a configuration file to speed up the process of creating the dashboard used in the broker.

## Build

First, the ThingsBoard software must be downloaded and installed as detailed on its [website](https://thingsboard.io/docs/user-guide/install/ubuntu/), with the default configurations.  
Then, the ThingsBoard registration window can be accessed at the address "**http://localhost:8080**", where the administrator's credentials must be entered: "**tenant@thingsboard.org**" as user, and "**tenant**" as password. Then, access the "**Dashboards**" and "**Device profiles**" tabs where the "**ws_dashboard.json**" and "**weather_station_profile.json**" files must be imported respectively, using the "**+**" icon at the top right of each window.

<p align="center">
<img width="750" align="center" src="https://github.com/pablo-sanmillanf/TFG_IoT/assets/101637076/b8877125-6b2d-40b9-a74f-64dce8a12166">
<br/>
<i>Dashboards Tab</i>
</p>

Then, a new device must be created in the "**Devices**" tab, which must use the device profile imported earlier. Within this new device, in the "**Details**" window you can copy the access token required to send information to the MQTT broker. On the other hand, in the "**Attributes**" window, the attributes sent to the weather station can be modified. These attributes must be of the "**Shared Attributes**" category. These attributes must be of the "**Shared Attributes**" category, and, although any attribute can be sent, the station will only understand the following attributes:
- "**send_temperature**". Boolean. Indicates if the user wants to receive temperature measurements. Its default value is yes.
- "**send_pressure**". Boolean. Indicates if the user wants to receive pressure measurements. Its default value is yes.
- "**send_humidity**". Boolean. Indicates if the user wants to receive humidity measurements. Its default value is yes.
- "**send_IAQ**". Boolean. Indicates if the user wants to receive IAQ measurements. Its default value is yes.
- "**send_altitude**". Boolean. Indicates if the user wants to receive altitude measurements. Its default value is yes.
- "**send**". Boolean. Indicates whether the user wants to receive measurements or not. Its default value is yes.
- "**send_rate**". Integer. Indicates how many seconds the user wants the embedded system to send data. Considering that the embedded system stores a measurement every second, this value also indicates how many measurements will come in each message. Its default value is 10 and its maximum value is 100.

Finally, to have the dashboard with the data in the main "**Home**" tab, click on the "**⋮**" icon in the upper right corner of the screen, next to the user name, access the "**Profile**" menu and modify the "**Home dashboard**" field to use the previously imported dashboard: the "**WS Dashboard**" dashboard.
