/**
  ******************************************************************************
  * @file   LSM6DSOX.h
  * @author Pablo San Millán Fierro (pablo.sanmillanf@alumnos.upm.es)
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

#ifdef __cplusplus
extern "C" {
#endif
  /* Exported variables --------------------------------------------------------*/
  /* Exported types ------------------------------------------------------------*/
  /* Exported constants --------------------------------------------------------*/
  /* Exported macro ------------------------------------------------------------*/
  /* Exported Functions --------------------------------------------------------*/

#ifdef __cplusplus
    namespace LSM6DSOX{
#endif
      /**
       * @brief Starts the LSM6DSOX Sensor with the given configurations.
       * 
       *
       * @return 0 if success, -1 if error.
       */
      int init ();
      
      /**
       * @brief Gets the temperature from the sensor or a 0 if the communications fail.
       * 
       *
       * @return The temperature as a float or 0 if error.
       */
      float get_temperature();
      
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

#ifdef __cplusplus 
    }
#endif
    
#ifdef __cplusplus
}
#endif

#endif
 
