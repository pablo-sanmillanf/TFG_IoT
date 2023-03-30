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
  BME688(float amb_temp): amb_temp(amb_temp){};
  int init();
  int set_oversamplings(uint8_t ovsp_temp, uint8_t ovsp_press, uint8_t ovsp_hum);
  int set_heater_configurations(bool run_gas, float target_temp, uint16_t ms);
  int get_data_one_measure(float *temperature, float *pressure, float *humidity, float *gas_resistance);
};

#ifdef __cplusplus
}
#endif

#endif
