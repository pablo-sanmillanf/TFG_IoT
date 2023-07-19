/**
  ******************************************************************************
  * @file   BME688.cpp
  * @author Pablo San Millán Fierro (pablo.sanmillanf@alumnos.upm.es)
  * @brief  BME688 Handler Sensor Module.
  *
  * @note   End-of-degree work.
  *         This module manages the BME688 gas sensor.
  ******************************************************************************
*/
/* Includes ------------------------------------------------------------------*/
#include "BME688.h" // Module header
#include "../i2c_master/i2c_master.h"
#include <unistd.h>

/* Private defines -----------------------------------------------------------*/
#define BME688_ADRR       0x76

#define MAX_GAS_WAIT_TIME 0xFC0

#define RESET_REG                         0xE0
#define CHIP_ID_REG                       0xD0
#define VARIANT_ID                        0xF0
#define CONTROL_MEASURES_REG              0x74
#define CONTROL_HUMIDITY_REG              0x72
#define CONTROL_GAS_0_REG                 0x70
#define CONTROL_GAS_1_REG                 0x71
#define RESISTANCE_HEATER_0_REG           0x5A
#define GAS_WAIT_0_REG                    0x64
#define START_DATA_FIELD_0_REG            0x1D

#define START_GROUP_1_CALIB_REGS          0x8A
#define START_GROUP_2_CALIB_REGS          0xE1
#define START_GROUP_3_CALIB_REGS          0x00

#define LEN_GROUP_1_CALIB_REGS            23
#define LEN_GROUP_2_CALIB_REGS            14
#define LEN_GROUP_3_CALIB_REGS            5

#define LEN_DATA_FIELD_0                  17

#define RESET_VALUE         0xB6
#define CHIP_ID_VALUE       0x61
#define VARIANT_ID_VALUE    0x01

#define OP_MODE_MASK        0x03
#define OVSP_MASK           0x07
#define NB_CONV_MASK        0x0F
#define NEW_DATA_MSK        0x80
#define GAS_RANGE_MSK       0x0F

#define SLEEP_OP_MODE         0
#define FORCED_OP_MODE        1
#define PARALLEL_OP_MODE      2

#define OVSP_TEMP_POS     5
#define OVSP_PRESS_POS    2
#define OVSP_HUM_POS      0

#define HEATER_OFF_POS    3
#define RUN_GAS_POS       5

#define TEMPERATURE_T1_LSB  8
#define TEMPERATURE_T1_MSB  9
#define TEMPERATURE_T2_LSB  0
#define TEMPERATURE_T2_MSB  1
#define TEMPERATURE_T3      2

#define PRESSURE_P1_LSB   4
#define PRESSURE_P1_MSB   5
#define PRESSURE_P2_LSB   6
#define PRESSURE_P2_MSB   7
#define PRESSURE_P3       8
#define PRESSURE_P4_LSB   10
#define PRESSURE_P4_MSB   11
#define PRESSURE_P5_LSB   12
#define PRESSURE_P5_MSB   13
#define PRESSURE_P6       15
#define PRESSURE_P7       14
#define PRESSURE_P8_LSB   18
#define PRESSURE_P8_MSB   19
#define PRESSURE_P9_LSB   20
#define PRESSURE_P9_MSB   21
#define PRESSURE_P10      22

#define HUMIDITY_H1_MSB     2
#define HUMIDITY_H1_H2_LSB  1
#define HUMIDITY_H2_MSB     0
#define HUMIDITY_H3         3
#define HUMIDITY_H4         4
#define HUMIDITY_H5         5
#define HUMIDITY_H6         6
#define HUMIDITY_H7         7

