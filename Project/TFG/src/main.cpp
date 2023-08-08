//THIS MAIN FILE IS USED TO DEBUG THE PROJECT MODULES (LAST DEFINE) AND TO STORE SOME EXAMPLES.


#define __BUTTON_CODE__

#ifndef __BUTTON_CODE__

/***
 * EXAMPLE OF MULTITHREAD WITH BUTTONS CLASS
 */

#include <thread>
#include "buttons/buttons.h"
#include <iostream>
#include <unistd.h>


void printer_thread(){
  Buttons::BUTTON data;
  std::string type;
  for(;;){
    data = Buttons::buttonsQueue.pop();
    if(data.type == Buttons::LONG_PULSE)
      type = "LONG";
    else
      type = "SHORT";

    switch(data.symbol){
      case Buttons::UP:
        std::cout << "UP " << type << " Pulse" << std::endl;
        break;
      case Buttons::DOWN:
        std::cout << "DOWN " << type << " Pulse" << std::endl;
        break;
      case Buttons::LEFT:
        std::cout << "LEFT " << type << " Pulse" << std::endl;
        break;
      case Buttons::RIGHT:
        std::cout << "RIGHT " << type << " Pulse" << std::endl;
        break;
      case Buttons::CENTER:
        std::cout << "CENTER " << type << " Pulse" << std::endl;
        break;
    }
  }
}

int main(){
  std::thread first (Buttons::buttons_thread, 1, 2000, 25, 12, 24, 18, 23);

  usleep(100000);
  std::thread third (printer_thread);  // spawn new thread that calls printer_thread()


  std::cout << "Main wait\n" << std::endl;
  // synchronize threads:
  first.join();                // pauses until first finishes
  third.join();               // pauses until third finishes
  return 0;
}
#endif

#define __BME_LSM_PRINTER_CODE__

#ifndef __BME_LSM_PRINTER_CODE__

#include "measures/BME688/BME688.h"
#include "measures/IAQTracker/IAQTracker.h"
#include "measures/LSM6DSOX/LSM6DSOX.h"
#include <unistd.h>
#include <atomic>
#include <thread>
#include <cmath>
#include <iostream>



struct BME_data {
    std::atomic<float> temperature;
    std::atomic<float> pressure;
    std::atomic<float> humidity;
    std::atomic<float> iaq;
    std::atomic<float> altitude;
    // Constructor so that I can init the values I want.
    BME_data(float temp, float press, float hum, float iaq, float h) :
      temperature(temp), pressure(press), humidity(hum), iaq(iaq), altitude(h){}
} bme_data(-1, -1, -1, -1, -1);


struct axis_data{
    std::atomic<float> x;
    std::atomic<float> y;
    std::atomic<float> z;
};

struct LSM_data {
    axis_data acc;
    axis_data gyr;
    std::atomic<float> temp;
    // Constructor so that I can init the values I want.
    LSM_data(float temp) :
      temp(temp){}
} lsm_data(-1);


void acc_sensor_thread();
void gas_sensor_thread();
void printer_thread();
float obtain_sea_level_altitude(float pressure);


int main(){
  std::thread first (acc_sensor_thread);     // spawn new thread that calls acc_sensor_thread()
  std::thread second (gas_sensor_thread);     // spawn new thread that calls gas_sensor_thread()

  usleep(100000);
  std::thread third (printer_thread);  // spawn new thread that calls printer_thread()


  printf("Main wait\n");
  // synchronize threads:
  first.join();                // pauses until first finishes
  second.join();               // pauses until second finishes
  third.join();               // pauses until third finishes
  return 0;
}

void acc_sensor_thread(){
  LSM6DSOX sensor(LSM6DSOX_52_HZ_ODR, LSM6DSOX_52_HZ_ODR);

  float temp, x_acc, y_acc, z_acc, x_gyr, y_gyr, z_gyr;

  for(;;){

    if(sensor.get_temperature(&temp) != -1)
      lsm_data.temp = temp;

    if(sensor.get_acc_values(&x_acc, &y_acc, &z_acc) != -1){
      lsm_data.acc.x = x_acc;
      lsm_data.acc.y = y_acc;
      lsm_data.acc.z = z_acc;
    }

    if(sensor.get_gyr_values(&x_gyr, &y_gyr, &z_gyr) != -1){
      lsm_data.gyr.x = x_gyr;
      lsm_data.gyr.y = y_gyr;
      lsm_data.gyr.z = z_gyr;
    }

    usleep(100100);
  }
}


void gas_sensor_thread(){
  BME688 sensor(25, 2.00);
  IAQTracker tracker;
  float temperature, pressure, humidity, gas_resistance, iaq;

  printf("Init sensor status: %d\n", sensor.init());
  printf("Set oversamplings status: %d\n", sensor.set_oversamplings(OVSP_1_X, OVSP_4_X, OVSP_16_X));
  printf("Set heater configurations status: %d\n", sensor.set_heater_configurations(true, 300.0, 100));
  for(;;){
    sensor.get_data_one_measure(&temperature, &pressure, &humidity, &gas_resistance);
    bme_data.temperature = temperature;
    bme_data.pressure = pressure;
    bme_data.humidity = humidity;
    bme_data.altitude = obtain_sea_level_altitude(pressure);

    if(tracker.get_IAQ(&iaq, temperature, humidity, gas_resistance) != -1)
      bme_data.iaq = iaq;

    usleep(100000);
  }
}

