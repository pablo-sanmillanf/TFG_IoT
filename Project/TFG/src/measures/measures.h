/**
  ******************************************************************************
  * @file   measures.h
  * @author Pablo San Millán Fierro (pablo.sanmillanf@alumnos.upm.es)
  * @brief  Measures gatherer tread handler Module Header.
  *
  * @note   End-of-degree work.
  *         This module gather all the measures from the sensors.
  ******************************************************************************
*/

#ifndef __MEASURES_H__
#define __MEASURES_H__

/* Includes ------------------------------------------------------------------*/

#include <atomic>
#include <thread>
#include "BME688/BME688.h"

namespace Measures{

/* Exported types ------------------------------------------------------------*/

typedef struct {
    std::atomic<float> temperature;
    std::atomic<float> pressure;
    std::atomic<float> humidity;
    std::atomic<float> iaq;
    std::atomic<float> altitude;
}BME_DATA;

/* Exported variables --------------------------------------------------------*/
extern BME_DATA bme_data;
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported Functions --------------------------------------------------------*/

class Meas{
  bool run = false;
  BME688 sensor;
  bool gas_on;
  bool temp_on;
  bool hum_on;
  bool press_on;
  uint8_t ovsp_temp;
  uint8_t ovsp_press;
  uint8_t ovsp_hum;
  float target_gas_temp;
  uint16_t gas_ms;
  std::thread *measures_thread;
  uint32_t measure_rate_us;

  void init(uint8_t ovsp_temp, uint8_t ovsp_press, uint8_t ovsp_hum, float target_gas_temp, uint16_t gas_ms, uint32_t measure_rate_us);
  void change_oversamplings();
  static void thread(Meas *meas);
public:

  /**
   * @brief Class constructor with all the parameters as default.
   */
  Meas (): Meas (OVSP_16_X, OVSP_16_X, OVSP_16_X, 300, 100, 25, 0, 100000) {};

  /**
   * @brief Class constructor with parameters related with BME configurations as default.
   *
   * @param[in] temp_offset The temperature offset to be subtracted to the compensated temperature given by the BME sensor.
   * @param[in] measure_rate_us The time in microseconds between the end of one measure and the start of the next.
   *
   */
  Meas (float temp_offset, uint32_t measure_rate_us): Meas (OVSP_16_X, OVSP_16_X, OVSP_16_X, 300, 100, 25, temp_offset, measure_rate_us) {};

  /**
   * @brief Class constructor with parameters related with BME oversamplings as default.
   *
   * @param[in] target_gas_temp  The temperature that the hot plate of the gas sensor will reach to get the measure.
   * @param[in] measure_rate_us  The time to obtain the desired temperature in the hot plate of the gas sensor. Should be greater than 30ms.
   * @param[in] amb_temp         An estimated ambient temperature for the gas sensor. The value doesn't have to be very accurate and a value
   *                             near 25ºC can be correct.
   * @param[in] temp_offset The temperature offset to be subtracted to the compensated temperature given by the BME sensor.
   * @param[in] measure_rate_us The time in microseconds between the end of one measure and the start of the next.
   *
   */
  Meas (float target_gas_temp, uint16_t gas_ms, float amb_temp, float temp_offset, uint32_t measure_rate_us): Meas (OVSP_16_X, OVSP_16_X, OVSP_16_X, target_gas_temp, gas_ms, amb_temp, temp_offset, measure_rate_us) {};

  /**
   * @brief Class constructor without parameters as default.
   *
   * @param[in] ovsp_temp The temperature oversampling. Must be OVSP_0_X, OVSP_1_X, OVSP_2_X, OVSP_4_X, OVSP_8_X or OVSP_16_X.
   * @param[in] ovsp_press The pressure oversampling. Must be OVSP_0_X, OVSP_1_X, OVSP_2_X, OVSP_4_X, OVSP_8_X or OVSP_16_X.
   * @param[in] ovsp_hum The humidity oversampling. Must be OVSP_0_X, OVSP_1_X, OVSP_2_X, OVSP_4_X, OVSP_8_X or OVSP_16_X.
   * @param[in] target_gas_temp  The temperature that the hot plate of the gas sensor will reach to get the measure.
   * @param[in] gas_ms           The time to obtain the desired temperature in the hot plate of the gas sensor. Should be greater than 30ms.
   * @param[in] amb_temp         An estimated ambient temperature for the gas sensor. The value doesn't have to be very accurate and a value
   *                             near 25ºC can be correct.
   * @param[in] temp_offset The temperature offset to be subtracted to the compensated temperature given by the BME sensor.
   * @param[in] measure_rate_us The time in microseconds between the end of one measure and the start of the next.
   *
   */
  Meas (uint8_t ovsp_temp, uint8_t ovsp_press, uint8_t ovsp_hum, float target_gas_temp, uint16_t gas_ms, float amb_temp,
      float temp_offset, uint32_t measure_rate_us):sensor(amb_temp, temp_offset){
    this->init(ovsp_temp, ovsp_press, ovsp_hum, target_gas_temp, gas_ms, measure_rate_us);};


  /**
   * @brief Set the offset to extract to the resulting temperature read from the temperature sensor.
   *        As the rest of the measures depends on the temperatures measures, this change also will affect them.
   *
   * @param[in] offset The temp offset to be substracted.
   *
   */
  void set_temp_offset(float offset);


  /**
   * @brief Set the state of the gas sensor from the BME (ON or OFF).
   *
   * @param[in] state The desired state for the sensor.
   *
   */
  void set_gas_state(bool state);

  /**
   * @brief Set the state of the temperature sensor from the BME (ON or OFF).This will also affect to the humidity and
   *        pressure measures,since these are partially calculated from the temperature measures.
   *
   * @param[in] state The desired state for the sensor.
   *
   */
  void set_temp_state(bool state);

  /**
   * @brief Set the state of the humidity sensor from the BME (ON or OFF).
   *
   * @param[in] state The desired state for the sensor.
   *
   */
  void set_hum_state(bool state);

  /**
   * @brief Set the state of the pressure sensor from the BME (ON or OFF). This will also affect to the altitude measures,
   *        since these are calculated from the pressure measures.
   *
   * @param[in] state The desired state for the sensor.
   *
   */
  void set_press_state(bool state);

  /**
   * @brief Starts the measuring cycle in a different thread. It returns instantly.
   *
   */
  void start_measures();

  /**
   * @brief End communications with all the sensors, stops all the measuring procedures and free all the related resources.
   */
  ~Meas();

};

}


#endif /* __MEASURES_H__ */
