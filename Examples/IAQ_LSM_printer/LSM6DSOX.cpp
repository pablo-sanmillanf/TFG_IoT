/**
  ******************************************************************************
  * @file   LSM6DSOX.cpp
  * @author Pablo San Mill�n Fierro (pablo.sanmillanf@alumnos.upm.es)
  * @brief  LSM6DSOX Sensor Handler Module.
  *
  * @note   End-of-degree work.
  *         This module manages the LSM6DSOX sensor
  ******************************************************************************
*/
/* Includes ------------------------------------------------------------------*/
#include "LSM6DSOX.h" // Module header

/* Private typedef -----------------------------------------------------------*/
#define ADR_LSM  0x6A

#define RESET_REG    0x12
#define RESET_VALUE  0xA3

#define CONF_ACC_GYR_REG  0x10

#define TEMP_DATA_REG 0x20
#define GYR_DATA_REG  0x22
#define ACC_DATA_REG  0x28

#define ODR_MASK 0xF0
#define ACC_FSR_MASK 0x0C
#define GYR_FSR_MASK 0x0E

/* Private variables----------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
static int get_values(float *x_value, float *y_value, float *z_value, int reg, int scale_range);
static float convert_value(int16_t value, int scale_range);
/* Functions -----------------------------------------------------------------*/

/**
 * @brief Class constructor. Starts the LSM6DSOX Sensor with the given configurations.
 * 
 * @param[in] odr_acc Value of ODR for the accelerometer.
 * @param[in] odr_gyr Value of ODR for the gyroscope.
 * @param[in] fsr_acc Value of FSR for the accelerometer.
 * @param[in] fsr_gyr Value of FSR for the gyroscope.
 */
LSM6DSOX::LSM6DSOX(uint8_t odr_acc, uint8_t odr_gyr, uint8_t fsr_acc, uint8_t fsr_gyr) {
  int len = 3;
  uint8_t data[len];
  
  //Start I2C communications
  I2C_Master::start(1);

  //Reset
  data[0] = RESET_REG;
  data[1] = RESET_VALUE;  
  I2C_Master::write_msg(ADR_LSM, data,  2);
  
  fsr_odr_reg_acc = odr_acc | fsr_acc;
  fsr_odr_reg_gyr = odr_gyr | fsr_gyr;
  //Configure accelerometer and gyroscope
  data[0] = CONF_ACC_GYR_REG;
  data[1] = fsr_odr_reg_acc;
  data[2] = fsr_odr_reg_gyr;
  I2C_Master::write_msg(ADR_LSM, data,  3);
}

/**
 * @brief Class destructor. Free all the associated resources.
 */
LSM6DSOX::~LSM6DSOX() {
  I2C_Master::end();
}

/**
 * @brief Sets the Full Scale Ranges for the gyroscope and the accelerometer according
 *        to the defines listed in the module header.
 * 
 * @param[in] fsr_acc Value of FSR for the accelerometer.
 * @param[in] fsr_gyr Value of FSR for the gyroscope.
 *
 * @return 0 if success, -1 if error.
 */
int LSM6DSOX::set_fsr(uint8_t fsr_acc, uint8_t fsr_gyr) {
  int len = 3;
  uint8_t data[len];
  
  fsr_odr_reg_acc = (fsr_odr_reg_acc & (~ACC_FSR_MASK)) | fsr_acc;
  fsr_odr_reg_gyr = (fsr_odr_reg_gyr & (~GYR_FSR_MASK)) | fsr_gyr;
  
  //Configure accelerometer and gyroscope
  data[0] = CONF_ACC_GYR_REG;
  data[1] = fsr_odr_reg_acc;
  data[2] = fsr_odr_reg_gyr;
  
  if(I2C_Master::write_msg(ADR_LSM, data,  3) == -1)
    return -1;
  
  return 0;
}

/**
 * @brief Sets the Output Data Rate for the gyroscope and the accelerometer according
 *        to the defines listed in the module header.
 * 
 * @param[in] odr_acc Value of ODR for the accelerometer.
 * @param[in] odr_gyr Value of ODR for the gyroscope.
 *
 * @return 0 if success, -1 if error.
 */
