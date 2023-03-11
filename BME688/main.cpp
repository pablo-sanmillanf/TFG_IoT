#include "BME688.h"
#include <stdint.h>
#include <stdio.h>

int main(void)
{
  float temperature, pressure, humidity, gas_resistance;
  uint16_t sample_count = 1;
  int status;
  BME688 sensor;

  printf("Sample, Temperature(deg C), Pressure(Pa), Humidity(%%), Gas resistance(ohm)\n");

  while (sample_count <= 300)
  {
    
    status = sensor.get_one_measure(&temperature, &pressure, &humidity, &gas_resistance);
    printf("%u, %.2f, %.2f, %.2f, %.2f, %d\n", sample_count, temperature, pressure, humidity, gas_resistance, status);
    sample_count++;
  }

  return 0;
}
 
