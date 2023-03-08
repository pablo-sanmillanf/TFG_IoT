#include <stdint.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <linux/i2c.h>
#include <signal.h>
#include "LSM6DSOX.h"
#include "../i2c_master/i2c_master.h"
//+-2g


static int finish = 0;

void finisher(int signal);

int main(){
  signal(SIGINT, finisher);

  
  int status = LSM6DSOX::init();
  

  printf("Init status: %d\n", status);
  
  float temp, x_acc, y_acc, z_acc, x_gyr, y_gyr, z_gyr;
  
  while(!finish){
  
    temp = LSM6DSOX::get_temperature();
    printf("Temperature: %.1f\n", temp);
     
    status = LSM6DSOX::get_acc_values(&x_acc, &y_acc, &z_acc);

    printf("Accelerometer:\t X: %.1f\t Y: %.1f\t Z: %.1f\tRead status: %d\n", x_acc, y_acc, z_acc, status);
    
    status = LSM6DSOX::get_gyr_values(&x_gyr, &y_gyr, &z_gyr);

    printf("Gyroscope:\t X: %.1f\t Y: %.1f\t Z: %.1f\tRead status: %d\n\n", x_gyr, y_gyr, z_gyr, status);
  
    sleep(2);
  }
  I2C_Master::end();

  return 0;
}



void finisher(int signal){
  finish = 1;
  printf("\n\n|----------------------------------------|");
  printf("\n|------------PROGRAM FINISHED------------|");
  printf("\n|----------------------------------------|\n");
}
