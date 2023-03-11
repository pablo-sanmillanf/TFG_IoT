/**
  ******************************************************************************
  * @file   BME688.h
  * @author Pablo San Millán Fierro (pablo.sanmillanf@alumnos.upm.es)
  * @brief  BME688 Gas Sensor Handler Module Header.
  *
  * @note   End-of-degree work.
  *         This module manages the BME688 Gas Sensor.
  *         Base on: https://github.com/BoschSensortec/BME68x-Sensor-API
  ******************************************************************************
*/

#ifndef __BME688_H__
#define __BME688_H__

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include "./bme68x/bme68x.h"

#ifdef __cplusplus
extern "C" {
#endif
/* Exported variables --------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Switch off measurement */
#define BME688_OFF_OS          BME68X_OS_NONE
#define BME68X_1_OS            BME68X_OS_1X
#define BME68X_2_OS            BME68X_OS_2X
#define BME68X_4_OS            BME68X_OS_4X
#define BME68X_8_OS            BME68X_OS_8X
#define BME68X_16_OS           BME68X_OS_16X

/* Exported Functions --------------------------------------------------------*/

class BME688{
    bme68x_dev bme;
    bme68x_conf conf;
    bme68x_heatr_conf heatr_conf;
  public:
    BME688() : BME688(BME68X_1_OS, BME68X_1_OS, BME68X_1_OS) {};
    BME688(uint8_t temp_os, uint8_t hum_os, uint8_t press_os);
    ~BME688();
    int get_one_measure(float *temperature, float *pressure, float *humidity, float *gas_resistance);
    
    
};


#ifdef __cplusplus
}
#endif

#endif
 
 
