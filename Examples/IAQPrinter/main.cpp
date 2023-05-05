/***
 * EXAMPLE OF MULTITHREAD WITH THE IAQ TRACKER CLASS
 */





#include "BME688.h"
#include "IAQTracker.h"
#include <unistd.h>
#include <atomic>
#include <thread>
#include <cmath>


struct BME_data {
    std::atomic<float> temperature;
    std::atomic<float> pressure;
    std::atomic<float> humidity;
    std::atomic<float> iaq;
    std::atomic<float> altitude;
    // Constructor so that I can init the values I want.
    BME_data(float temp, float press, float hum, float iaq, float h) :
      temperature(temp), pressure(press), humidity(hum), iaq(iaq), altitude(h){}
} data(-1, -1, -1, -1, -1);


void gas_sensor_thread();
void printer_thread();
float obtain_sea_level_altitude(float pressure);


int main(){
  std::thread first (gas_sensor_thread);     // spawn new thread that calls gas_sensor_thread()

  usleep(100000);
  std::thread second (printer_thread);  // spawn new thread that calls printer_thread()


  printf("Main wait\n");
  // synchronize threads:
  first.join();                // pauses until first finishes
  second.join();               // pauses until second finishes
  return 0;
}




void gas_sensor_thread(){
  BME688 sensor(25);
  IAQTracker tracker;
  float temperature, pressure, humidity, gas_resistance, iaq;

  printf("Init sensor status: %d\n", sensor.init());
  printf("Set oversamplings status: %d\n", sensor.set_oversamplings(OVSP_1_X, OVSP_4_X, OVSP_16_X));
  printf("Set heater configurations status: %d\n", sensor.set_heater_configurations(true, 300.0, 100));
  for(;;){
    sensor.get_data_one_measure(&temperature, &pressure, &humidity, &gas_resistance);
    data.temperature = temperature;
    data.pressure = pressure;
    data.humidity = humidity;
    data.altitude = obtain_sea_level_altitude(pressure);

    if(tracker.get_IAQ(&iaq, temperature, humidity, gas_resistance) != -1)
      data.iaq = iaq;

    usleep(100000);
  }
}

void printer_thread(){
  unsigned long long cnt = 0;
  for(;;){
    printf("Printer iteration %llu:\tTemp: %.2f, Press: %.2f, Hum: %.2f, IAQ: %.2f Altitude: %.2f\n", ++cnt,
        (float)data.temperature, (float)data.pressure, (float)data.humidity, (float)data.iaq, (float)data.altitude);

    usleep(1000000);
  }
}



/**
  * @brief     Converts a pressure measurement into a height in meters
  * @param[in] pressure The pressure in Pascals.
  * @return    floating point altitude in meters.
  */
float obtain_sea_level_altitude(float pressure){
  return 44330.76923 * (1 - pow(pressure / 101325.00, 0.190266));
}

