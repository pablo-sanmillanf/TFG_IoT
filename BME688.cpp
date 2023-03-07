/**
  ******************************************************************************
  * @file   bme688.cpp
  * @author Pablo San Millán Fierro (pablo.sanmillanf@alumnos.upm.es)
  * @brief  BME688 Gas Sensor Handler Module.
  *
  * @note   End-of-degree work.
  *         This module manages the BME688 Gas Sensor.
  *         Base on: https://github.com/BoschSensortec/BME68x-Sensor-API
  ******************************************************************************
*/
/* Includes ------------------------------------------------------------------*/
#include "bme688.h" // Module header
#include "i2c_master.h"

/* Private typedef -----------------------------------------------------------*/
#define BME688_ADRR       0x76

#define RESET_REG                         0xE0
#define TEMP_PRESS_OVSMP_AND_MODE_REG     0x74
#define HUM_OVSMP_REG                     0x72
#define GAS_RUN_AND_HEATER_SELECTION_REG  0x71
#define GAS_MEASURE_WAIT_0_REG            0x64
#define GAS_HEATER_RESISTANCE_REG         0x5A

#define RESET_VALUE 0xB6

#define FIRST_GROUP_CALIB_REGS    0x00
#define SECOND_GROUP_CALIB_REGS   0x02
#define THIRD_GROUP_CALIB_REGS    0x8A
#define FOURTH_GROUP_CALIB_REGS   0xE1

#define THIRD_GROUP_CALIB_REGS_LEN  23
#define FOURTH_GROUP_CALIB_REGS_LEN 14

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

#define GAS_G1        12
#define GAS_G2_LSB    10
#define GAS_G2_MSB    11
#define GAS_G3        13

#define TEMP_RESULT_0_REG 0x22
#define TEMP_RESULT_1_REG 0x33
#define TEMP_RESULT_2_REG 0x44

#define PRESS_RESULT_0_REG 0x1F
#define PRESS_RESULT_1_REG 0x30
#define PRESS_RESULT_2_REG 0x41

#define HUM_RESULT_0_REG 0x25
#define HUM_RESULT_1_REG 0x36
#define HUM_RESULT_2_REG 0x47

#define TEMP_PRESS_LEN_DATA 3
#define HUM_GAS_LEN_DATA    2
#define GAS_RANGE_LEN_DATA  1

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
/* Private variables----------------------------------------------------------*/
static bme688_oversamplings ovsp;
/* Private function prototypes -----------------------------------------------*/
static int obtain_calibrations(bme688_calib_sensor *calibs);
static uint8_t calc_res_heat_x(float target_temp, float amb_temp, bme688_calib_sensor *calibs);
static float calc_compensated_temperature(float temp_adc, bme688_calib_sensor *calibs);
static float calc_compensated_pressure(float press_adc, bme688_calib_sensor *calibs);
static float calc_compensated_humidity(float hum_adc, bme688_calib_sensor *calibs);
static float calc_compensated_gas_resistance(uint16_t gas_adc, uint8_t gas_range);
static uint8_t translate_ovsp_value(uint8_t oversampling);
/* Functions -----------------------------------------------------------------*/

/**
 * @brief Sets the temperature, pressure and humidity oversampling. A0 will disable that sensor.
 *        Allowed values are 0, 1, 2, 4, 8 and 16. Different values are truncated to the next 
 *        lower value.
 * 
 * @param[in] temperature The temperature oversampling
 * @param[in] pressure The pressure oversampling
 * @param[in] humidity The humidity oversampling
 *
 * @return The translated oversampling.
 */
void BME688::set_oversamplings(uint8_t temperature, uint8_t pressure, uint8_t humidity){
  ovsp.temp = translate_ovsp_value(temperature);
  ovsp.press = translate_ovsp_value(pressure);
  ovsp.hum = translate_ovsp_value(humidity);
}

