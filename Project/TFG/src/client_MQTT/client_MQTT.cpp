/**
  ******************************************************************************
  * @file   client_MQTT.cpp
  * @author Pablo San Millán Fierro (pablo.sanmillanf@alumnos.upm.es)
  * @brief  MQTT communications handler Module.
  *
  * @note   End-of-degree work.
  *         This module handles the MQTT communications with the MQTT broker.
  ******************************************************************************
*/


/* Includes ------------------------------------------------------------------*/
#include "client_MQTT.h" // Module header
#include "../measures/measures.h"

#include <unistd.h>
#include <json/json.h>
#include <mqtt/client.h>
#include <mqtt/async_client.h>
#include <mutex>
#include <condition_variable>

/* External variables---------------------------------------------------------*/
std::atomic_bool MQTT::send_temp = true;
std::atomic_bool MQTT::send_press = true;
std::atomic_bool MQTT::send_hum = true;
std::atomic_bool MQTT::send_IAQ = true;
std::atomic_bool MQTT::send_alt = true;

std::atomic_bool MQTT::send = true;
std::atomic_uint8_t MQTT::send_rate = 10;
/* Private defines -----------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private variables----------------------------------------------------------*/
static std::mutex mutex;
static std::condition_variable cond;
static std::atomic_bool run = true;
static int regular_wait_time = 200;
/* Private function prototypes -----------------------------------------------*/
static void wait(mqtt::token_ptr token);
static int read_attributes_state(std::string host, std::string access_token, std::string attributes_topic);
static void check_attributes(Json::Value root);
/* Functions -----------------------------------------------------------------*/

/**
 * @brief The main function of the MQTT communications handler thread.
 *
 * @param[in] host The IP address or domain of the server.
 * @param[in] access_token The access token that represents the device in the server.
 * @param[in] telemetry_topic The MQTT topic where the client will publish the info gathered from the sensors.
 * @param[in] attributes_topic The MQTT topic that points to attributes info.
 *
 * @return 1 if a on_off variable has changed its state, 0 if not and -1 if error.
 */
void MQTT::client_mqtt_thread(std::string host, std::string access_token,
    std::string telemetry_topic, std::string attributes_topic){

  run = true;

  int result = read_attributes_state(host, access_token, attributes_topic);
  if(result != -1){

    //Sets JSON floats precision
    Json::StreamWriterBuilder builder;
    builder.settings_["precision"] = 2;
    builder.settings_["indentation"] = "";
    builder.settings_["precisionType"] = "decimal";
    std::unique_ptr<Json::StreamWriter> writer(builder.newStreamWriter());

    mqtt::async_client cli(host, "request");

    mqtt::connect_options connOpts;
    connOpts.set_user_name(access_token);
    connOpts.set_keep_alive_interval(20);
    connOpts.set_clean_session(true);

    cli.set_message_callback([&](mqtt::const_message_ptr msg) {
      Json::Reader reader;
      Json::Value root;
      if(reader.parse(msg->get_payload_str(), root))
        check_attributes(root);
    });

    std::string *msgs = new std::string[100];
    int i = 0;

    try {

      wait(cli.connect(connOpts)); // Connect to the server
      wait(cli.subscribe(attributes_topic, 1)); // Subscribe to attributes changes.

      while(run){
        if(MQTT::send){
          std::stringstream sstream;
          Json::Value root;

          if(MQTT::send_temp){
            root["temperature"] = Json::Value(Measures::bme_data.temperature);
          }
          if(MQTT::send_press){
            root["pressure"] = Json::Value(Measures::bme_data.pressure);
          }
          if(MQTT::send_hum){
            root["humidity"] = Json::Value(Measures::bme_data.humidity);
          }
          if(MQTT::send_IAQ){
            root["IAQ"] = Json::Value(Measures::bme_data.iaq);
          }
          if(MQTT::send_alt){
            root["altitude"] = Json::Value(Measures::bme_data.altitude);
          }


          // Write to stream.
          if(!root.empty()){
            writer->write(root, &sstream);
            msgs[i++] = sstream.str();
          }
          else{
            msgs[i++] = "{}";
          }


          if(i >= MQTT::send_rate){
            i = 0;

            for(int e = 0; e < MQTT::send_rate; e++){
              wait(cli.publish(telemetry_topic, msgs[e], 0, false));
            }
          }
          sleep(1);
        }
        else{
          //Blocks sending thread until a reception of a true for the MQTT::continuous_send variable
          std::unique_lock<std::mutex> lock(mutex);
          cond.wait(lock);
        }
      }
    }
    catch (const mqtt::exception& exc) {
      //std::cerr << "Error: " << exc.what() << " [" << exc.get_reason_code() << "]" << std::endl;
      return;
    }

    wait(cli.disconnect());
  }
}


