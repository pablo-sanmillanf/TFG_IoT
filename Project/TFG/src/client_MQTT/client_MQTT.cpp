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
std::atomic_bool MQTT::temp_on = true;
std::atomic_bool MQTT::press_on = true;
std::atomic_bool MQTT::hum_on = true;
std::atomic_bool MQTT::IAQ_on = true;
std::atomic_bool MQTT::alt_on = true;

std::atomic_bool MQTT::continuous_send = true;
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
static bool check_attributes(Json::Value root);
/* Functions -----------------------------------------------------------------*/

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
void MQTT::client_mqtt_thread(std::string host, std::string access_token,
    std::string telemetry_topic, std::string attributes_topic, std::function<void()> on_off_callback){

  run = true;

  int result = read_attributes_state(host, access_token, attributes_topic);
  if(result != -1){

    if(result==1)
      on_off_callback();

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
      std::cout << "MSG_RECEIVED: " << msg->get_payload_str() << std::endl;
      if(reader.parse(msg->get_payload_str(), root))
        if(check_attributes(root))
          on_off_callback();
    });

    std::string *msgs = new std::string[100];
    int i = 0;

    try {

      wait(cli.connect(connOpts)); // Connect to the server
      wait(cli.subscribe(attributes_topic, 1)); // Subscribe to attributes changes.

      while(run){
        if(MQTT::continuous_send){
          std::stringstream sstream;
          Json::Value root;

          if(MQTT::send_temp && MQTT::temp_on){
            root["temperature"] = Json::Value(Measures::bme_data.temperature);
          }
          if(MQTT::send_press && MQTT::press_on){
            root["pressure"] = Json::Value(Measures::bme_data.pressure);
          }
          if(MQTT::send_hum && MQTT::hum_on){
            root["humidity"] = Json::Value(Measures::bme_data.humidity);
          }
          if(MQTT::send_IAQ && MQTT::IAQ_on){
            root["IAQ"] = Json::Value(Measures::bme_data.iaq);
          }
          if(MQTT::send_alt && MQTT::alt_on){
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

            std::cout << std::endl << "DATA SENT: " << std::endl;
            for(int e = 0; e < MQTT::send_rate; e++){
              while(!cli.publish(telemetry_topic, msgs[e], 0, false)->wait_for(regular_wait_time) && run);
              std::cout << "It " << e << ": " << msgs[e] << std::endl;
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
      std::cerr << "Error: " << exc.what() << " [" << exc.get_reason_code() << "]" << std::endl;
      return;
    }

    wait(cli.disconnect());
  }
  std::cout << "MQTT finished" << std::endl;
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
  int result = 0;
  bool received = false;
  std::string request_topic = attributes_topic + "/request/1";
  std::string response_topic = attributes_topic + "/response/+";
  std::string request_payload = "{\"sharedKeys\": \"" +
                    MQTT::send_temp_mqtt_string + "," +
                    MQTT::send_press_mqtt_string + "," +
                    MQTT::send_hum_mqtt_string + "," +
                    MQTT::send_IAQ_mqtt_string + "," +
                    MQTT::send_alt_mqtt_string + "," +
                    MQTT::temp_on_mqtt_string + "," +
                    MQTT::press_on_mqtt_string + "," +
                    MQTT::hum_on_mqtt_string + "," +
                    MQTT::IAQ_on_mqtt_string + "," +
                    MQTT::alt_on_mqtt_string + "," +
                    MQTT::continuous_send_mqtt_string + "," +
                    MQTT::send_rate_mqtt_string + "\"}";


  mqtt::async_client cli(host, "request");

  mqtt::connect_options connOpts;
  connOpts.set_user_name(access_token);
  connOpts.set_keep_alive_interval(20);
  connOpts.set_clean_session(true);

  cli.set_message_callback([&](mqtt::const_message_ptr msg) {
    Json::Reader reader;
    Json::Value root;
    std::cout << "MSG_RECEIVED: " << msg->get_payload_str() << std::endl;
    if(reader.parse(msg->get_payload_str(), root)){
      if(check_attributes(root["shared"]))
        result = 1;
      received = true;
    }
  });

  try {
    // Connect to the server
    wait(cli.connect(connOpts));

    std::cout << "Connected" << std::endl;

    // Subscribe to the response topic and request it publishing in request topic.
    wait(cli.subscribe(attributes_topic, 1));
    wait(cli.publish(request_topic, request_payload.c_str(), request_payload.size()));

    std::cout << "Published" << std::endl;

    while(!received && run){

      std::cout << "Wait" << std::endl;
      usleep(1000*regular_wait_time);
    }
    std::cout << "Waited: " << received << " - " << run << std::endl;
    // Disconnect
    wait(cli.disconnect());

  }
  catch (const mqtt::exception& exc) {
    std::cerr << "Error: " << exc.what() << " [" << exc.get_reason_code() << "]" << std::endl;
    return -1;
  }
  return result;
}


/**
 * @brief Process all the attributes in the JSON received as argument and changes the client
 *        behavior accordingly.
 *
 * @param[in] root The root JSON value.
 *
 * @return true if a on_off variable has changed its state, false if not.
 *
 */
static bool check_attributes(Json::Value root){
  bool aux_on, notify = false;
  uint8_t aux_rate;

  MQTT::send_temp = root.get(MQTT::send_temp_mqtt_string, (bool)MQTT::send_temp).asBool();
  MQTT::send_press = root.get(MQTT::send_press_mqtt_string, (bool)MQTT::send_press).asBool();
  MQTT::send_hum = root.get(MQTT::send_hum_mqtt_string, (bool)MQTT::send_hum).asBool();
  MQTT::send_IAQ = root.get(MQTT::send_IAQ_mqtt_string, (bool)MQTT::send_IAQ).asBool();
  MQTT::send_alt = root.get(MQTT::send_alt_mqtt_string, (bool)MQTT::send_alt).asBool();

  aux_on = root.get(MQTT::temp_on_mqtt_string, (bool)MQTT::temp_on).asBool();
  if(MQTT::temp_on != aux_on){
    MQTT::temp_on = aux_on;
    notify = true;
  }

  aux_on = root.get(MQTT::press_on_mqtt_string, (bool)MQTT::press_on).asBool();
  if(MQTT::press_on != aux_on){
    MQTT::press_on = aux_on;
    notify = true;
  }

  aux_on = root.get(MQTT::hum_on_mqtt_string, (bool)MQTT::hum_on).asBool();
  if(MQTT::hum_on != aux_on){
    MQTT::hum_on = aux_on;
    notify = true;
  }

  aux_on = root.get(MQTT::IAQ_on_mqtt_string, (bool)MQTT::IAQ_on).asBool();
  if(MQTT::IAQ_on != aux_on){
    MQTT::IAQ_on = aux_on;
    notify = true;
  }

  aux_on = root.get(MQTT::alt_on_mqtt_string, (bool)MQTT::alt_on).asBool();
  if(MQTT::alt_on != aux_on){
    MQTT::alt_on = aux_on;
    notify = true;
  }

  MQTT::continuous_send = root.get(MQTT::continuous_send_mqtt_string, (bool)MQTT::continuous_send).asBool();
  if(MQTT::continuous_send)
    cond.notify_one();

  aux_rate = root.get("sendRate", (uint8_t)MQTT::send_rate).asUInt();
  if(aux_rate <= 0)
    aux_rate = 1;
  else if(aux_rate > 100)
    aux_rate = 100;
  MQTT::send_rate = aux_rate;

  return notify;
}


