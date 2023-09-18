# TFG_IoT  

## Basics
  
This repository contains all the software needed to build an embedded system that implements a low-cost weather station capable of measuring temperature in degrees Celsius, relative humidity in percent, atmospheric pressure in Pascals, altitude above sea level in meters and air quality according to the IAQ (Indoor Air Quality) index metric. In addition, the weather station will be able to send the information from these measurements to an MQTT server, so that they can be accessed remotely.

The scenario of this project is composed of two main elements: 
 - **An Ubuntu virtual machine**. It will host the MQTT broker with which the weather station will communicate, as well as the tools to develop all the software for the weather station, including the Linux distribution used in the embedded system and the executable code that will develop the functions of the station. In the case of the MQTT broker, the ThingsBoard software has been used, explained in the "**ThingsBoard**" folder. For the Linux distribution of the embedded system, the two most widely adopted softwares in the industry have been used: Buildroot and Yocto. These tools are self-excluding, which means that they are independent methods for developing an embedded system. The reason for building an embedded system for each tool was to test the result with both methods. The instructions to build the system with these two tools are in the "**Yocto_Project**" and "**Buildroot**" folders. Finally, in the case of software development, the Eclipse CDT software has been used. The created project is located in the folder "**Project**".

 - **The embedded system**. It is the one that implements the functions of the weather station itself. It is composed of the [Raspberry Pi 3B+](https://www.raspberrypi.com/news/raspberry-pi-3-model-bplus-sale-now-35/) and a series of sensors and actuators. More specifically, it has the following sensors and actuators:
   - **BME688 sensor**. This I2C sensor integrates in the same package, a temperature sensor, a relative humidity sensor, an atmospheric pressure sensor and a gas sensor. With these measures will be obtained each and every one of the measurements of the weather station.
   - **TFT-LCD screen**. It is a circular SPI screen with a diameter of 240 pixels and with the ability to set different colors for each pixel using the 565 format, in which RGB (Red Green Blue) colors are encapsulated in two bytes (5 bits for red, 6 for green and 5 for blue). This screen will be used to visualize the system status at all times.
   - **Micro Joystick**. This is a small joystick with five buttons to represent the five possible gestures: "up", "down", "right", "left" and "center". With this joystick the user can move through the different menus of the system.
  
It is worth noting that both the BME688 sensor and the LCD display are located inside the [Arduino MKR IoT Carrier v2](https://docs.arduino.cc/hardware/mkr-iot-carrier-rev2) board, which groups a large number of sensors and actuators for IoT applications. The LCD display is placed in the central area while the BME688 sensor is placed near the edge of the board, as shown in the figure below.
<p align="center">
<img width="250" align="center" src="https://github.com/pablo-sanmillanf/TFG_IoT/assets/101637076/a2dfb8ff-76b3-4a12-bace-0e6ad03b8fdc">
<br/>
<i>Arduino MKR IoT Carrier v2 board</i>
</p>
    
<br/> 
The connections between these three elements are shown in the following table.

| Raspberry Pi 3 B+ | Arduino MKR IoT Carrier v2 | Joystick |
| :---------------: | :------------------------: | :------: |
|      GPIO 25      |             -	             |    UP    |
|      GPIO 24      |             -              |   LEFT   |
|      GPIO 23      |             -              |  CENTER  |
|      GPIO 18      |             -              |   RIGHT  |
|      GPIO 7       |             -              |   DOWN   |
|        GND        |            GND             |  COMMON  |
|      GPIO 4       |           RESET            |     -    |
|      GPIO 17      |             14             |     -    |
|      GPIO 8       |             13             |     -    |
|      GPIO 3       |             12             |     -    |
|      GPIO 2       |             11             |     -    |
|      GPIO 9       |             10             |     -    |
|      GPIO 11      |             9              |     -    |
|      GPIO 10      |             8              |     -    |
|     3V3 Power     |             3              |     -    |
  
<br/>
As you can see, in order to have the complete system, a large number of connections are necessary, which makes it quite difficult to manage. To solve this problem, it has been decided to include in the project a printed circuit board (PCB) with which to easily connect the Raspberry Pi and the Arduino MKR IoT Carrier v2. In the case of the joystick, it has been decided to solder it directly to the board.  

As a graphical summary, the following figure shows a diagram of the entire project scenario, indicating in yellow the logical and physical connections that must be made for the embedded system to work as it is proposed, in red the connections that can be made to access the system through a command interface, and in blue a couple of clarifications on the PCB that refer to the location of the joystick used in the application and the location of the joystick used in the application.
In red the connections that can be made to access the system through a command interface, and in blue a couple of clarifications on the PCB that refer to the location of the joystick used in the application and the input for the serial connections. 

<p align="center">
<img width="750" align="center" src="https://github.com/pablo-sanmillanf/TFG_IoT/assets/101637076/442b0825-1f40-4bb6-8689-f129085d2e63">
<br/>
<i>Project scenario</i>
</p>
  
</br>

## Description of the application

The application has been structured in a series of windows and menus that will be displayed on the LCD screen and through which the user will be able to move using the joystick. Two groups of these windows can be distinguished: the windows for displaying system measurements and data, and the windows for configuring the system itself.
The first group is composed of six windows with which the user can display the system time, temperature, relative humidity, atmospheric pressure, altitude above sea level and the IAQ index. Both the temperature, humidity and IAQ windows change the color of the central part of the screen and the filling of the icon bar according to the value of the associated magnitude.  
In order to move between these screens, the user can use the "right" or "left" gestures of the joystick to move to the next or previous window, respectively. This behavior is cyclic, so that the user can move from the last window to the first and vice versa. In addition, the system has been designed in such a way that, after an inactivity period of five seconds, the application automatically switches to the next window.

<p align="center">
<img width="750" align="center" src="https://github.com/pablo-sanmillanf/TFG_IoT/assets/101637076/8525b6c5-b298-48db-9120-44ce81457d55">
<br/>
<i>Main windows</i>
</p>
Also, it can be seen how in the upper part of all the windows there is an indicator of the window in which the user is, helping him in this way in the location of the rest of the windows. On the other hand, there is also an indicator at the bottom of all the windows that indicates that after pressing the "down" gesture of the joystick, the windows and configuration menus can be accessed.  
On the other hand, in the case of the group of menus and system configuration windows to be accessed with the joystick "down" gesture, it is formed by a main window, in which all the possible system configurations are arranged in a vertical list, and a series of windows associated with each of these configurations. To move through this vertical list, the "up" and "down" gestures of the joystick are used. Once the cursor has been placed on the desired menu, the "right" gesture can be used to access the windows associated with that configuration. On the other hand, if you want to return to the other group of windows detailed above, with the "left" gesture from any of the menus, you will return to the system time window.

<p align="center">
<img width="400" align="center" src="https://github.com/pablo-sanmillanf/TFG_IoT/assets/101637076/1cfd66e8-d7aa-444d-9306-ef0f3eb17e9f">
<br/>
<i>Conf window</i>
</p>

Focusing on the specific configurations that can be made with this group of windows, there are seven possible configurations:
 - "**Carrusel**". With this configuration, you can decide whether or not, after five seconds of inactivity in the main windows, you want to move automatically to the next window.
 - "**IP Broker**". Thanks to this configuration you can choose the IP address of the MQTT broker where the measurements obtained by the sensors will be published. This broker must be connected to the same Wifi network as the system, so if this is not the case or if the weather station is not connected to the Internet, this utility will not work. It should be noted that the system has no way to visually indicate to the user whether or not it is connected to the internet and whether or not it is connected to the broker.
 - "**Token MQTT**". This setting will allow you to choose the token to be used for system authentication to the MQTT broker. This token must have been previously provided by the broker to enable MQTT communications.
 - "**Temp Offset**". With this setting it is possible to set an offset in the temperature measurements. The idea of this offset is to be able to compensate for temperature disturbances caused by self-heating of the system, so that the final result is closer to reality. Since these disturbances are always additive, the chosen value will always be subtracted from the temperature obtained from the sensor. This offset may have a precision of up to two decimal places.
 - "**Hora local**". Within this configuration, the user will be able to choose the time zone of the system.
 - "**Nuevo WiFi**". With this configuration, new WiFi networks can be added to the list of networks recognized by the system, choosing their identifier from those reachable by the system and then entering their password.
 - "**Apagar**".This will shut down the system completely.
