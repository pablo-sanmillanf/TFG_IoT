/**
  ******************************************************************************
  * @file   i2c_master.cpp
  * @author Pablo San Millán Fierro (pablo.sanmillanf@alumnos.upm.es)
  * @brief  I2C Handler Module.
  *
  * @note   End-of-degree work.
  *         This module manages the I2C communications
  ******************************************************************************
*/
/* Includes ------------------------------------------------------------------*/
#include "i2c_master.h" // Module header
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <linux/i2c.h>

/* Private typedef -----------------------------------------------------------*/
/* Private variables----------------------------------------------------------*/
static int fd;
static struct i2c_rdwr_ioctl_data packets;
static struct i2c_msg messages[2];

/* Private function prototypes -----------------------------------------------*/
/* Functions -----------------------------------------------------------------*/

/**
 * @brief Starts the I2C device to allow I2C comunnications.
 * 
 * @param[in] i2c_device Indicates which I2C device of type i2c_<number> 
 *                       within the /dev/ directory will be used.
 *
 * @return 0 if success, -1 if error.
 */
int I2C_Master::start (int i2c_device) {
  if(fd == 0){
    //Open file descriptor
    char i2cFile[15];
    sprintf(i2cFile, "/dev/i2c-%d", i2c_device);
    
    fd = open(i2cFile, O_RDWR);
    
    if(fd == -1){
      return -1;
    }
  }
  
  return 0;
}


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
int I2C_Master::write_msg(uint8_t addr, uint8_t data[], uint8_t data_length){
  //Write configuration registers
  messages[0].addr = addr;
  messages[0].flags = 0;
  messages[0].len = data_length;
  messages[0].buf = data; //Pointer to the data bytes to be written
  //Build packet list
  packets.msgs = messages;
  packets.nmsgs = 1;
  //Send message(s)
  //I2C_RDWR-> write/read i2c
  return ioctl(fd, I2C_RDWR, &packets);  
}



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
int I2C_Master::read_msg(uint8_t addr, uint8_t read_reg, uint8_t data[], uint8_t data_length){
  //Write in th I2C device to point to reading registers
  messages[0].addr = addr;
  messages[0].flags = 0;
  messages[0].len = 1;
  messages[0].buf = &read_reg; //Pointer to the data bytes to be written
  //Read the accelerometer values
  messages[1].addr = addr;
  messages[1].flags = I2C_M_RD;
  messages[1].len = data_length;
  messages[1].buf = data; //Pointer for reading the data
  //Build packet list
  packets.msgs = messages;
  packets.nmsgs = 2;
  //Send message(s)
  //I2C_RDWR-> write/read i2c
  return ioctl(fd, I2C_RDWR, &packets);
}


/**
 * @brief End I2C communications and free all the related resources.
 * 
 * @return 0 if success, -1 if error.
 */
int I2C_Master::end(){
  int status = close(fd);
  if(status != -1){
    fd = 0;
  }
  return status;
}