#define GAS_G1              12
#define GAS_G2_LSB          10
#define GAS_G2_MSB          11
#define GAS_G3              13
#define RES_HEAT_VAL        0
#define RES_HEAT_RANGE      2
#define RANGE_SW_ERROR      4
/* Private typedef -----------------------------------------------------------*/
/* Private variables----------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
static int get_data_forced_mode(float *temperature, float *pressure, float *humidity, float *gas_resistance,
                                float temp_offset, bme688_calib_sensor *calibs);
static int set_operation_mode(uint8_t mode);
static uint32_t get_measure_duration(uint8_t mode, bme688_oversamplings ovsp);
static int get_calibs(bme688_calib_sensor *calibs);
static int set_heat_gas_confs(uint8_t mode, float target_temp, float amb_temp, uint16_t ms, bme688_calib_gas_sensor gas_cals);
static uint8_t calc_res_heat_x(float target_temp, float amb_temp, bme688_calib_gas_sensor gas_cals);
static uint8_t calc_gas_wait_x(uint16_t ms);
static float calc_compensated_temperature(float temp_adc, float temp_offset, bme688_calib_sensor *calibs);
static float calc_compensated_pressure(float press_adc, bme688_calib_sensor *calibs);
static float calc_compensated_humidity(float hum_adc, bme688_calib_sensor *calibs);
static float calc_compensated_gas_resistance(uint16_t gas_adc, uint8_t gas_range);


/* Functions -----------------------------------------------------------------*/
/**
  * @brief Starts the module and the communications with the BME sensor and obtain the calibration parameters from the sensor.
  *
  * @return 0 if success, -1 if error.
  */
int BME688::init(){
  uint8_t buffer[2];
  I2C_Master::start(1);

  buffer[0] = RESET_REG;
  buffer[1] = RESET_VALUE;
  if(I2C_Master::write_msg(BME688_ADRR, buffer, 2) == -1)
    return -1;
  usleep(10000);

  if(I2C_Master::read_msg(BME688_ADRR, CHIP_ID_REG, buffer, 1) == -1)
    return -1;

  if(I2C_Master::read_msg(BME688_ADRR, VARIANT_ID, buffer, 1) == -1)
    return -1;

  if(buffer[0] != VARIANT_ID_VALUE)
    return -1;

  return get_calibs(&calibs);
}

/**
  * @brief Set a new temperature offset to be subtracted to the compensated temperature. This will also
  *        affect to the humidity measures.
  *
  * @param[in] temp_offset The new temperature offset to be subtracted to the compensated temperature.
  *
  */
void BME688::set_temp_offset(float temp_offset){
  this->temp_offset = temp_offset;
}


/**
  * @brief Set the oversamplings for the temperature, pressure and humidity.
  *
  * @param[in] ovsp_temp The temperature oversampling. Must be OVSP_1_X, OVSP_2_X, OVSP_4_X, OVSP_8_X or OVSP_16_X.
  * @param[in] ovsp_press The pressure oversampling. Must be OVSP_1_X, OVSP_2_X, OVSP_4_X, OVSP_8_X or OVSP_16_X.
  * @param[in] ovsp_hum The humidity oversampling. Must be OVSP_1_X, OVSP_2_X, OVSP_4_X, OVSP_8_X or OVSP_16_X.
  *
  * @return 0 if success, -1 if error.
  */
int BME688::set_oversamplings(uint8_t ovsp_temp, uint8_t ovsp_press, uint8_t ovsp_hum){
  uint8_t buffer[4];

  ovsp.temp = ovsp_temp;
  ovsp.press = ovsp_press;
  ovsp.hum = ovsp_hum;

  if(I2C_Master::read_msg(BME688_ADRR, CONTROL_HUMIDITY_REG, &buffer[1], 1) == -1)
    return -1;

  if(I2C_Master::read_msg(BME688_ADRR, CONTROL_MEASURES_REG, &buffer[3], 1) == -1)
    return -1;
  buffer[0] = CONTROL_HUMIDITY_REG;
  buffer[1] = (buffer[1] & ~OVSP_MASK) | (ovsp_hum & OVSP_MASK);
  buffer[2] = CONTROL_MEASURES_REG;
  buffer[3] = (buffer[3] & ~(OVSP_MASK << OVSP_TEMP_POS | OVSP_MASK << OVSP_PRESS_POS)) |
      (ovsp_temp & OVSP_MASK) << OVSP_TEMP_POS | (ovsp_press & OVSP_MASK) << OVSP_PRESS_POS;

  if(I2C_Master::write_msg(BME688_ADRR, buffer, 4) == -1)
    return -1;
  return 0;
}


