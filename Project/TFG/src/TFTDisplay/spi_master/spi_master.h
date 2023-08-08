/**
  ******************************************************************************
  * @file   spi_master.h
  * @author Alejandro Hontanilla Belinchón (a.hontanillab@alumnos.upm.es)
  * @brief  SPI Handler Module header.
  *
  * @note   End-of-degree work.
  *         This module manages the SPI communications
  ******************************************************************************
*/

#ifndef __SPI_ADAPTER_H__
#define __SPI_ADAPTER_H__

  /* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <getopt.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/ioctl.h>
#include <sys/stat.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>
#include <errno.h>

#ifdef __cplusplus
extern "C" {
#endif
  /* Exported variables --------------------------------------------------------*/
  /* Exported types ------------------------------------------------------------*/
  /* Exported constants --------------------------------------------------------*/
  /* Exported macro ------------------------------------------------------------*/
  /* Exported Functions --------------------------------------------------------*/

#ifdef __cplusplus
    namespace SPI_Master{
#endif
      /**
       * @brief Starts the I2C device to allow I2C comunnications.
       * 
       * @param[in] i2c_device Indicates which I2C device of type i2c_<number> 
       *                       within the /dev/ directory will be used.
       *
       * @return 0 if success, -1 if error.
       */
      int spi_start (int spi_device, int mode, int bits, int speed);

      /**
       * @brief Starts the I2C device to allow I2C comunnications.
       *
       * @param[in] i2c_device Indicates which I2C device of type i2c_<number>
       *                       within the /dev/ directory will be used.
       *
       * @return 0 if success, -1 if error.
       */
      void send_spi_msg (uint8_t* messages, uint8_t rx[], uint8_t cmd, int size, int delay, int speed);
            
      /**
       * @brief Starts the I2C device to allow I2C comunnications.
       *
       * @param[in] i2c_device Indicates which I2C device of type i2c_<number>
       *                       within the /dev/ directory will be used.
       *
       * @return 0 if success, -1 if error.
       */
      void send_spi_data (uint8_t* messages, uint8_t rx[], uint8_t cmd, int size, int delay, int speed);

      /**
       * @brief End I2C communications and free all the related resources.
       * 
       * @return 0 if success, -1 if error.
       */
      int spi_end();

#ifdef __cplusplus 
    }
#endif
    
#ifdef __cplusplus
}
#endif

#endif
