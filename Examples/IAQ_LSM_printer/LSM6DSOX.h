/**
  ******************************************************************************
  * @file   LSM6DSOX.h
  * @author Pablo San Mill�n Fierro (pablo.sanmillanf@alumnos.upm.es)
  * @brief  LSM6DSOX Sensor Handler Module header.
  *
  * @note   End-of-degree work.
  *         This module manages the LSM6DSOX sensor
  ******************************************************************************
*/

#ifndef __LSM6DSOX_H__
#define __LSM6DSOX_H__

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include "i2c_master.h"

#ifdef __cplusplus
extern "C" {
#endif
/* Exported variables --------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/

#define LSM6DSOX_OFF_ODR        0x00
#define LSM6DSOX_12_5_HZ_ODR    0x10
#define LSM6DSOX_26_HZ_ODR      0x20
#define LSM6DSOX_52_HZ_ODR      0x30
#define LSM6DSOX_104_HZ_ODR     0x40
#define LSM6DSOX_208_HZ_ODR     0x50
#define LSM6DSOX_416_HZ_ODR     0x60
#define LSM6DSOX_833_HZ_ODR     0x70
#define LSM6DSOX_1_66_KHZ_ODR   0x80
#define LSM6DSOX_3_33_KHZ_ODR   0x90
#define LSM6DSOX_6_66_KHZ_ODR   0xA0


#define ACC_16_G_FSR  0x04
#define ACC_8_G_FSR   0x0C
#define ACC_4_G_FSR   0x08
#define ACC_2_G_FSR   0x00

#define GYR_2000_DPS_FSR  0x0C
#define GYR_1000_DPS_FSR  0x08
#define GYR_500_DPS_FSR   0x04
#define GYR_250_DPS_FSR   0x00
#define GYR_125_DPS_FSR   0x02
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported Functions --------------------------------------------------------*/

class LSM6DSOX{
    uint8_t fsr_odr_reg_acc, fsr_odr_reg_gyr;
  public:
    //Non complete constructors
    LSM6DSOX () : LSM6DSOX(LSM6DSOX_OFF_ODR, LSM6DSOX_OFF_ODR, ACC_2_G_FSR, GYR_250_DPS_FSR) {};
    LSM6DSOX (uint8_t odr_acc, uint8_t odr_gyr) : LSM6DSOX(odr_acc, odr_gyr, ACC_2_G_FSR, GYR_250_DPS_FSR) {};
    
    /**
     * @brief Class constructor. Starts the LSM6DSOX Sensor with the given configurations.
     * 
     * @param[in] odr_acc Value of ODR for the accelerometer.
     * @param[in] odr_gyr Value of ODR for the gyroscope.
     * @param[in] fsr_acc Value of FSR for the accelerometer.
     * @param[in] fsr_gyr Value of FSR for the gyroscope.
     */
    LSM6DSOX (uint8_t odr_acc, uint8_t odr_gyr, uint8_t fsr_acc, uint8_t fsr_gyr);
    
    /**
     * @brief Class destructor. Free all the associated resources.
     */
    ~LSM6DSOX();
    
    /**
     * @brief Sets the Full Scale Ranges for the gyroscope and the accelerometer according
     *        to the defines listed in the module header.
     * 
     * @param[in] fsr_acc Value of FSR for the accelerometer.
     * @param[in] fsr_gyr Value of FSR for the gyroscope.
     *
     * @return 0 if success, -1 if error.
     */
    int set_fsr(uint8_t fsr_acc, uint8_t fsr_gyr);
    
    /**
     * @brief Sets the Output Data Rate for the gyroscope and the accelerometer according
     *        to the defines listed in the module header.
     * 
     * @param[in] odr_acc Value of ODR for the accelerometer.
     * @param[in] odr_gyr Value of ODR for the gyroscope.
     *
     * @return 0 if success, -1 if error.
     */
    int set_odr(uint8_t odr_acc, uint8_t odr_gyr);
    
    /**
     * @brief Gets the temperature from the sensor.
     *
     * @param[out] temperature The temperature obtained from the sensor.
     *
     * @return 0 if success, -1 if error.
     */
    int get_temperature(float *temperature);
    
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
    int get_acc_values(float *x_value, float *y_value, float *z_value);
    
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
    int get_gyr_values(float *x_value, float *y_value, float *z_value);
};


#ifdef __cplusplus
}
#endif

#endif
 