/**
  * @brief Set the configuration parameters of the heater of the gas sensor.
  *
  * @param[in] run_gas If true, the gas sensor is active, false otherwise.
  * @param[in] target_temp The temperature that the hot plate will reach to get the measure.
  * @param[in] ms The time to obtain the desired temperature. Should be greater than 30ms.
  *
  * @return 0 if success, -1 if error.
  */
int BME688::set_heater_configurations(bool run_gas, float target_temp, uint16_t ms){
  uint8_t buffer[4];
  uint8_t nb_conv = 0;
  if(I2C_Master::read_msg(BME688_ADRR, CONTROL_GAS_0_REG, &buffer[1], 1) == -1)
    return -1;

  if(I2C_Master::read_msg(BME688_ADRR, CONTROL_GAS_1_REG, &buffer[3], 1) == -1)
    return -1;

  if(set_heat_gas_confs(FORCED_OP_MODE, target_temp, amb_temp, ms, calibs.gas))
    return -1;

  buffer[0] = CONTROL_GAS_0_REG;
  buffer[1] = (buffer[1] & ~(1 << HEATER_OFF_POS)) | (uint8_t)(!run_gas) << HEATER_OFF_POS;
  buffer[2] = CONTROL_GAS_1_REG;
  buffer[3] = (buffer[3] & ~(1 << RUN_GAS_POS | NB_CONV_MASK)) | (uint8_t)run_gas << RUN_GAS_POS | (nb_conv & NB_CONV_MASK);

  if(I2C_Master::write_msg(BME688_ADRR, buffer, 4) == -1)
    return -1;
  return 0;
}


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
int BME688::get_data_one_measure(float *temperature, float *pressure, float *humidity, float *gas_resistance){
  set_operation_mode(FORCED_OP_MODE);

  usleep(get_measure_duration(FORCED_OP_MODE, ovsp) + 100000);

  return get_data_forced_mode(temperature, pressure, humidity, gas_resistance, temp_offset, &calibs);
}


/**
  * @brief End communications with the sensor and free all the related resources.
  */
BME688::~BME688(){
  I2C_Master::end();
}


/* Private functions ---------------------------------------------------------*/
/**
  * @brief Set the operation mode of the BME sensor. Can be SLEEP_OP_MODE, FORCED_OP_MODE or PARALLEL_OP_MODE. In practice,
  * only SLEEP_OP_MODE (sensor off) and FORCED_OP_MODE (one measure, after that sensor off) will be used.
  *
  * @param[in] mode The operation mode. Can be SLEEP_OP_MODE, FORCED_OP_MODE or PARALLEL_OP_MODE.
  *
  * @return 0 if success, -1 if error.
  */
static int set_operation_mode(uint8_t mode){
  uint8_t buffer[2];
  if(I2C_Master::read_msg(BME688_ADRR, CONTROL_MEASURES_REG, &buffer[1], 1) == -1)
    return -1;

  buffer[0] = CONTROL_MEASURES_REG;
  buffer[1] = (buffer[1] & ~OP_MODE_MASK) | (mode & OP_MODE_MASK);

  if(I2C_Master::write_msg(BME688_ADRR, buffer, 2) == -1)
    return -1;
  return 0;
}


/**
  * @brief Calculate the time that the measure will take. As the datasheet has almost no information of those times,
  * the calculations have been taken from the open library created by Bosch: BME68x-Sensor-API.
  *
  * @param[in] mode The operation mode. Can be SLEEP_OP_MODE, FORCED_OP_MODE or PARALLEL_OP_MODE.
  * @param[in] ovsp A structure with the oversamplings for temperature, pressure and humidity.
  *
  * @return 0 if success, -1 if error.
  */