/**
 * @brief This function is used to finish the MQTT communications thread correctly. The termination
 *        is not instantaneous and after this function the join() function of the thread in question
 *        must be called.
 *
 */
void MQTT::client_mqtt_thread_finisher(){
  run = false;
  cond.notify_one();
}


/* Private functions ---------------------------------------------------------*/
/**
 * @brief This function is used to wait until an MQTT operation is finished or until the thread finished flag is set.
 *
 * @param[in] token The pointer associated to the MQTT operation.
 *
 */
static void wait(mqtt::token_ptr token){
  while(!token->wait_for(regular_wait_time) && run);
}



/**
 * @brief Read all the attributes that are relevant from the server. Those attributes configure
 *        the information that the client must send to the server and the cadence and number.
 *
 * @param[in] host The IP address or domain of the server.
 * @param[in] access_token The access token that represents the device in the server.
 * @param[in] attributes_topic The MQTT topic that points to attributes info.
 *
 * @return 1 if a on_off variable has changed its state, 0 if not and -1 if error.
 */
static int read_attributes_state(std::string host, std::string access_token, std::string attributes_topic){
  bool received = false;
  std::string request_topic = attributes_topic + "/request/1";
  std::string response_topic = attributes_topic + "/response/+";
  std::string request_payload = "{\"sharedKeys\": \"" +
                    MQTT::send_temp_mqtt_string + "," +
                    MQTT::send_press_mqtt_string + "," +
                    MQTT::send_hum_mqtt_string + "," +
                    MQTT::send_IAQ_mqtt_string + "," +
                    MQTT::send_alt_mqtt_string + "," +
                    MQTT::send_mqtt_string + "," +
                    MQTT::send_rate_mqtt_string + "\"}";


  mqtt::async_client cli(host, "request");

  mqtt::connect_options connOpts;
  connOpts.set_user_name(access_token);
  connOpts.set_keep_alive_interval(20);
  connOpts.set_clean_session(true);

  cli.set_message_callback([&](mqtt::const_message_ptr msg) {
    Json::Reader reader;
    Json::Value root;
    if(reader.parse(msg->get_payload_str(), root)){
      check_attributes(root["shared"]);
      received = true;
    }
  });

  try {
    // Connect to the server
    wait(cli.connect(connOpts));

    // Subscribe to the response topic and request it publishing in request topic.
    wait(cli.subscribe(attributes_topic, 1));
    wait(cli.publish(request_topic, request_payload.c_str(), request_payload.size()));

    while(!received && run){
      usleep(1000*regular_wait_time);
    }
    // Disconnect
    wait(cli.disconnect());

  }
  catch (const mqtt::exception& exc) {
    //std::cerr << "Error: " << exc.what() << " [" << exc.get_reason_code() << "]" << std::endl;
    return -1;
  }
  return 0;
}


/**
 * @brief Process all the attributes in the JSON received as argument and changes the client
 *        behavior accordingly.
 *
 * @param[in] root The root JSON value.
 *
 *
 */
static void check_attributes(Json::Value root){
  uint8_t aux_rate;

  MQTT::send_temp = root.get(MQTT::send_temp_mqtt_string, (bool)MQTT::send_temp).asBool();
  MQTT::send_press = root.get(MQTT::send_press_mqtt_string, (bool)MQTT::send_press).asBool();
  MQTT::send_hum = root.get(MQTT::send_hum_mqtt_string, (bool)MQTT::send_hum).asBool();
  MQTT::send_IAQ = root.get(MQTT::send_IAQ_mqtt_string, (bool)MQTT::send_IAQ).asBool();
  MQTT::send_alt = root.get(MQTT::send_alt_mqtt_string, (bool)MQTT::send_alt).asBool();

  MQTT::send = root.get(MQTT::send_mqtt_string, (bool)MQTT::send).asBool();
  if(MQTT::send)
    cond.notify_one();

  aux_rate = root.get("sendRate", (uint8_t)MQTT::send_rate).asUInt();
  if(aux_rate <= 0)
    aux_rate = 1;
  else if(aux_rate > 100)
    aux_rate = 100;
  MQTT::send_rate = aux_rate;

}


