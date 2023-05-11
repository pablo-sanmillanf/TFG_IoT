/**
  ******************************************************************************
  * @file   BME688.h
  * @author Pablo San Millán Fierro (pablo.sanmillanf@alumnos.upm.es)
  * @brief  BME688 Handler Sensor Module Header.
  *
  * @note   End-of-degree work.
  *         This module manages the BME688 gas sensor.
  ******************************************************************************
*/

#ifndef __BME688_H__
#define __BME688_H__

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif
/* Exported variables --------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
struct bme688_calib_temperature_sensor
{
  float par_t1;
  float par_t2;
  float par_t3;
  float t_fine;
};

struct bme688_calib_pressure_sensor
{
  float par_p1;
  float par_p2;
  float par_p3;
  float par_p4;
  float par_p5;
  float par_p6;
  float par_p7;
  float par_p8;
  float par_p9;
  float par_p10;
};

struct bme688_calib_humidity_sensor
{
  float par_h1;
  float par_h2;
  float par_h3;
  float par_h4;
  float par_h5;
  float par_h6;
  float par_h7;
};

struct bme688_calib_gas_sensor
{
  float par_g1;
  float par_g2;
  float par_g3;
  float res_heat_range;
  float res_heat_val;
};

struct bme688_calib_sensor
{
  bme688_calib_temperature_sensor temp;
  bme688_calib_pressure_sensor press;
  bme688_calib_humidity_sensor hum;
  bme688_calib_gas_sensor gas;
};

struct bme688_oversamplings
{
  uint8_t temp;
  uint8_t press;
  uint8_t hum;
};
/* Exported constants --------------------------------------------------------*/
#define OVSP_1_X      1
#define OVSP_2_X      2
#define OVSP_4_X      3
#define OVSP_8_X      4
#define OVSP_16_X     5
/* Exported macro ------------------------------------------------------------*/
/* Exported Functions --------------------------------------------------------*/
class BME688{
  bme688_calib_sensor calibs;
  bme688_oversamplings ovsp;
  float amb_temp;
public:

  /**
    * @brief Class constructor. Sets the ambient temperature.
    */
  BME688(float amb_temp): amb_temp(amb_temp){};

  /**
    * @brief Starts the module and the communications with the BME sensor and obtain the calibration parameters from the sensor.
    *
    * @return 0 if success, -1 if error.
    */
  int init();

  /**
    * @brief Set the oversamplings for the temperature, pressure and humidity.
    *
    * @param[in] ovsp_temp The temperature oversampling. Must be OVSP_1_X, OVSP_2_X, OVSP_4_X, OVSP_8_X or OVSP_16_X.
    * @param[in] ovsp_press The pressure oversampling. Must be OVSP_1_X, OVSP_2_X, OVSP_4_X, OVSP_8_X or OVSP_16_X.
    * @param[in] ovsp_hum The humidity oversampling. Must be OVSP_1_X, OVSP_2_X, OVSP_4_X, OVSP_8_X or OVSP_16_X.
    *
    * @return 0 if success, -1 if error.
    */
  int set_oversamplings(uint8_t ovsp_temp, uint8_t ovsp_press, uint8_t ovsp_hum);

  /**
    * @brief Set the configuration parameters of the heater of the gas sensor.
    *
    * @param[in] run_gas If true, the gas sensor is active, false otherwise.
    * @param[in] target_temp The temperature that the hot plate will reach to get the measure.
    * @param[in] ms The time to obtain the desired temperature. Should be greater than 30ms.
    *
    * @return 0 if success, -1 if error.
    */
  int set_heater_configurations(bool run_gas, float target_temp, uint16_t ms);

  /**
    * @brief Obtain a measure from all the metris of the BME sensor. If a specific metric is not needed, pass NULL as parameter.
    *
    * @param[out] temperature The temperature obtained from the sensor.
    * @param[out] pressure The pressure obtained from the sensor.
    * @param[out] humidity The humidity obtained from the sensor.
    * @param[out] gas_resistance The gas resistance of the hot plate obtained from the sensor. Greater values indicates good
    *                            quality air, smaller values indicates bad quality air.
    *
    * @return 0 if success, -1 if error.
    */
  int get_data_one_measure(float *temperature, float *pressure, float *humidity, float *gas_resistance);

  /**
   * @brief End communications with the sensor and free all the related resources.
   */
  ~BME688();
};

#ifdef __cplusplus
}
#endif

#endif