void BME688::init(uint8_t gas_run){
  uint8_t buffer[2];
  
  buffer[0] = RESET_REG;
  buffer[1] = RESET_VALUE;
  I2C_Master::write_msg(BME688_ADRR, buffer, 2);
  
  buffer[0] = HUM_OVSMP_REG;
  buffer[1] = ovsp.hum;
  I2C_Master::write_msg(BME688_ADRR, buffer, 2);
  
  if(gas_run){
    
  }
  
  buffer[0] = TEMP_PRESS_OVSMP_AND_MODE_REG;
  buffer[1] = ovsp.temp << 5 & ovsp.press << 2 & 0x01;
  I2C_Master::write_msg(BME688_ADRR, buffer, 2);
}


int BME688::get_temperature(){
  uint8_t buffer[TEMP_PRESS_LEN_DATA];
  I2C_Master::read_msg(BME688_ADRR, TEMP_RESULT_0_REG, buffer, TEMP_PRESS_LEN_DATA);
  int raw_temp = (buffer[0] << 12) & (buffer[1] << 4) & (buffer[2] >> 4);
  return raw_temp;
}

int BME688::get_pressure(){
  uint8_t buffer[TEMP_PRESS_LEN_DATA];
  I2C_Master::read_msg(BME688_ADRR, PRESS_RESULT_0_REG, buffer, TEMP_PRESS_LEN_DATA);
  int raw_press = (buffer[0] << 12) & (buffer[1] << 4) & (buffer[2] >> 4);
  return raw_press;
}

int BME688::get_humidity(){
  uint8_t buffer[HUM_GAS_LEN_DATA];
  I2C_Master::read_msg(BME688_ADRR, HUM_RESULT_0_REG, buffer, HUM_GAS_LEN_DATA);
  int raw_hum = (buffer[0] << 8) & buffer[1];
  return raw_hum;
}


/* Private functions ---------------------------------------------------------*/
static int obtain_calibrations(bme688_calib_sensor *calibs){
  uint8_t buffer[THIRD_GROUP_CALIB_REGS_LEN]; //Length of the largest group.
  
  //First group of calibration registers
  if(I2C_Master::read_msg(BME688_ADRR, FIRST_GROUP_CALIB_REGS, buffer, 1) != -1){
    calibs->gas.res_heat_val = buffer[0];
    
    //Second group of calibration registers
    if(I2C_Master::read_msg(BME688_ADRR, SECOND_GROUP_CALIB_REGS, buffer, 1) != -1){
      calibs->gas.res_heat_range = (0x30 & buffer[0]) >> 4;
      
      //Third group of calibration registers
      if(I2C_Master::read_msg(BME688_ADRR, THIRD_GROUP_CALIB_REGS, buffer, THIRD_GROUP_CALIB_REGS_LEN) != -1){
        
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
        
        //Fourth group of calibration registers
        if(I2C_Master::read_msg(BME688_ADRR, FOURTH_GROUP_CALIB_REGS, buffer, FOURTH_GROUP_CALIB_REGS_LEN) != -1){
          
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
          
          return 0;
        }
      }
    }
  }
  return -1;
}



static float calc_compensated_temperature(float temp_adc, bme688_calib_sensor *calibs){
  float var1, var2, temp_comp;
  
  //Perform calibrations according to the operatios specified in the datasheet
  var1 = ((temp_adc / 16384.0) - (calibs->temp.par_t1 / 1024.0)) * calibs->temp.par_t2;
  var2 = (temp_adc / 131072.0) - (calibs->temp.par_t1 / 8192.0);
  var2 = var * var * calibs->temp.par_t3 * 16.0;
  calibs->temp.t_fine = var1 + var2;
  temp_comp = calibs->temp.t_fine / 5120.0;
  
  return temp_comp;
}


