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

/* Private typedef -----------------------------------------------------------*/
struct bme688_calib_temperature_sensor
{
  uint16_t par_t1;
  int16_t par_t2;
  int8_t par_t3;
};

struct bme688_calib_pressure_sensor
{
  uint16_t par_p1;
  int16_t par_p2;
  int8_t par_p3;
  int16_t par_p4;
  int16_t par_p5;
  int8_t par_p6;
  int8_t par_p7;
  int16_t par_p8;
  int16_t par_p9;
  uint8_t par_p10;
};

struct bme688_calib_humidity_sensor
{
  uint16_t par_h1;
  uint16_t par_h2;
  int8_t par_h3;
  int8_t par_h4;
  int8_t par_h5;
  uint8_t par_h6;
  int8_t par_h7;
};

struct bme688_calib_gas_sensor
{
  int8_t par_g1;
  int16_t par_g2;
  int8_t par_g3;
  uint8_t res_heat_range;
  int8_t res_heat_val;
};

struct bme688_calib_sensor
{
  bme688_calib_temperature_sensor temperature;
  bme688_calib_pressure_sensor pressure;
  bme688_calib_humidity_sensor humidity;
  bme688_calib_gas_sensor gas;
};
  
/* Private variables----------------------------------------------------------*/
int fd;
struct i2c_rdwr_ioctl_data packets;
struct i2c_msg messages[2];

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
I2C_Master::start (int i2c_device) {
  if(fd == 0){
    //Open file descriptor
    char i2cFile[15];
    sprintf(i2cFile, "/dev/i2c-%d", i2c_device);
    
    fd = open(i2cFile, O_RDWR);
    
    if(fd != -1){
      return 0;
    }
  }
  
  return fd;
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