#include "BME688.h"

int main(){
  BME688 sensor(25);
  float temperature, pressure, humidity, gas_resistance;
  int status;

  printf("Init sensor status: %d\n", sensor.init());
  printf("Set oversamplings status: %d\n", sensor.set_oversamplings(OVSP_1_X, OVSP_4_X, OVSP_16_X));
  printf("Set heater configurations status: %d\n", sensor.set_heater_configurations(true, 300.0, 100));

  for(;;){
    status = sensor.get_data_one_measure(&temperature, &pressure, &humidity, &gas_resistance);
    printf("%.2f ºC, %.2f Pa, %.2f %%, %.2f Ohm, Status: %d\n", temperature, pressure, humidity, gas_resistance, status);
  }
  return 0;
}