void printer_thread(){
  unsigned long long cnt = 0;
  for(;;){
    printf("\nPrinter iteration %llu:\n", ++cnt);
    printf("\tBME688:\t\tTemp: %.2f, Press: %.2f, Hum: %.2f, IAQ: %.2f Altitude: %.2f\n",
        (float)bme_data.temperature,
        (float)bme_data.pressure,
        (float)bme_data.humidity,
        (float)bme_data.iaq,
        (float)bme_data.altitude);

    printf("\tLSM6DSOX:\tTemp: %.2f, Acc-X: %.2f, Acc-Y: %.2f, Acc-Z: %.2f Gyr-X: %.2f, Gyr-Y: %.2f, Gyr-Z: %.2f\n",
            (float)lsm_data.temp,
            (float)lsm_data.acc.x,
            (float)lsm_data.acc.y,
            (float)lsm_data.acc.z,
            (float)lsm_data.gyr.x,
            (float)lsm_data.gyr.y,
            (float)lsm_data.gyr.z);

    usleep(1000000);
  }
}



/**
  * @brief     Converts a pressure measurement into a height in meters.
  * @param[in] pressure The pressure in Pascals.
  * @return    floating point altitude in meters.
  */
float obtain_sea_level_altitude(float pressure){
  return 44330.76923 * (1 - std::pow(pressure / 101325.00, 0.190266));
}


#endif



#define __DEBUG_CODE__

#ifndef __DEBUG_CODE__

#include <unistd.h>
#include <atomic>
#include <thread>
#include <cmath>
#include <iostream>
#include <iomanip>
#include <ctime>
#include "measures/measures.h"
#include "client_MQTT/client_MQTT.h"
#include "configurations/configurations.h"
#include "buttons/buttons.h"



void printer_thread();
std::string get_time();


int main(){
  /*
  Measures::Meas meas;
  meas.start_measures();

  usleep(100000);
  std::thread third (printer_thread);  // spawn new thread that calls printer_thread()


  printf("Main wait\n");
  // synchronize threads:
  third.join();               // pauses until third finishes
  */

  //std::thread first (Buttons::buttons_thread, 1, 1000, 25, 12, 24, 18, 23);
  Measures::Meas meas;
  meas.start_measures();
  std::string host = "192.168.1.78";
  std::string token = "Vxkhwan4knR2Q889ELpk";
  std::string telemetry_topic = "v1/devices/me/telemetry";
  std::string attributes_topic = "v1/devices/me/attributes";

  auto fn = [&]() {
    std::cout << "ON_OFF changed: \n\tTemp: " << MQTT::temp_on
                                << "\tPress: " << MQTT::press_on
                                << "\tHum: " << MQTT::hum_on
                                << "\tIAQ: " << MQTT::IAQ_on
                                << "\tAlt: " << MQTT::alt_on << std::endl;
  };


  std::thread mqtt(MQTT::client_mqtt_thread, host, token, telemetry_topic, attributes_topic, fn);

  //Configurations::handle_server_mqtt_token(&token);
  //std::cout << "Final token-> " << token << " LEN: " << token.size() << std::endl;
  mqtt.join();
  return 0;
}



void printer_thread(){

  for(;;){
    //std::cout << get_time() << "-> " << bme_data.temperature << std::endl;

    printf("\tBME688:\t\tTemp: %.2f, Press: %.2f, Hum: %.2f, IAQ: %.2f Altitude: %.2f\n",
            (float)Measures::bme_data.temperature,
            (float)Measures::bme_data.pressure,
            (float)Measures::bme_data.humidity,
            (float)Measures::bme_data.iaq,
            (float)Measures::bme_data.altitude);

    usleep(500000);
  }
}


/**
  * @brief     Obtain the current time as a string with the format "H:M:S".
  * @return    The current time as a string.
  */
std::string get_time(){
  auto t = std::time(nullptr);
  auto tm = *std::localtime(&t);
  std::stringstream ss;
  ss << std::put_time(&tm, "%H:%M:%S");
  return ss.str();
}

#endif


//#define __TFT_CODE__

#ifndef __TFT_CODE__

#include <iostream>
#include <stdio.h>
#include <stdint.h>
#include "TFTDisplay/TFTDisplay.h"
#include <chrono>
#include <thread>

int main() {

  Display_driver::init_display();


  Display_driver::write_panning_fast_string(30, 100, 180, 7,"Hola que tal estas, espero que bien!!",
      TFTDisplay::main_color,
      TFTDisplay::background_color,
      FreeSansBold12pt7b);
  using namespace std::chrono_literals; // ns, us, ms, s, h, etc.
  std::this_thread::sleep_for(1s);

  Display_driver::write_fast_string_truncated(30, 100, 180,"Hola que tal estas, espero que bien!!",
      TFTDisplay::main_color,
          TFTDisplay::background_color,
          FreeSansBold12pt7b);


  Display_driver::uninit();
}

#endif