int LSM6DSOX::set_odr(uint8_t odr_acc, uint8_t odr_gyr) {
  int len = 3;
  uint8_t data[len];
  
  fsr_odr_reg_acc = (fsr_odr_reg_acc & (~ODR_MASK)) | odr_acc;
  fsr_odr_reg_gyr = (fsr_odr_reg_gyr & (~ODR_MASK)) | odr_gyr;
  
  //Configure accelerometer and gyroscope
  data[0] = CONF_ACC_GYR_REG;
  data[1] = fsr_odr_reg_acc;
  data[2] = fsr_odr_reg_gyr;
  
  if(I2C_Master::write_msg(ADR_LSM, data,  3) == -1)
    return -1;
  
  return 0;
}


/**
 * @brief Gets the temperature from the sensor.
 *
 * @param[out] temperature The temperature obtained from the sensor.
 *
 * @return 0 if success, -1 if error.
 */
int LSM6DSOX::get_temperature(float *temperature){
  int len = 2;
  uint8_t data[len];
  if(I2C_Master::read_msg(ADR_LSM, TEMP_DATA_REG, data, len) == -1)
    return -1;

  *temperature = (float)((int16_t)(data[1] << 8 | data[0])) / 256.0 + 25.0;
   
  return 0;
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
  
  uint8_t scale_range = fsr_odr_reg_acc & ACC_FSR_MASK;
  
  if(scale_range == ACC_16_G_FSR){
    return get_values(x_value, y_value, z_value, ACC_DATA_REG, 16);
  }
  else if(scale_range == ACC_8_G_FSR){
    return get_values(x_value, y_value, z_value, ACC_DATA_REG, 8);
  }
  else if(scale_range == ACC_4_G_FSR){
    return get_values(x_value, y_value, z_value, ACC_DATA_REG, 4);
  }
  else if(scale_range == ACC_2_G_FSR){
    return get_values(x_value, y_value, z_value, ACC_DATA_REG, 2);
  }
  
  return -1;
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

  uint8_t scale_range = fsr_odr_reg_gyr & GYR_FSR_MASK;
  
  if(scale_range == GYR_2000_DPS_FSR){
    return get_values(x_value, y_value, z_value, GYR_DATA_REG, 2000);
  }
  else if(scale_range == GYR_1000_DPS_FSR){
    return get_values(x_value, y_value, z_value, GYR_DATA_REG, 1000);
  }
  else if(scale_range == GYR_500_DPS_FSR){
    return get_values(x_value, y_value, z_value, GYR_DATA_REG, 500);
  }
  else if(scale_range == GYR_250_DPS_FSR){
    return get_values(x_value, y_value, z_value, GYR_DATA_REG, 250);
  }
  else if(scale_range == GYR_125_DPS_FSR){
    return get_values(x_value, y_value, z_value, GYR_DATA_REG, 125);
  }
  
  return -1;
}


/* Private functions ---------------------------------------------------------*/

/**
 * @brief Gets values from the gyroscope or the accelerometer from the 3 axis.
 * 
 * @param[out] x_value     Pointer to the location where the x-axis value is to be 
 *                         saved.
 * @param[out] y_value     Pointer to the location where the y-axis value is to be 
 *                         saved.
 * @param[out] z_value     Pointer to the location where the z-axis value is to be 
 *                         saved.
 * @param[in] reg          First I2C register to read from.
 * @param[in] scale_range  Scale range of the values.
 *
 * @return 0 if success, -1 if error.
 */
static int get_values(float *x_value, float *y_value, float *z_value, int reg, int scale_range){

  int len = 6;
  uint8_t data[len];
  if(I2C_Master::read_msg(ADR_LSM, reg, data, len) == -1)
    return -1;
    
  *x_value = convert_value(data[1] << 8 | data[0], scale_range);
  *y_value = convert_value(data[3] << 8 | data[2], scale_range);
  *z_value = convert_value(data[5] << 8 | data[4], scale_range);
  
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


