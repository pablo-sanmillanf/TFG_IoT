/**
  ******************************************************************************
  * @file   BME688.cpp
  * @author Pablo San Millán Fierro (pablo.sanmillanf@alumnos.upm.es)
  * @brief  BME688 Gas Sensor Handler Module.
  *
  * @note   End-of-degree work.
  *         This module manages the BME688 Gas Sensor.
  *         Base on: https://github.com/BoschSensortec/BME68x-Sensor-API
  ******************************************************************************
*/
/* Includes ------------------------------------------------------------------*/
#include "BME688.h"
#include "../i2c_master/i2c_master.h"
#include <unistd.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>

/* Private typedef -----------------------------------------------------------*/
/* Private variables----------------------------------------------------------*/
static uint8_t bme_addr;
/* Private function prototypes -----------------------------------------------*/
static int8_t bme68x_interface_init(struct bme68x_dev *bme, int8_t amb_temp);
static BME68X_INTF_RET_TYPE bme68x_i2c_read(uint8_t read_reg, uint8_t *data, uint32_t len, void *intf_ptr);
static BME68X_INTF_RET_TYPE bme68x_i2c_write(uint8_t write_reg, const uint8_t *data, uint32_t len, void *intf_ptr);
static void bme68x_delay_us(uint32_t period, void *intf_ptr);
static void bme68x_check_rslt(const char api_name[], int8_t rslt);
/* Functions -----------------------------------------------------------------*/

BME688::BME688(uint8_t temp_os, uint8_t hum_os, uint8_t press_os) {
  //Start I2C communications
  int8_t rslt = I2C_Master::start(1);
  bme68x_check_rslt("I2C_Master::start", rslt);
  

  rslt = bme68x_interface_init(&bme, 20);
  bme68x_check_rslt("bme68x_interface_init", rslt);

  rslt = bme68x_init(&bme);
  bme68x_check_rslt("bme68x_init", rslt);

  conf.filter = BME68X_FILTER_OFF;
  conf.odr = BME68X_ODR_NONE;
  conf.os_hum = hum_os;
  conf.os_pres = press_os;
  conf.os_temp = temp_os;
  rslt = bme68x_set_conf(&conf, &bme);
  bme68x_check_rslt("bme68x_set_conf", rslt);

  heatr_conf.enable = BME68X_ENABLE;
  heatr_conf.heatr_temp = 300;
  heatr_conf.heatr_dur = 100;
  rslt = bme68x_set_heatr_conf(BME68X_FORCED_MODE, &heatr_conf, &bme);
  bme68x_check_rslt("bme68x_set_heatr_conf", rslt);
}


/**
 * @brief Class destructor. Free all the associated resources.
 */
BME688::~BME688() {
  I2C_Master::end();
}


int BME688::get_one_measure(float *temperature, float *pressure, float *humidity, float *gas_resistance){
  struct bme68x_data data;
  uint8_t n_fields;
  
  uint8_t rslt = bme68x_set_op_mode(BME68X_FORCED_MODE, &bme);
  bme68x_check_rslt("bme68x_set_op_mode", rslt);
  
  uint32_t del_period = bme68x_get_meas_dur(BME68X_FORCED_MODE, &conf, &bme) + (heatr_conf.heatr_dur * 1000);
  bme.delay_us(del_period, bme.intf_ptr);

  rslt = bme68x_get_data(BME68X_FORCED_MODE, &data, &n_fields, &bme);
  bme68x_check_rslt("bme68x_get_data", rslt);
  
  *temperature = data.temperature;
  *pressure = data.pressure;
  *humidity = data.humidity;
  *gas_resistance = data.gas_resistance;
  
  return rslt;
}


/* Private functions ---------------------------------------------------------*/
static int8_t bme68x_interface_init(struct bme68x_dev *bme, int8_t amb_temp){
  
  int8_t rslt = BME68X_OK;

  if (bme != NULL)
  {
    bme_addr = BME68X_I2C_ADDR_LOW;
    bme->read = bme68x_i2c_read;
    bme->write = bme68x_i2c_write;
    bme->intf = BME68X_I2C_INTF;

    bme->delay_us = bme68x_delay_us;
    bme->intf_ptr = &bme_addr;
    bme->amb_temp = amb_temp;
  }
  else
  {
      rslt = BME68X_E_NULL_PTR;
  }

  return rslt;
}


static BME68X_INTF_RET_TYPE bme68x_i2c_read(uint8_t read_reg, uint8_t *data, uint32_t len, void *intf_ptr){
  
  if(I2C_Master::read_msg(*(uint8_t*)intf_ptr, read_reg, data, len) == -1)
    return -1;
  return 0;
}


static BME68X_INTF_RET_TYPE bme68x_i2c_write(uint8_t write_reg, const uint8_t *data, uint32_t len, void *intf_ptr){
  
  uint8_t buffer[len + 1];
  
  buffer[0] = write_reg;
  for(int i = 0; i<len; i++){
    buffer[i + 1] = data[i];
  }
  if(I2C_Master::write_msg(*(uint8_t*)intf_ptr, buffer,  len + 1) == -1)
    return -1;
  return 0;
}


static void bme68x_delay_us(uint32_t period, void *intf_ptr){
  
  struct timespec ts;
  ts.tv_sec = period / 1000000;
  ts.tv_nsec = (period % 1000000) * 1000;
  nanosleep(&ts, NULL);
}



static void bme68x_check_rslt(const char api_name[], int8_t rslt)
{
  switch (rslt)
  {
    case BME68X_OK:

      /* Do nothing */
      break;
    case BME68X_E_NULL_PTR:
      printf("API name [%s]  Error [%d] : Null pointer\r\n", api_name, rslt);
      break;
    case BME68X_E_COM_FAIL:
      printf("API name [%s]  Error [%d] : Communication failure\r\n", api_name, rslt);
      break;
    case BME68X_E_INVALID_LENGTH:
      printf("API name [%s]  Error [%d] : Incorrect length parameter\r\n", api_name, rslt);
      break;
    case BME68X_E_DEV_NOT_FOUND:
      printf("API name [%s]  Error [%d] : Device not found\r\n", api_name, rslt);
      break;
    case BME68X_E_SELF_TEST:
      printf("API name [%s]  Error [%d] : Self test error\r\n", api_name, rslt);
      break;
    case BME68X_W_NO_NEW_DATA:
      printf("API name [%s]  Warning [%d] : No new data found\r\n", api_name, rslt);
      break;
    default:
      printf("API name [%s]  Error [%d] : Unknown error code\r\n", api_name, rslt);
      break;
  }
}