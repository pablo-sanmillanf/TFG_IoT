#include <stdint.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <linux/i2c.h>
#include <signal.h>
#include "i2c_master.h" // Module header
//+-2g
#define ACC_SCALE_RANGE 2
#define GYR_SCALE_RANGE 250
#define ADR_LSM  0x6A

static int finish = 0;

float get_temperature(int16_t value);
float convert_acc(int16_t value, int scale_range);
float convert_gyr(int16_t value, int scale_range);
void finisher(int signal);

int main(){
  signal(SIGINT, finisher);


  
  I2C_Master::start(1);
  
  int w_len = 3;
  int r_len = 6;
  uint8_t data[w_len];
  uint8_t read_bytes[r_len];

  //Reset
  data[0] = 0x12;
  data[1] = 0xA3;  
  int status =I2C_Master::write_msg(ADR_LSM, data,  2);
  

  printf("Reset status: %d\n", status);
  
  data[0] = 0x10;
  data[1] = 0x30;
  data[2] = 0x30;
  status =I2C_Master::write_msg(ADR_LSM, data,  3);
  
  while(!finish){
    status =I2C_Master::read_msg(ADR_LSM, 0x20, read_bytes, 2);
  
    float temp = get_temperature(read_bytes[1] << 8 | read_bytes[0]);
    printf("Temperature: %.1f\tRead status: %d\n", temp, status);
     
    status =I2C_Master::read_msg(ADR_LSM, 0x28, read_bytes, r_len);
  
    float x_acc = convert_acc(read_bytes[1] << 8 | read_bytes[0], ACC_SCALE_RANGE);
    float y_acc = convert_acc(read_bytes[3] << 8 | read_bytes[2], ACC_SCALE_RANGE);
    float z_acc = convert_acc(read_bytes[5] << 8 | read_bytes[4], ACC_SCALE_RANGE);

    printf("Accelerometer:\t X: %.1f\t Y: %.1f\t Z: %.1f\tRead status: %d\n", x_acc, y_acc, z_acc, status);
    
    float x_gyr = convert_gyr(read_bytes[1] << 8 | read_bytes[0], GYR_SCALE_RANGE);
    float y_gyr = convert_gyr(read_bytes[3] << 8 | read_bytes[2], GYR_SCALE_RANGE);
    float z_gyr = convert_gyr(read_bytes[5] << 8 | read_bytes[4], GYR_SCALE_RANGE);

    printf("Gyroscope:\t X: %.1f\t Y: %.1f\t Z: %.1f\tRead status: %d\n\n", x_gyr, y_gyr, z_gyr, status);
  
    sleep(2);
  }
  I2C_Master::end();

  return 0;
}

float get_temperature(int16_t value){
  return (float)value / 256.0 + 25.0;
}

float convert_acc(int16_t value, int scale_range){
  float result;
  result = (float)value / 65536.0 * scale_range * 2;

  return result;
}


float convert_gyr(int16_t value, int scale_range){
  float result;
  result = (float)value / 65536.0 * scale_range * 2;

  return result;
}

void finisher(int signal){
  finish = 1;
  printf("\n\n|----------------------------------------|");
  printf("\n|------------PROGRAM FINISHED------------|");
  printf("\n|----------------------------------------|\n");
}
