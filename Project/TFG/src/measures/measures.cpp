/**
  ******************************************************************************
  * @file   measures.cpp
  * @author Pablo San Millán Fierro (pablo.sanmillanf@alumnos.upm.es)
  * @brief  Measures gatherer tread handler Module.
  *
  * @note   End-of-degree work.
  *         This module gather all the measures from the sensors.
  ******************************************************************************
*/

/* Includes ------------------------------------------------------------------*/
#include "measures.h" // Module header
#include "IAQTracker/IAQTracker.h"
#include <unistd.h>
#include <cmath>
#include <syslog.h>

/* External variables---------------------------------------------------------*/
Measures::BME_DATA Measures::bme_data = {-1, -1, -1, -1, -1};
/* Private defines -----------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private variables----------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
static float obtain_sea_level_altitude(float pressure);
/* Functions -----------------------------------------------------------------*/

/**
 * @brief Class constructor without parameters as default.
 *
 * @param[in] ovsp_temp The temperature oversampling. Must be OVSP_0_X, OVSP_1_X, OVSP_2_X, OVSP_4_X, OVSP_8_X or OVSP_16_X.
 * @param[in] ovsp_press The pressure oversampling. Must be OVSP_0_X, OVSP_1_X, OVSP_2_X, OVSP_4_X, OVSP_8_X or OVSP_16_X.
 * @param[in] ovsp_hum The humidity oversampling. Must be OVSP_0_X, OVSP_1_X, OVSP_2_X, OVSP_4_X, OVSP_8_X or OVSP_16_X.
 * @param[in] target_gas_temp  The temperature that the hot plate of the gas sensor will reach to get the measure.
 * @param[in] gas_ms  The time to obtain the desired temperature in the hot plate of the gas sensor. Should be greater than 30ms.
 * @param[in] measure_rate_us The time in microseconds between the end of one measure and the start of the next.
 *
 */
void Measures::Meas::init(uint8_t ovsp_temp, uint8_t ovsp_press, uint8_t ovsp_hum,
    float target_gas_temp, uint16_t gas_ms, uint32_t measure_rate_us){

  gas_on = true;
  temp_on = true;
  hum_on = true;
  press_on = true;
  this->ovsp_temp = ovsp_temp;
  this->ovsp_press = ovsp_press;
  this->ovsp_hum = ovsp_hum;
  this->target_gas_temp = target_gas_temp;
  this->gas_ms = gas_ms;
  this->measure_rate_us = measure_rate_us;

  sensor.init();
  sensor.set_oversamplings(ovsp_temp, ovsp_press, ovsp_hum);
  sensor.set_heater_configurations(gas_on, target_gas_temp, gas_ms);
}


/**
 * @brief Set the offset to extract to the resulting temperature read from the temperature sensor.
 *        As the rest of the measures depends on the temperatures measures, this change also will affect them.
 *
 * @param[in] offset The temp offset to be substracted.
 *
 */
void Measures::Meas::set_temp_offset(float offset){
  sensor.set_temp_offset(offset);
}

/**
 * @brief Set the state of the gas sensor from the BME (ON or OFF).
 *
 * @param[in] state The desired state for the sensor.
 *
 */
void Measures::Meas::set_gas_state(bool state){
  gas_on = state;
  sensor.set_heater_configurations(gas_on, target_gas_temp, gas_ms);
}


/**
 * @brief Set the state of the temperature sensor from the BME (ON or OFF).This will also affect to the humidity and
 *        pressure measures,since these are partially calculated from the temperature measures.
 *
 * @param[in] state The desired state for the sensor.
 *
 */
void Measures::Meas::set_temp_state(bool state){
  temp_on = state;
  change_oversamplings();
}


/**
 * @brief Set the state of the humidity sensor from the BME (ON or OFF).
 *
 * @param[in] state The desired state for the sensor.
 *
 */
void Measures::Meas::set_hum_state(bool state){
  hum_on = state;
  change_oversamplings();
}


/**
 * @brief Set the state of the pressure sensor from the BME (ON or OFF). This will also affect to the altitude measures,
 *        since these are calculated from the pressure measures.
 *
 * @param[in] state The desired state for the sensor.
 *
 */
void Measures::Meas::set_press_state(bool state){
  press_on = state;
  change_oversamplings();
}


/**
 * @brief Starts the measuring cycle in a different thread. It returns instantly.
 *
 */
void Measures::Meas::start_measures(){
  if(!run){
    run = true;
    measures_thread = new std::thread (this->thread, this);
  }
}


void Measures::Meas::change_oversamplings(){
  uint8_t ovsp_temp, ovsp_press, ovsp_hum;
  if(temp_on)
    ovsp_temp = this->ovsp_temp;
  else
    ovsp_temp = OVSP_0_X;

  if(press_on)
    ovsp_press = this->ovsp_press;
  else
    ovsp_press = OVSP_0_X;

  if(hum_on)
    ovsp_hum = this->ovsp_hum;
  else
    ovsp_hum = OVSP_0_X;

  sensor.set_oversamplings(ovsp_temp, ovsp_press, ovsp_hum);
}


void Measures::Meas::thread(Meas *meas){
  float temperature, pressure, humidity, gas_resistance, iaq;
  IAQTracker tracker;

  openlog("Weather app", 0, LOG_LOCAL1);

  while(meas->run){
    meas->sensor.get_data_one_measure(&temperature, &pressure, &humidity, &gas_resistance);
    bme_data.temperature = temperature;
    bme_data.pressure = pressure;
    bme_data.humidity = humidity;
    bme_data.altitude = obtain_sea_level_altitude(pressure);

    if(tracker.get_IAQ(&iaq, temperature, humidity, gas_resistance) != -1)
      bme_data.iaq = iaq;

    syslog(LOG_INFO, "Temp: %.2f ºC,  Hum: %.2f %%,  Press: %.2f Pa,  Alt: %.2f m,  IAQ: %.2f\n",
        (float)bme_data.temperature,
        (float)bme_data.pressure,
        (float)bme_data.humidity,
        (float)bme_data.altitude,
        (float)bme_data.iaq);

    usleep(meas->measure_rate_us);
  }

}


/**
 * @brief End communications with all the sensors, stops all the measuring procedures and free all the related resources.
 */
Measures::Meas::~Meas(){
  run = false;
  measures_thread->join();
}



/* Private functions ---------------------------------------------------------*/

/**
  * @brief     Converts a pressure measurement into a height in meters.
  * @param[in] pressure The pressure in Pascals.
  * @return    floating point altitude in meters.
  */
static float obtain_sea_level_altitude(float pressure){
  return 44330.76923 * (1 - std::pow((float)(pressure / 101325.00), 0.190266));
}