static uint32_t get_measure_duration(uint8_t mode, bme688_oversamplings ovsp){
  uint8_t meas_cycles = 0;
  uint32_t us = 0;
  uint8_t ovsp_values[] = {0, 1, 2, 4, 8, 16};

  meas_cycles = ovsp_values[ovsp.temp] + ovsp_values[ovsp.press] + ovsp_values[ovsp.hum];

  //Copied from BME68x-Sensor-API. There are no time references in the datasheet
  /* TPH measurement duration */
  us = meas_cycles * 1963;
  us += 477 * 4; /* TPH switching duration */
  us += 477 * 5; /* Gas measurement duration */

  if(mode != PARALLEL_OP_MODE)
    us += 1000;

  return us;
}


/**
  * @brief Obtain a measure from all the metrics of the BME sensor in the forced mode. If a specific metric is not needed,
  * pass NULL as parameter.
  *
  * @param[out] temperature The temperature obtained from the sensor.
  * @param[out] pressure The pressure obtained from the sensor.
  * @param[out] humidity The humidity obtained from the sensor.
  * @param[out] gas_resistance The gas resistance of the hot plate obtained from the sensor. Greater values indicates good
  *                            quality air, smaller values indicates bad quality air.
  * @param[in] temp_offset A temperature offset to be subtracted to the resulting temperature. This parameter will compensate
  *                        all the offset added by PCB temperature, sensor element self-heating, etc.
  * @param[in] calibs A structure with the calibration parameters for the calculation of the compensated metrics.
  *
  *
  * @return 0 if success, -1 if error.
  */
static int get_data_forced_mode(float *temperature, float *pressure, float *humidity, float *gas_resistance,
                                float temp_offset, bme688_calib_sensor *calibs){
  uint8_t buffer[LEN_DATA_FIELD_0];
  uint8_t gas_range;
  uint32_t adc_temp;
  uint32_t adc_pres;
  uint16_t adc_hum;
  uint16_t adc_gas_res;
  uint8_t tries = 5;

  do{
    if(I2C_Master::read_msg(BME688_ADRR, START_DATA_FIELD_0_REG, buffer, LEN_DATA_FIELD_0) != -1){
      if(buffer[0] & NEW_DATA_MSK){

        if(temperature != NULL){
          adc_temp = (uint32_t)(((uint32_t)buffer[5] * 4096) | ((uint32_t)buffer[6] * 16) | ((uint32_t)buffer[7] / 16));
          *temperature = calc_compensated_temperature(adc_temp, temp_offset, calibs);
        }

        if(pressure != NULL){
          adc_pres = (uint32_t)(((uint32_t)buffer[2] * 4096) | ((uint32_t)buffer[3] * 16) | ((uint32_t)buffer[4] / 16));
          *pressure = calc_compensated_pressure(adc_pres, calibs);
        }
        if(humidity != NULL){
          adc_hum = (uint16_t)(((uint32_t)buffer[8] * 256) | (uint32_t)buffer[9]);
          *humidity = calc_compensated_humidity(adc_hum, calibs);
        }
        if(gas_resistance != NULL){
          adc_gas_res = (uint16_t)((uint32_t)buffer[15] * 4 | (((uint32_t)buffer[16]) / 64));
          gas_range = buffer[16] & GAS_RANGE_MSK;
          *gas_resistance = calc_compensated_gas_resistance(adc_gas_res, gas_range);
        }

        return 0;
      }
    }
    tries--;
  }while(tries);

  return -1;
}


/**
  * @brief Obtain all the calibration parameters from the registers of the BME688 sensor for the calculation of the
  * compensated metrics.
  *
  * @param[out] calibs A structure with the calibration parameters for the calculation of the compensated metrics.
  *
  * @return 0 if success, -1 if error.
  */
