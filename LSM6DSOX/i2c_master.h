/**
  * @file   i2c_master.h
  * @author Pablo San Millán Fierro (pablo.sanmillanf@alumnos.upm.es)
  * @brief  I2C Handler Module header.
  *
  * @note   End-of-degree work.
  *         This module manages the I2C communications
  ******************************************************************************
*/

#ifndef __I2C_MASTER_H__
#define __I2C_MASTER_H__

  /* Includes ------------------------------------------------------------------*/
    #include <stdint.h>
    #include <stdio.h>
    #include <fcntl.h>
    #include <unistd.h>
    #include <sys/ioctl.h>
    #include <linux/i2c-dev.h>
    #include <linux/i2c.h>

#ifdef __cplusplus
extern "C" {
#endif
  /* Exported variables --------------------------------------------------------*/
  /* Exported types ------------------------------------------------------------*/
  /* Exported constants --------------------------------------------------------*/
  /* Exported macro ------------------------------------------------------------*/
  /* Exported Functions --------------------------------------------------------*/

#ifdef __cplusplus
    namespace I2C_Master{
#endif
      /**
       * @brief Starts the I2C device to allow I2C comunnications.
       * 
       * @param[in] i2c_device Indicates which I2C device of type i2c_<number> 
       *                       within the /dev/ directory will be used.
       *
       * @return 0 if success, -1 if error.
       */
      int start (int i2c_device);
      
      /**
       * @brief Sends I2C data of length `data_length` to the slave with 
       *        address `addr`.
       * 
       * @param[in] addr I2C 7-bits slave address.
       * @param[in] data Pointer to data array to be sent to slave.
       * @param[in] data_length Bytes to be written.
       *
       * @return non negative value if success, -1 if error.
       */
      int write_msg(uint8_t addr, uint8_t data[], uint8_t data_length);
      
      /**
       * @brief Recieve I2C data of length `data_length` starting at 
       *        register `read_reg` from the slave with address `addr`.
       * 
       * @param[in] addr I2C 7-bits slave address.
       * @param[in] addr I2C register to start the reading process.
       * @param[out] data Pointer to the array where the read data will 
                          be stored.
       * @param[in] data_length Bytes to be read.
       *
       * @return non negative value if success, -1 if error.
       */
      int read_msg(uint8_t addr, uint8_t read_reg, uint8_t data[], uint8_t data_length);
      
      /**
       * @brief End I2C communications and free all the related resources.
       * 
       * @return 0 if success, -1 if error.
       */
      int end();

#ifdef __cplusplus 
    }
#endif
    
#ifdef __cplusplus
}
#endif

#endif

