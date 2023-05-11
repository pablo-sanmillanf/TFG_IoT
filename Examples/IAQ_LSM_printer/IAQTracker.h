/**
  ******************************************************************************
  * @file   IAQTracker.h
  * @author Pablo San Millán Fierro (pablo.sanmillanf@alumnos.upm.es)
  * @brief  IAQ Calculator Module Header.
  *
  * @note   End-of-degree work.
  *         This module manages the calculation of the IAQ based on the data
  *         from the BME688 gas sensor.
  ******************************************************************************
*/

#ifndef __IAQTRACKER_H__
#define __IAQTRACKER_H__

/* Includes ------------------------------------------------------------------*/

#ifdef __cplusplus
extern "C" {
#endif
/* Exported variables --------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported Functions --------------------------------------------------------*/

class IAQTracker{
  float hum_corr_factor;
  int burn_in_cycles;

  float gas_ceil;
  int gas_refresh_period;
  int gas_current_period;

  float *calib_gas_data;
  int calib_gas_data_size;
  int calib_gas_data_index;
  bool calib_data_completed;

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
  void init(float hum_corr_factor, int burn_in_cycles, int calib_samples, int gas_refresh_period);
public:

  /**
    * @brief Class constructor. Starts the module with the relevant parameters to calculate the IAQ.
    *
    */
  IAQTracker();

  /**
    * @brief Class constructor. Starts the module with the relevant parameters to calculate the IAQ.
    *
    * @param[in] hum_corr_factor The humidity correction factor. Is the ratio between the value in logarithmic units
    *                            of the gas resistance and the absolute humidity value.
    *
    */
  IAQTracker(float hum_corr_factor);

  /**
    * @brief Class constructor. Starts the module with the relevant parameters to calculate the IAQ.
    *
    * @param[in] hum_corr_factor The humidity correction factor. Is the ratio between the value in logarithmic units
    *                            of the gas resistance and the absolute humidity value.
    * @param[in] burn_in_cycles The number of calls to the get_IAQ method for a measurement to be considered good. This
    *                           is done because in the first instants of operation, the sensor returns erroneous data.
    *
    */
  IAQTracker(float hum_corr_factor, int burn_in_cycles);

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
  IAQTracker(float hum_corr_factor, int burn_in_cycles, int gas_refresh_period);

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
  IAQTracker(float hum_corr_factor, int burn_in_cycles, int calib_samples, int gas_refresh_period);

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
  int get_IAQ(float *iaq, float temp, float hum, float gas_res);

};

#ifdef __cplusplus
}
#endif

#endif /* __IAQTRACKER_H__ */
