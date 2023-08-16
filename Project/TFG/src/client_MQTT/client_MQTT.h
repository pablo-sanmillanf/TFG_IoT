/**
  ******************************************************************************
  * @file   client_MQTT.h
  * @author Pablo San Millán Fierro (pablo.sanmillanf@alumnos.upm.es)
  * @brief  MQTT communications handler Module Header.
  *
  * @note   End-of-degree work.
  *         This module handles the MQTT communications with the MQTT broker.
  ******************************************************************************
*/

#ifndef __CLIENT_MQTT_H__
#define __CLIENT_MQTT_H__

/* Includes ------------------------------------------------------------------*/

#include <string>
#include <atomic>
#include <functional>

namespace MQTT{

/* Exported types ------------------------------------------------------------*/
/* Exported variables --------------------------------------------------------*/
extern std::atomic_bool send_temp;
extern std::atomic_bool send_press;
extern std::atomic_bool send_hum;
extern std::atomic_bool send_IAQ;
extern std::atomic_bool send_alt;
extern std::atomic_bool temp_on;
extern std::atomic_bool press_on;
extern std::atomic_bool hum_on;
extern std::atomic_bool IAQ_on;
extern std::atomic_bool alt_on;

extern std::atomic_bool continuous_send;
extern std::atomic_uint8_t send_rate;

/* Exported constants --------------------------------------------------------*/

//These strings will be the ones that the broker will have to have to edit the associated parameter
const std::string send_temp_mqtt_string = "send_temperature";
const std::string send_press_mqtt_string = "send_pressure";
const std::string send_hum_mqtt_string = "send_humidity";
const std::string send_IAQ_mqtt_string = "send_IAQ";
const std::string send_alt_mqtt_string = "send_altitude";
const std::string temp_on_mqtt_string = "temperature_on";
const std::string press_on_mqtt_string = "pressure_on";
const std::string hum_on_mqtt_string = "humidity_on";
const std::string IAQ_on_mqtt_string = "IAQ_on";
const std::string alt_on_mqtt_string = "altitude_on";

const std::string continuous_send_mqtt_string = "continuous_send";
const std::string send_rate_mqtt_string = "send_rate";

/* Exported macro ------------------------------------------------------------*/
/* Exported Functions --------------------------------------------------------*/

/**
 * @brief The main function of the MQTT communications handler thread.
 *
 * @param[in] host The IP address or domain of the server.
 * @param[in] access_token The access token that represents the device in the server.
 * @param[in] telemetry_topic The MQTT topic where the client will publish the info gathered from the sensors.
 * @param[in] attributes_topic The MQTT topic that points to attributes info.
 * @param[in] on_off_callback The function callback that will execute when an on_off variable will change.
 *
 * @return 1 if a on_off variable has changed its state, 0 if not and -1 if error.
 */
void client_mqtt_thread(std::string host, std::string access_token,
    std::string telemetry_topic, std::string attributes_topic, std::function<void()> on_off_callback);


/**
 * @brief This function is used to finish the MQTT communications thread correctly. The termination
 *        is not instantaneous and after this function the join() function of the thread in question
 *        must be called.
 *
 */
void client_mqtt_thread_finisher();

}

#endif /* __CLIENT_MQTT_H__ */