static int get_calibs(bme688_calib_sensor *calibs){
  uint8_t buffer[LEN_GROUP_1_CALIB_REGS];

  if(I2C_Master::read_msg(BME688_ADRR, START_GROUP_1_CALIB_REGS, buffer, LEN_GROUP_1_CALIB_REGS) == -1)
    return -1;
  calibs->temp.par_t2   = (int16_t)(buffer[TEMPERATURE_T2_LSB] | (uint16_t)buffer[TEMPERATURE_T2_MSB] << 8);
  calibs->temp.par_t3   = (int8_t)buffer[TEMPERATURE_T3];
  calibs->press.par_p1  = (uint16_t)(buffer[PRESSURE_P1_LSB] | (uint16_t)buffer[PRESSURE_P1_MSB] << 8);
  calibs->press.par_p2  = (int16_t)(buffer[PRESSURE_P2_LSB] | (uint16_t)buffer[PRESSURE_P2_MSB] << 8);
  calibs->press.par_p3  = (int8_t)buffer[PRESSURE_P3];
  calibs->press.par_p4  = (int16_t)(buffer[PRESSURE_P4_LSB] | (uint16_t)buffer[PRESSURE_P4_MSB] << 8);
  calibs->press.par_p5  = (int16_t)(buffer[PRESSURE_P5_LSB] | (uint16_t)buffer[PRESSURE_P5_MSB] << 8);
  calibs->press.par_p6  = (int8_t)buffer[PRESSURE_P6];
  calibs->press.par_p7  = (int8_t)buffer[PRESSURE_P7];
  calibs->press.par_p8  = (int16_t)(buffer[PRESSURE_P8_LSB] | (uint16_t)buffer[PRESSURE_P8_MSB] << 8);
  calibs->press.par_p9  = (int16_t)(buffer[PRESSURE_P9_LSB] | (uint16_t)buffer[PRESSURE_P9_MSB] << 8);
  calibs->press.par_p10 = (uint8_t)buffer[PRESSURE_P10];

  if(I2C_Master::read_msg(BME688_ADRR, START_GROUP_2_CALIB_REGS, buffer, LEN_GROUP_2_CALIB_REGS) == -1)
    return -1;
  calibs->temp.par_t1 = (uint16_t)(buffer[TEMPERATURE_T1_LSB] | (uint16_t)buffer[TEMPERATURE_T1_MSB] << 8);
  calibs->hum.par_h1  = (uint16_t)((0x0F & buffer[HUMIDITY_H1_H2_LSB]) | (uint16_t)buffer[HUMIDITY_H1_MSB] << 4);
  calibs->hum.par_h2  = (uint16_t)((buffer[HUMIDITY_H1_H2_LSB] >> 4) | (uint16_t)buffer[HUMIDITY_H2_MSB] << 4);
  calibs->hum.par_h3  = (int8_t)buffer[HUMIDITY_H3];
  calibs->hum.par_h4  = (int8_t)buffer[HUMIDITY_H4];
  calibs->hum.par_h5  = (int8_t)buffer[HUMIDITY_H5];
  calibs->hum.par_h6  = (uint8_t)buffer[HUMIDITY_H6];
  calibs->hum.par_h7  = (int8_t)buffer[HUMIDITY_H7];
  calibs->gas.par_g1  = (int8_t)buffer[GAS_G1];
  calibs->gas.par_g2  = (int16_t)(buffer[GAS_G2_LSB] | (uint16_t)buffer[GAS_G2_MSB] << 8);
  calibs->gas.par_g3  = (int8_t)buffer[GAS_G3];

  if(I2C_Master::read_msg(BME688_ADRR, START_GROUP_3_CALIB_REGS, buffer, LEN_GROUP_3_CALIB_REGS) == -1)
    return -1;
  calibs->gas.res_heat_val = buffer[RES_HEAT_VAL];
  calibs->gas.res_heat_range = (0x30 & buffer[RES_HEAT_RANGE]) >> 4;

  return 0;
}


