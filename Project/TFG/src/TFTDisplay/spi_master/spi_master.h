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
       * @brief Starts the SPI device and configures it to allow SPI communications.
       * 
       * @param[in] spi_device Indicates which SPI device of type spidev0.<number>
       *                       within the /dev/ directory will be used.
       *
       * @return 0 if success, -1 if error.
       */
      int spi_start (int spi_device, int mode, int bits, int speed);

      /**
       * @brief It sends SPI data of length "size" to the other SPI end, at a speed "speed" and with a delay time "delay" after the data is sent.
       *        The response is stored in the rx buffer. To differentiate between normal data and commands, the GPIO17 pin will be enabled or
       *        disabled. This will be indicated by the parameter "cmd".
       *
       * @param[in] messages Pointer to the SPI data to send.
       * @param[in] rx Pointer to the array where the response will be stored.
       * @param[in] cmd If true, the data sent is a command, if false, it is normal data.
       * @param[in] size The size of the data sent.
       * @param[in] speed Speed of the SPI link.
       *
       */
      void send_spi_msg (uint8_t* messages, uint8_t rx[], uint8_t cmd, int size, int delay, int speed);

      /**
       * @brief End SPI communications and free all the related resources.
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
