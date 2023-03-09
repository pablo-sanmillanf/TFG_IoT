#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include "LSM6DSOX.h"


static int finish = 0;

void finisher(int signal);

int main(){
  int status;
  
  signal(SIGINT, finisher);

  LSM6DSOX sensor(LSM6DSOX_52_HZ_ODR, LSM6DSOX_52_HZ_ODR);
  
  float temp, x_acc, y_acc, z_acc, x_gyr, y_gyr, z_gyr;
  
  while(!finish){
  
    temp = sensor.get_temperature();
    printf("Temperature: %.1f\n", temp);
     
    status = sensor.get_acc_values(&x_acc, &y_acc, &z_acc);

    printf("Accelerometer:\t X: %.1f\t Y: %.1f\t Z: %.1f\tRead status: %d\n", x_acc, y_acc, z_acc, status);
    
    status = sensor.get_gyr_values(&x_gyr, &y_gyr, &z_gyr);

    printf("Gyroscope:\t X: %.1f\t Y: %.1f\t Z: %.1f\tRead status: %d\n\n", x_gyr, y_gyr, z_gyr, status);
  
    sleep(2);
  }

  return 0;
}



void finisher(int signal){
  finish = 1;
  printf("\n\n|----------------------------------------|");
  printf("\n|------------PROGRAM FINISHED------------|");
  printf("\n|----------------------------------------|\n");
}
