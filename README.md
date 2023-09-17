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
