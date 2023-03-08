/**
  ******************************************************************************
  * @file   LSM6DSOX.cpp
  * @author Pablo San Millán Fierro (pablo.sanmillanf@alumnos.upm.es)
  * @brief  LSM6DSOX Sensor Handler Module.
  *
  * @note   End-of-degree work.
  *         This module manages the LSM6DSOX sensor
  ******************************************************************************
*/
/* Includes ------------------------------------------------------------------*/
#include "LSM6DSOX.h" // Module header
#include "../i2c_master/i2c_master.h"

/* Private typedef -----------------------------------------------------------*/
#define ADR_LSM  0x6A

#define RESET_REG    0x12
#define RESET_VALUE  0xA3

#define CONF_ACC_GYR_REG  0x10

#define ACC_SCALE_RANGE 2
#define GYR_SCALE_RANGE 250
/* Private variables----------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
static float convert_value(int16_t value, int scale_range);
/* Functions -----------------------------------------------------------------*/

/**
 * @brief Starts the LSM6DSOX Sensor with the given configurations.
 * 
 *
 * @return 0 if success, -1 if error.
 */
int LSM6DSOX::init () {
  int len = 3;
  uint8_t data[len];
  
  //Start I2C communications
  I2C_Master::start(1);

  //Reset
  data[0] = RESET_REG;
  data[1] = RESET_VALUE;  
  if(I2C_Master::write_msg(ADR_LSM, data,  2) == -1)
    return -1;
  
  //Configure accelerometer and gyroscope
  data[0] = CONF_ACC_GYR_REG;
  data[1] = 0x30;
  data[2] = 0x30;
  if(I2C_Master::write_msg(ADR_LSM, data,  3) == -1)
    return -1;
  
  return 0;
}


/**
 * @brief Gets the temperature from the sensor or a 0 if the communications fail.
 * 
 *
 * @return The temperature as a float or 0 if error.
 */
float LSM6DSOX::get_temperature(){
  int len = 2;
  uint8_t data[len];
  if(I2C_Master::read_msg(ADR_LSM, 0x20, data, len) == -1)
    return 0;
   
  return (float)((int16_t)(data[1] << 8 | data[0])) / 256.0 + 25.0;
}



/**
 * @brief Gets the accelerometer values from the 3 axis.
 * 
 * @param[out] x_value Pointer to the location where the x-axis value is to be 
 *                     saved.
 * @param[out] y_value Pointer to the location where the y-axis value is to be 
 *                     saved.
 * @param[out] z_value Pointer to the location where the z-axis value is to be 
 *                     saved.
 *
 * @return 0 if success, -1 if error.
 */
int LSM6DSOX::get_acc_values(float *x_value, float *y_value, float *z_value){

  int len = 6;
  uint8_t data[len];
  if(I2C_Master::read_msg(ADR_LSM, 0x28, data, len) == -1)
    return -1;
  
  *x_value = convert_value(data[1] << 8 | data[0], ACC_SCALE_RANGE);
  *y_value = convert_value(data[3] << 8 | data[2], ACC_SCALE_RANGE);
  *z_value = convert_value(data[5] << 8 | data[4], ACC_SCALE_RANGE);
  
  return 0;
}


/**
 * @brief Gets the gyroscope values from the 3 axis.
 * 
 * @param[out] x_value Pointer to the location where the x-axis value is to be 
 *                     saved.
 * @param[out] y_value Pointer to the location where the y-axis value is to be 
 *                     saved.
 * @param[out] z_value Pointer to the location where the z-axis value is to be 
 *                     saved.
 *
 * @return 0 if success, -1 if error.
 */
int LSM6DSOX::get_gyr_values(float *x_value, float *y_value, float *z_value){

  int len = 6;
  uint8_t data[len];
  if(I2C_Master::read_msg(ADR_LSM, 0x22, data, len) == -1)
    return -1;
  
  *x_value = convert_value(data[1] << 8 | data[0], GYR_SCALE_RANGE);
  *y_value = convert_value(data[3] << 8 | data[2], GYR_SCALE_RANGE);
  *z_value = convert_value(data[5] << 8 | data[4], GYR_SCALE_RANGE);
  
  return 0;
}




/**
 * @brief Converts the value as a int16_t to a value inside the scale_range
 * 
 * @param[in] value        The value to be converted.
 * @param[in] scale_range  Scale range of the value converted.
 *
 * @return The converted value as a float.
 */
static float convert_value(int16_t value, int scale_range){
  float result;
  result = (float)value / 65536.0 * scale_range * 2;

  return result;
}