static float calc_compensated_pressure(float press_adc, bme688_calib_sensor *calibs){
  float var1, var2, var3, var4, press_comp;
  
  //Perform calibrations according to the operatios specified in the datasheet
  var1 = (calibs->temp.t_fine / 2.0) - 64000.0;
  var2 = var1 * var1 * (calibs->press.par_p6 / 131072.0) + (var1 * calibs->press.par_p5 * 2.0);
  var2 = (var2 / 4.0) + (calibs->press.par_p4 * 65536.0);
  var1 = (((calibs->press.par_p3 * var1 * var1) / 16384.0) + (calibs->press.par_p2 * var1)) / 524288.0;
  var1 = (1.0 + (var1 / 32768.0)) * calibs->press.par_p1;
  press_comp = 1048576.0 - press_adc;
  press_comp = ((press_comp - (var2 / 4096.0)) * 6250.0) / var1;
  var1 = (calibs->press.par_p9 * press_comp * press_comp) / 2147483648.0;
  var2 = press_comp * (calibs->press.par_p8 / 32768.0);
  var4 = press_comp / 256.0;
  var3 = var4 * var4 * var4 * (par_p10 / 131072.0);
  press_comp = press_comp + (var1 + var2 + var3 + (calibs->press.par_p7 * 128.0)) / 16.0;
  
  return press_comp;
}


static float calc_compensated_humidity(float hum_adc, bme688_calib_sensor *calibs){
  float temp_scaled, var1, var2, var3, var4, hum_comp;
  
  //Perform calibrations according to the operatios specified in the datasheet
  temp_comp = calibs->temp.t_fine / 5120.0;
  var1 = hum_adc - (calibs->hum.par_h1 * 16.0) + ((calibs->hum.par_h3 / 2.0) * temp_comp);
  var2 = var1 * ((calibs->hum.par_h2 / 262144.0) * (1.0 + (calibs->hum.par_h4 / 16384.0) * 
  temp_comp) + (calibs->hum.par_h5 / 1048576.0 * temp_comp * temp_comp));
  var3 = calibs->hum.par_h6 / 16384.0;
  var4 = calibs->hum.par_h7 / 2097152.0;
  hum_comp = var2 + ((var3 + (var4 * temp_comp)) * var2 * var2);
  
  return hum_comp;
}


static uint8_t calc_res_heat_x(float target_temp, float amb_temp, bme688_calib_sensor *calibs){
  float var1, var2, var3, var4, var5;
  uint8_t res_heat_x;
  
  //Perform calibrations according to the operatios specified un the datasheet
  var1 = (calibs->gas.par_g1 / 16.0) + 49.0;
  var2 = ((calibs->gas.par_g2 / 32768.0) * 0.0005) + 0.00235;
  var3 = calibs->gas.par_g3 / 1024.0;
  var4 = var1 * (1.0 + (var2 *  target_temp));
  var5 = var4 + (var3 * amb_temp);
  res_heat_x = (uint8_t)(3.4 * ((var5 * (4.0 / (4.0 + calibs->gas.res_heat_range)) * (1.0/(1.0 + calibs->gas.res_heat_val * 0.002))) - 25));
  
  return res_heat_x;
}


static float calc_compensated_gas_resistance(uint16_t gas_adc, uint8_t gas_range){
  uint32_t var1;
  int32_t var2;
  float gas_res;
  
  //Perform calibrations according to the operatios specified un the datasheet
  var1 = ((uint32_t)262144) >> gas_range;
  var2 = (gas_adc - 512)*3 + 4096;
  gas_res = 1000000.0f * var1 / var2;
  
  return gas_res;
}

/**
 * @brief Translates the oversampling value to the value to be stored in the I2C registers. 
 *        Allowed values are 0, 1, 2, 4, 8 and 16. Different values are truncated to the next 
 *        lower value.
 * 
 * @param[in] oversampling The oversampling
 *
 * @return The translated oversampling.
 */
static uint8_t translate_ovsp_value(uint8_t oversampling){
  if(oversampling == 0){
    return 0x00;
  }
  else if(oversampling < 2){
    return 0x01;
  }
  else if(oversampling < 4){
    return 0x02;
  }
  else if(oversampling < 8){
    return 0x03;
  }
  else if(oversampling < 16){
    return 0x04;
  }
  return 0x07;
}