/**
  * @brief Set the configuration parameters of the heater of the gas sensor.
  *
  * @param[in] mode The operation mode. Can be SLEEP_OP_MODE, FORCED_OP_MODE or PARALLEL_OP_MODE.
  * @param[in] target_temp The temperature that the hot plate will reach to get the measure.
  * @param[in] amb_temp The ambient temperature.
  * @param[in] ms The time to obtain the desired temperature. Should be greater than 30ms.
  * @param[in] gas_cals A structure with the calibration parameters for the calculation of the compensated gas values.
  *
  * @return 0 if success, -1 if error.
  */
static int set_heat_gas_confs(uint8_t mode, float target_temp, float amb_temp, uint16_t ms, bme688_calib_gas_sensor gas_cals){
  uint8_t buffer[2];

  buffer[0] = RESISTANCE_HEATER_0_REG;
  buffer[1] = calc_res_heat_x(target_temp, amb_temp, gas_cals);


  if(I2C_Master::write_msg(BME688_ADRR, buffer, 2) == -1)
    return -1;

  buffer[0] = GAS_WAIT_0_REG;
  buffer[1] = calc_gas_wait_x(ms);

  if(I2C_Master::write_msg(BME688_ADRR, buffer, 2) == -1)
    return -1;

  return 0;
}



/**
  * @brief Calculate the target resistance of the heater as a value that can process the gas sensor according to the
  * format specified in the datasheet.
  *
  * @param[in] target_temp The temperature that the hot plate will reach to get the measure.
  * @param[in] amb_temp The ambient temperature.
  * @param[in] gas_cals A structure with the calibration parameters for the calculation of the compensated gas values.
  *
  * @return the target resistance of the heater.
  */
static uint8_t calc_res_heat_x(float target_temp, float amb_temp, bme688_calib_gas_sensor gas_cals){
  float var1, var2;
  uint8_t res_heat_x;

  //Perform calibrations according to the operations specified in the datasheet
  var1 = (gas_cals.par_g1 / 16.0 + 49.0) * (1.0 + ((gas_cals.par_g2 / 32768.0 * 0.0005 + 0.00235) *  target_temp));
  var2 = var1 + (gas_cals.par_g3 / 1024.0 * amb_temp);
  res_heat_x = (uint8_t)(3.4 * ((var2 * (4.0 / (4.0 + gas_cals.res_heat_range)) * (1.0/(1.0 + gas_cals.res_heat_val * 0.002))) - 25));

  return res_heat_x;
}


/**
  * @brief Calculate the time as a value that can process the gas sensor according to the format specified in the datasheet.
  *
  * @param[in] ms The time to obtain the desired temperature. Should be greater than 30ms.
  *
  * @return the time.
  */
static uint8_t calc_gas_wait_x(uint16_t ms){
  uint8_t reg, mult_factor = 0;
  if(ms >= MAX_GAS_WAIT_TIME){
    reg = 0xFF; //Maximum value
  }
  else{
    while (ms > 0x3F){
      ms /= 4;
      mult_factor++;
    }
    reg = ms + mult_factor * 0x40;
  }
  return reg;
}


/**
  * @brief Calculate the compensated temperature.
  *
  * @param[in] temp_adc The value of the temperature obtained from the sensor registers.
  * @param[in] temp_offset A temperature offset to be subtracted to the result. This parameter will compensate all the
  *                        offset added by PCB temperature, sensor element self-heating, etc.
  * @param[in] calibs A structure with the calibration parameters for the calculation of the compensated metrics.
  *
  * @return the compensated temperature.
  */
static float calc_compensated_temperature(float temp_adc, float temp_offset, bme688_calib_sensor *calibs){
  float var1, var2, temp_comp;

  //Perform calibrations according to the operations specified in the datasheet
  var1 = ((temp_adc / 16384.0) - (calibs->temp.par_t1 / 1024.0)) * calibs->temp.par_t2;
  var2 = (temp_adc / 131072.0) - (calibs->temp.par_t1 / 8192.0);
  var2 = var2 * var2 * calibs->temp.par_t3 * 16.0;
  calibs->temp.t_fine = var1 + var2 - temp_offset * 5120.0;
  temp_comp = calibs->temp.t_fine / 5120.0;

  return temp_comp;
}


