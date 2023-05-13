/**
  ******************************************************************************
  * @file   IAQTracker.cpp
  * @author Pablo San Millán Fierro (pablo.sanmillanf@alumnos.upm.es)
  * @brief  IAQ Calculator Module.
  *
  * @note   End-of-degree work.
  *         This module manages the calculation of the IAQ based on the data
  *         from the BME688 gas sensor.
  ******************************************************************************
*/
/* Includes ------------------------------------------------------------------*/
#include "IAQTracker.h" // Module header
#include <cmath>

/* Private defines -----------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private variables----------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
static float get_svd(float temp);

/* Functions -----------------------------------------------------------------*/

/**
  * @brief Starts the module with the relevant parameters to calculate the IAQ.
  *
  * @param[in] hum_corr_factor The humidity correction factor. Is the ratio between the value in logarithmic units
  *                            of the gas resistance and the absolute humidity value.
  * @param[in] burn_in_cycles The number of calls to the get_IAQ method for a measurement to be considered good. This
  *                           is done because in the first instants of operation, the sensor returns erroneous data.
  * @param[in] calib_samples The number of samples that will have the calibration array.
  * @param[in] gas_refresh_period The number of calls to the get_IAQ method after which the mean is updated with new
  *                               calibration data, regardless of whether or not the new data is greater than that
  *                               stored in the calibration array.
  *
  */
void IAQTracker::init(float hum_corr_factor, int burn_in_cycles, int calib_samples, int gas_refresh_period){
  this->hum_corr_factor = hum_corr_factor;
  this->burn_in_cycles = burn_in_cycles;
  this->gas_refresh_period = gas_refresh_period;

  gas_current_period = 0;
  gas_ceil = 0;
  calib_gas_data_index = 1;

  calib_gas_data = new float[calib_samples];
  calib_gas_data_size = calib_samples;
  calib_data_completed = false;
}


/**
  * @brief Class constructor. Starts the module with the relevant parameters to calculate the IAQ.
  *
  */
IAQTracker::IAQTracker(){
  this->init(0.827, 300, 100, 3600);
}


/**
  * @brief Class constructor. Starts the module with the relevant parameters to calculate the IAQ.
  *
  * @param[in] hum_corr_factor The humidity correction factor. Is the ratio between the value in logarithmic units
  *                            of the gas resistance and the absolute humidity value.
  *
  */
IAQTracker::IAQTracker(float hum_corr_factor){
  this->init(hum_corr_factor, 300, 100, 3600);
}


/**
  * @brief Class constructor. Starts the module with the relevant parameters to calculate the IAQ.
  *
  * @param[in] hum_corr_factor The humidity correction factor. Is the ratio between the value in logarithmic units
  *                            of the gas resistance and the absolute humidity value.
  * @param[in] burn_in_cycles The number of calls to the get_IAQ method for a measurement to be considered good. This
  *                           is done because in the first instants of operation, the sensor returns erroneous data.
  *
  */
IAQTracker::IAQTracker(float hum_corr_factor, int burn_in_cycles){
  this->init(hum_corr_factor, burn_in_cycles, 100, 3600);
}


/**
  * @brief Class constructor. Starts the module with the relevant parameters to calculate the IAQ.
  *
  * @param[in] hum_corr_factor The humidity correction factor. Is the ratio between the value in logarithmic units
  *                            of the gas resistance and the absolute humidity value.
  * @param[in] burn_in_cycles The number of calls to the get_IAQ method for a measurement to be considered good. This
  *                           is done because in the first instants of operation, the sensor returns erroneous data.
  * @param[in] gas_refresh_period The number of calls to the get_IAQ method after which the mean is updated with new
  *                               calibration data, regardless of whether or not the new data is greater than that
  *                               stored in the calibration array.
  *
  */
IAQTracker::IAQTracker(float hum_corr_factor, int burn_in_cycles, int gas_refresh_period){
  this->init(hum_corr_factor, burn_in_cycles, 100, gas_refresh_period);
}


/**
  * @brief Class constructor. Starts the module with the relevant parameters to calculate the IAQ.
  *
  * @param[in] hum_corr_factor The humidity correction factor. Is the ratio between the value in logarithmic units
  *                            of the gas resistance and the absolute humidity value.
  * @param[in] burn_in_cycles The number of calls to the get_IAQ method for a measurement to be considered good. This
  *                           is done because in the first instants of operation, the sensor returns erroneous data.
  * @param[in] calib_samples The number of samples that will have the calibration array.
  * @param[in] gas_refresh_period The number of calls to the get_IAQ method after which the mean is updated with new
  *                               calibration data, regardless of whether or not the new data is greater than that
  *                               stored in the calibration array.
  *
  */
IAQTracker::IAQTracker(float hum_corr_factor, int burn_in_cycles, int calib_samples, int gas_refresh_period){
  this->init(hum_corr_factor, burn_in_cycles, calib_samples, gas_refresh_period);
}


/**
  * @brief Obtains the IAQ using the calibration data, the relative humidity, the temperature and the gas resistance.
  *
  * @param[out] iaq The resulting IAQ. If the returning values is -1, the value remains unchanged.
  * @param[in] temp Temperature obtained from the BME sensor.
  * @param[in] hum Relative humidity obtained from the BME sensor.
  * @param[in] gas_res Gas resistance obtained from the BME sensor.
  *
  * @return 0 if a valid IAQ has been obtained, -1 if not.
  */
int IAQTracker::get_IAQ(float *iaq, float temp, float hum, float gas_res){
  float comp_gas = gas_res * expf(get_svd(temp) * hum_corr_factor);

  if(burn_in_cycles > 0){
    burn_in_cycles--;
    if(burn_in_cycles == 0){
      calib_gas_data[0] = comp_gas;
      gas_ceil = comp_gas;
    }
    return -1;
  }
  else{
    bool refresh = ++gas_current_period >= gas_refresh_period;
    if(refresh || comp_gas > gas_ceil){
      if(refresh){
        gas_current_period = 0;
      }

      calib_gas_data[calib_gas_data_index++] = comp_gas;

      if(calib_gas_data_index == calib_gas_data_size){
        calib_gas_data_index = 0;
        calib_data_completed = true;
      }

      //Calculate the mean of the highest values
      gas_ceil = 0;
      if(calib_data_completed){
        for(int i = 0; i < calib_gas_data_size; i++){
          gas_ceil += calib_gas_data[i];
        }
        gas_ceil /= calib_gas_data_size;
      }
      else{
        for(int i = 0; i < calib_gas_data_index; i++){
          gas_ceil += calib_gas_data[i];
        }
        gas_ceil /= calib_gas_data_index;
      }

    }


    *iaq = (1 - pow(comp_gas / gas_ceil, 2)) * 500;
    if(*iaq < 0)
      *iaq = 0;
  }

  return 0;
}


/* Private functions ---------------------------------------------------------*/
/**
  * @brief Calculates the Saturation Vapor Density with the Magnus formula and the ideal gas law based on the
  * current temperature. This will be used to obtain the absolute humidity.
  *
  * @param[in] temp Temperature obtained from the BME sensor.
  *
  * @return Saturation Vapor Density.
  */
static float get_svd(float temp){
  return 1.3237 * expf((17.625 * temp) / (temp + 243.04))/(temp + 273.15);
}