/**
  * @brief Calculate the compensated pressure.
  *
  * @param[in] press_adc The value of the pressure obtained from the sensor registers.
  * @param[in] calibs A structure with the calibration parameters for the calculation of the compensated metrics.
  *
  * @return the compensated pressure.
  */
static float calc_compensated_pressure(float press_adc, bme688_calib_sensor *calibs){
  float var1, var2, var3, var4, press_comp;

  //Perform calibrations according to the operations specified in the datasheet
  var1 = (calibs->temp.t_fine / 2.0) - 64000.0;
  var2 = var1 * var1 * (calibs->press.par_p6 / 131072.0) + (var1 * calibs->press.par_p5 * 2.0);
  var2 = (var2 / 4.0) + (calibs->press.par_p4 * 65536.0);
  var1 = (((calibs->press.par_p3 * var1 * var1) / 16384.0) + (calibs->press.par_p2 * var1)) / 524288.0;
  var1 = (1.0 + (var1 / 32768.0)) * calibs->press.par_p1;
  press_comp = 1048576.0 - press_adc;
  if(var1 != 0){
    press_comp = ((press_comp - (var2 / 4096.0)) * 6250.0) / var1;
    var1 = (calibs->press.par_p9 * press_comp * press_comp) / 2147483648.0;
    var2 = press_comp * (calibs->press.par_p8 / 32768.0);
    var4 = press_comp / 256.0;
    var3 = var4 * var4 * var4 * (calibs->press.par_p10 / 131072.0);
    press_comp = press_comp + (var1 + var2 + var3 + (calibs->press.par_p7 * 128.0)) / 16.0;
  }
  else{
    press_comp = 0;
  }


  return press_comp;
}


/**
  * @brief Calculate the compensated humidity.
  *
  * @param[in] hum_adc The value of the humidity obtained from the sensor registers.
  * @param[in] calibs A structure with the calibration parameters for the calculation of the compensated metrics.
  *
  * @return the compensated humidity.
  */
static float calc_compensated_humidity(float hum_adc, bme688_calib_sensor *calibs){
  float temp_comp, var1, var2, var3, var4, hum_comp;

  //Perform calibrations according to the operations specified in the datasheet
  temp_comp = calibs->temp.t_fine / 5120.0;
  var1 = hum_adc - (calibs->hum.par_h1 * 16.0) + ((calibs->hum.par_h3 / 2.0) * temp_comp);
  var2 = var1 * (
      calibs->hum.par_h2 / 262144.0 * (
          1.0 + (calibs->hum.par_h4 / 16384.0 * temp_comp) + (calibs->hum.par_h5 / 1048576.0 * temp_comp * temp_comp)
          )
      );
  var3 = calibs->hum.par_h6 / 16384.0;
  var4 = calibs->hum.par_h7 / 2097152.0;
  hum_comp = var2 + ((var3 + (var4 * temp_comp)) * var2 * var2);

  if (hum_comp > 100.0){
    hum_comp = 100.0;
  }
  else if (hum_comp < 0.0){
    hum_comp = 0.0;
  }
  return hum_comp;
}


/**
  * @brief Calculate the compensated gas resistance.
  *
  * @param[in] gas_adc The value of the gas resistance obtained from the sensor registers.
  * @param[in] gas_range A calibration parameter to obtain the gas resistance.
  *
  * @return the compensated gas resistance.
  */
static float calc_compensated_gas_resistance(uint16_t gas_adc, uint8_t gas_range){
  uint32_t var1;
  int32_t var2;
  float gas_res;

  //Perform calibrations according to the operations specified in the datasheet
  var1 = ((uint32_t)262144) >> gas_range;
  var2 = (gas_adc - 512)*3 + 4096;
  gas_res = 1000000.0f * var1 / var2;

  return gas_res;
}


