#define __CODE__

#ifndef __CODE__

/* C++ reimplementation of the gpiofind tool. */

#include <gpiod.hpp>

#include <cstdlib>
#include <iostream>

int main(int argc, char **argv)
{
  if (argc != 2) {
    ::std::cerr << "usage: " << argv[0] << " <line name>" << ::std::endl;
    return EXIT_FAILURE;
  }

  ::gpiod::line line = ::gpiod::find_line(argv[1]);
  if (!line)
    return EXIT_FAILURE;

  ::std::cout << line.get_chip().name() << " " << line.offset() << ::std::endl;

  return EXIT_SUCCESS;
}


#endif




#define __BUTTON_CODE__

#ifndef __BUTTON_CODE__

/***
 * EXAMPLE OF MULTITHREAD WITH BUTTONS CLASS
 */

#include <thread>
#include "buttons/buttons.h"
#include <iostream>
#include <unistd.h>
#include <gpiod.hpp>


void printer_thread(){
  Buttons::BUTTON data;
  std::string type;
  for(;;){
    data = Buttons::buttonsQueue.pop();
    if(data.type == Buttons::LONG_PULSE)
      type = "LONG";
    else if(data.type == Buttons::LONG_PULSE_RELEASED)
      type = "RELEASED";
    else
      type = "SHORT";

    switch(data.symbol){
      case Buttons::UP:
        std::cout << "UP " << type << " Pulse" << std::endl;
        break;
      case Buttons::DOWN:
        std::cout << "DOWN " << type << " Pulse" << std::endl;
        break;
      case Buttons::LEFT:
        std::cout << "LEFT " << type << " Pulse" << std::endl;
        break;
      case Buttons::RIGHT:
        std::cout << "RIGHT " << type << " Pulse" << std::endl;
        break;
      case Buttons::CENTER:
        std::cout << "CENTER " << type << " Pulse" << std::endl;
        break;
    }
  }
}

int main(){
  std::thread first (Buttons::buttons_thread, 0, 2000, 25, 7, 24, 18, 23);

  usleep(100000);
  std::thread third (printer_thread);  // spawn new thread that calls printer_thread()


  std::cout << "Main wait\n" << std::endl;
  // synchronize threads:
  first.join();                // pauses until first finishes
  third.join();               // pauses until third finishes
  return 0;
}
#endif

//#define MAIN

#ifndef MAIN
#include <iostream>
#include <stdio.h>
#include <stdint.h>
#include "TFTDisplay/TFTDisplay.h"
#include "app_data_storage/app_data_storage.h"
#include "measures/measures.h"
#include "client_MQTT/client_MQTT.h"
#include "buttons/buttons.h"
#include <chrono>
#include <thread>
#include <iomanip>
#include <cmath>
#include <ctime>
#include <sstream>
#include <csignal>

void finisher(int signal);

void main_menu_handler(std::function<void()> right_fn, std::function<void()> left_fn,
    std::function<void()> down_fn, bool timeout);

std::string get_time();

float get_percentage(float min_value, float max_value, float value);

bool yes_no_choice_menu(std::string title, int size);

std::string editable_text_menu(std::string title, std::string text, uint32_t long_pulse_us_cycle,
    std::function<int (std::string *text, int selected, bool right_nleft)> right_left_fn,
    std::function<std::string (std::string text, int selected, bool up_ndown)> up_down_fn);

int text_list_menu(std::string title, std::vector<std::string> list, uint32_t long_pulse_us_cycle, uint32_t index);

void pulse_action(Buttons::BUTTON previous_pulsed, uint32_t us_cycle, std::function<void ()> action);

std::vector<std::string> get_time_zone_list_and_index(std::string timezone_file_path, std::string tzs_directory_path,
    uint16_t *index);

std::vector<std::string> exec_cmd_and_get_lines(const char* cmd);



const std::string HOST_IP_KEY = "HOST_IP";
const std::string MQTT_TOKEN_KEY = "MQTT_TOKEN";
const std::string TEMP_OFFSET_KEY = "TEMP_OFFSET";
const std::string default_host_ip = "192.168.000.001";
const std::string default_mqtt_token = "00000000";
const std::string default_temp_offset = "0";

const std::string tb_telemetry_topic = "v1/devices/me/telemetry";
const std::string tb_attributes_topic = "v1/devices/me/attributes";

const int wait_time_ms = 100;
const int metric_period_s = 4 * 1000/wait_time_ms;

const int min_temp = -10;
const int max_temp = 50;

const std::string timezone_file_path = "/etc/timezone";
const std::string sym_link_timezone_path = "/etc/TZ";
const std::string tzs_directory_path = "/usr/share/zoneinfo/uclibc/";

const std::string networks_file_path = "/etc/wpa_supplicant.conf";

const std::vector<std::string> conf_menus = {
    "Carrusel",
    "IP Broker",
    "Token MQTT",
    "Temp Offset",
    "Hora local",
    "Nuevo WiFi",
    "Apagar"
};
std::atomic_bool run = true;



int main() {
  std::signal(SIGINT, finisher);

  Storage::AppDataStorage storage;

  std::string host_ip = storage.read_data_param(HOST_IP_KEY, default_host_ip);
  std::string mqtt_token = storage.read_data_param(MQTT_TOKEN_KEY, default_mqtt_token);
  float temp_offset = std::stof(storage.read_data_param(TEMP_OFFSET_KEY, default_temp_offset));

  Measures::Meas meas(temp_offset, 100000);

  meas.start_measures();

  auto fn = [&]() {
    std::cout << "ON_OFF changed: \n\tTemp: " << MQTT::temp_on
                                << "\tPress: " << MQTT::press_on
                                << "\tHum: " << MQTT::hum_on
                                << "\tIAQ: " << MQTT::IAQ_on
                                << "\tAlt: " << MQTT::alt_on << std::endl;
  };
  std::thread buttons_thread(Buttons::buttons_thread, 0, 500, 25, 7, 24, 18, 23);
  std::thread mqtt_thread(MQTT::client_mqtt_thread, host_ip, mqtt_token, tb_telemetry_topic, tb_attributes_topic, fn);


  enum {TIME, TEMP, HUM, PRESS, ALT, IAQ, CONF} state;

  TFTDisplay::start();

  int cnt;  //This cnt will be used to count the seconds per menu in the main carousel and to count the conf menu
            //where the user is

  int top_border_conf_menu, bottom_border_conf_menu;
  bool change_menus = true;

  //Menus configuration functions
  auto conf_fn = [&]() {
    cnt = 0;
    state = CONF;
    top_border_conf_menu = 0;
    bottom_border_conf_menu = TFTDisplay::max_conf_menus - 1;
    TFTDisplay::print_conf_menu(true, true, conf_menus, 0, -1);
    std::this_thread::sleep_for(std::chrono::milliseconds(wait_time_ms));
  };

  auto init_time_fn = [&]() {
    cnt = 0;
    state = TIME;
    std::string str_time = get_time();
    TFTDisplay::print_main_menu_time(true, str_time);
    std::this_thread::sleep_for(std::chrono::milliseconds(wait_time_ms));
  };

  auto init_temp_fn = [&]() {
    cnt = 0;
    state = TEMP;
    TFTDisplay::print_main_menu_temp(
        true,
        Measures::bme_data.temperature,
        get_percentage(min_temp, max_temp, Measures::bme_data.temperature)
        );
    std::this_thread::sleep_for(std::chrono::milliseconds(wait_time_ms));
  };

  auto init_hum_fn = [&]() {
    cnt = 0;
    state = HUM;
    TFTDisplay::print_main_menu_hum(true, Measures::bme_data.humidity);
    std::this_thread::sleep_for(std::chrono::milliseconds(wait_time_ms));
  };

  auto init_press_fn = [&]() {
    cnt = 0;
    state = PRESS;
    TFTDisplay::print_main_menu_press(true, Measures::bme_data.pressure);
    std::this_thread::sleep_for(std::chrono::milliseconds(wait_time_ms));
  };

  auto init_alt_fn = [&]() {
    cnt = 0;
    state = ALT;
    TFTDisplay::print_main_menu_alt(true, Measures::bme_data.altitude);
    std::this_thread::sleep_for(std::chrono::milliseconds(wait_time_ms));
  };

  auto init_iaq_fn = [&]() {
    cnt = 0;
    state = IAQ;
    TFTDisplay::print_main_menu_iaq(true, Measures::bme_data.iaq);
    std::this_thread::sleep_for(std::chrono::milliseconds(wait_time_ms));
  };

  init_time_fn();
  while(run){
    switch(state){
      case TIME:
      {
        std::string str_time = get_time();
        TFTDisplay::print_main_menu_time(false, str_time);
        std::this_thread::sleep_for(std::chrono::milliseconds(wait_time_ms));

        main_menu_handler(
            init_temp_fn,
            init_iaq_fn,
            conf_fn,
            ++cnt == metric_period_s && change_menus
        );
        break;
      }
      case TEMP:
      {
        TFTDisplay::print_main_menu_temp(
            false,
            Measures::bme_data.temperature,
            get_percentage(min_temp, max_temp, Measures::bme_data.temperature)
            );
        std::this_thread::sleep_for(std::chrono::milliseconds(wait_time_ms));


        main_menu_handler(
            init_hum_fn,
            init_time_fn,
            conf_fn,
            ++cnt == metric_period_s && change_menus
        );
        break;
      }
      case HUM:
      {
        TFTDisplay::print_main_menu_hum(false, Measures::bme_data.humidity);
        std::this_thread::sleep_for(std::chrono::milliseconds(wait_time_ms));

        main_menu_handler(
            init_press_fn,
            init_temp_fn,
            conf_fn,
            ++cnt == metric_period_s && change_menus
        );
        break;
      }
      case PRESS:
      {
        TFTDisplay::print_main_menu_press(false, Measures::bme_data.pressure);
        std::this_thread::sleep_for(std::chrono::milliseconds(wait_time_ms));

        main_menu_handler(
            init_alt_fn,
            init_hum_fn,
            conf_fn,
            ++cnt == metric_period_s && change_menus
        );
        break;
      }
      case ALT:
      {
        TFTDisplay::print_main_menu_alt(false, Measures::bme_data.altitude);
        std::this_thread::sleep_for(std::chrono::milliseconds(wait_time_ms));

        main_menu_handler(
            init_iaq_fn,
            init_press_fn,
            conf_fn,
            ++cnt == metric_period_s && change_menus
        );
        break;
      }
      case IAQ:
      {
        TFTDisplay::print_main_menu_iaq(false, Measures::bme_data.iaq);
        std::this_thread::sleep_for(std::chrono::milliseconds(wait_time_ms));

        main_menu_handler(
            init_time_fn,
            init_alt_fn,
            conf_fn,
            ++cnt == metric_period_s && change_menus
        );
        break;
      }
      case CONF:
      {
        Buttons::BUTTON data = Buttons::buttonsQueue.pop();

        if(data.type != Buttons::LONG_PULSE_RELEASED){
          if(data.symbol == Buttons::RIGHT){
            enum CONF_MENU_OPTION {CARRUSEL = 0, IP_BROKER = 1, TOKEN_MQTT, TEMP_OFFSET, HORA_LOCAL, NUEVO_WIFI, APAGAR};
            CONF_MENU_OPTION conf_menu = static_cast<CONF_MENU_OPTION>(cnt + top_border_conf_menu);

            switch(conf_menu){
              case CARRUSEL:
              {
                change_menus = yes_no_choice_menu("Activar carrusel?", 1);
                break;
              }
              case IP_BROKER:
              {
                std::string new_host_ip = editable_text_menu(conf_menus[IP_BROKER], host_ip, 1,
                  [](std::string *text, int selected, bool right_nleft) {
                    if(right_nleft){
                      do{
                        selected++;
                      }while((*text)[selected] == '.' && selected < int(text->size() - 1));
                      return selected;
                    }
                    else{
                      do{
                        selected--;
                      }while((*text)[selected] == '.' && selected > 1);
                      return selected;
                    }
                  },
                  [](std::string text, int selected, bool up_ndown) {
                    std::string delim = ".";

                    int start = 0;
                    auto end = text.find(delim);
                    while (end != std::string::npos){
                      if(selected < int(end)){
                        int byte = stoi(text.substr(start, end - start));
                        int factor = std::pow(10, float(2 - (selected - start)));
                        if(up_ndown){
                          if(factor + byte > 0xFF)
                            byte = 0xFF;
                          else
                            byte += factor;
                        }
                        else{
                          if(byte - factor < 0)
                            byte = 0;
                          else
                            byte -= factor;
                        }
                        std::stringstream ss;
                        ss << std::setw(3) << std::setfill('0') << byte;
                        text.replace(start, end - start, ss.str());

                        return text;
                      }
                      else{
                        start = end + delim.length();
                        end = text.find(delim, start);
                      }
                    }
                    if(selected < int(text.length())){
                      int byte = stoi(text.substr(start, text.length() - start));
                      int factor = std::pow(10, float(2 - (selected - start)));
                      if(up_ndown){
                        if(factor + byte > 0xFF)
                          byte = 0xFF;
                        else
                          byte += factor;
                      }
                      else{
                        if(byte - factor < 0)
                          byte = 0;
                        else
                          byte -= factor;
                      }
                      std::stringstream ss;
                      ss << std::setw(3) << std::setfill('0') << byte;
                      text.replace(start, text.length() - start, ss.str());
                    }
                    return text;
                  });
                if(host_ip.compare(new_host_ip) != 0 && yes_no_choice_menu("Guardar?", 2)){
                  host_ip = new_host_ip;
                  storage.write_data_param(HOST_IP_KEY, host_ip);

                  MQTT::client_mqtt_thread_finisher();
                  mqtt_thread.join();
                  mqtt_thread = std::thread(
                      MQTT::client_mqtt_thread, host_ip, mqtt_token, tb_telemetry_topic, tb_attributes_topic, fn);
                }
                break;
              }
              case TOKEN_MQTT:
              {
                std::string new_mqtt_token = editable_text_menu(conf_menus[TOKEN_MQTT], mqtt_token, 1,
                  [](std::string *text, int selected, bool right_nleft) {
                    if(right_nleft){
                      if(selected == int(text->size() - 1) && text->back() != ' '){
                        text->push_back(' ');
                        return selected + 1;
                      }
                      else if(selected < int(text->size() - 1))
                        return selected + 1;
                      return selected;
                    }
                    else{
                      if(text->size() > 1 && selected == int(text->size() - 1) && text->back() == ' ')
                        text->pop_back();
                      if(selected > 0)
                        return selected - 1;
                      return selected;
                    }
                  },
                  [](std::string text, int selected, bool up_ndown) {
                    uint8_t min_char = 0x20, max_char = 0x7F;
                    if(up_ndown){
                      if(++text[selected] == max_char)
                        text[selected] =
                            (selected == int(text.size() - 1)) ? (min_char) : (min_char + 1);
                    }
                    else{
                      if(selected == int(text.size() - 1)){
                        if(--text[selected] == min_char - 1)
                          text[selected] = max_char - 1;
                      }
                      else{
                        if(--text[selected] == min_char)
                          text[selected] = max_char - 1;
                      }
                    }
                    return text;
                  });
                if(mqtt_token.compare(new_mqtt_token) != 0 && yes_no_choice_menu("Guardar?", 2)){
                  mqtt_token = new_mqtt_token;
                  storage.write_data_param(MQTT_TOKEN_KEY, mqtt_token);

                  MQTT::client_mqtt_thread_finisher();
                  mqtt_thread.join();
                  mqtt_thread = std::thread(
                      MQTT::client_mqtt_thread, host_ip, mqtt_token, tb_telemetry_topic, tb_attributes_topic, fn);
                }
                break;
              }
              case TEMP_OFFSET:
              {
                std::stringstream stream;
                stream << std::fixed << std::setprecision(2) << temp_offset;
                std::string temp_offset_str = stream.str();

                std::string new_temp_offset_str = editable_text_menu(conf_menus[TEMP_OFFSET], temp_offset_str, 1,
                  [](std::string *text, int selected, bool right_nleft) {
                    if(right_nleft){
                      do{
                        selected++;
                      }while((*text)[selected] == '.' && selected < int(text->size() - 1));
                      return selected;
                    }
                    else{
                      do{
                        selected--;
                      }while((*text)[selected] == '.' && selected > 1);
                      return selected;
                    }
                  },
                  [](std::string text, int selected, bool up_ndown) {
                    float offset = std::atof(text.c_str());
                    float factor;

                    //We are assuming that the number has the format 0.00
                    switch(selected){
                      case 0:
                        factor = 1;
                        break;
                      case 2:
                        factor = 0.1;
                        break;
                      case 3:
                        factor = 0.01;
                        break;
                    }

                    if(up_ndown){
                      if(offset + factor > 9.99)
                        offset = 9.99;
                      else
                        offset += factor;
                    }
                    else{
                      if(offset - factor < 0)
                        offset = 0;
                      else
                        offset -= factor;
                    }

                    std::stringstream stream;
                    stream << std::fixed << std::setprecision(2) << offset;
                    return stream.str();
                  });

                if(temp_offset_str.compare(new_temp_offset_str) != 0 && yes_no_choice_menu("Guardar?", 2)){
                  temp_offset = std::atof(new_temp_offset_str.c_str());

                  storage.write_data_param(TEMP_OFFSET_KEY, new_temp_offset_str);
                  meas.set_temp_offset(temp_offset);
                }
                break;
              }
              case HORA_LOCAL:
              {
                uint16_t tz_index;
                std::vector<std::string> tzs = get_time_zone_list_and_index(timezone_file_path, tzs_directory_path, &tz_index);
                uint16_t new_tz_index = text_list_menu(conf_menus[HORA_LOCAL], tzs, 1, tz_index);

                if(tz_index != new_tz_index && yes_no_choice_menu("Guardar?", 2)){
                  // Write the new time zone in timezone file
                  std::ofstream ofs_tz_file(timezone_file_path);
                  ofs_tz_file << tzs[new_tz_index] << std::endl;
                  ofs_tz_file.close();

                  system(("ln -sf " + tzs_directory_path + tzs[new_tz_index] + " " + sym_link_timezone_path).c_str());
                }
                break;
              }
              case NUEVO_WIFI:
              {
                TFTDisplay::print_centered_title("Listando redes...", 1);
                std::vector<std::string> avail_nets =
                    exec_cmd_and_get_lines("iwlist wlan0 scan | grep ESSID | grep -o '\".*\"' | tr -d '\"'");
                uint16_t avail_net_index = text_list_menu("Red WiFi", avail_nets, 1, 0);


                bool wrong_psw = false;
                std::string new_psw = " ";
                do{

                  new_psw = editable_text_menu("Clave WiFi", new_psw, 1,
                    [](std::string *text, int selected, bool right_nleft) {
                      if(right_nleft){
                        if(selected == int(text->size() - 1) && text->back() != ' '){
                          text->push_back(' ');
                          return selected + 1;
                        }
                        else if(selected < int(text->size() - 1))
                          return selected + 1;
                        return selected;
                      }
                      else{
                        if(text->size() > 1 && selected == int(text->size() - 1) && text->back() == ' ')
                          text->pop_back();
                        if(selected > 0)
                          return selected - 1;
                        return selected;
                      }
                    },
                    [](std::string text, int selected, bool up_ndown) {
                      uint8_t min_char = 0x20, max_char = 0x7F;
                      if(up_ndown){
                        if(++text[selected] == max_char)
                          text[selected] =
                              (selected == int(text.size() - 1)) ? (min_char) : (min_char + 1);
                      }
                      else{
                        if(selected == int(text.size() - 1)){
                          if(--text[selected] == min_char - 1)
                            text[selected] = max_char - 1;
                        }
                        else{
                          if(--text[selected] == min_char)
                            text[selected] = max_char - 1;
                        }
                      }
                      return text;
                    });
                  if(new_psw.size() < 8 || new_psw.size() > 63){
                    TFTDisplay::print_centered_title("La clave debe\ntener entre\n8 y 63\ncaracteres.", 1);
                    std::this_thread::sleep_for(std::chrono::seconds(2));

                    if(yes_no_choice_menu("Elegir otra clave?", 1))
                      wrong_psw = true;
                    else
                      wrong_psw = false;
                  }
                  else{
                    wrong_psw = false;
                    if(yes_no_choice_menu("Guardar Wifi?", 1)){

                      std::vector<std::string> saved_nets = exec_cmd_and_get_lines(
                                ("cat " + networks_file_path + " | grep ssid | grep -o '\".*\"' | tr -d '\"'").c_str());
                      uint16_t saved_net_index = std::find(
                          saved_nets.begin(), saved_nets.end(), avail_nets[avail_net_index]) - saved_nets.begin();

                      if(saved_net_index < saved_nets.size()){
                        std::vector<std::string> saved_nets_open_line_nbr =
                            exec_cmd_and_get_lines(("sed -n '/network={/=' " + networks_file_path).c_str());
                        std::vector<std::string> saved_nets_close_line_nbr =
                            exec_cmd_and_get_lines(("sed -n '/}/=' " + networks_file_path).c_str());

                        system(("sed -i -e '" + saved_nets_open_line_nbr[saved_net_index] + "," +
                                               saved_nets_close_line_nbr[saved_net_index] + "d' " +
                                               networks_file_path).c_str());
                      }
                      //Add new network to wpa_supplicant.conf
                      system(("wpa_passphrase \"" + avail_nets[avail_net_index] + "\" \"" +
                          new_psw + "\" >> " + networks_file_path).c_str());

                      //Reconfigure wpa_supplicant
                      system("wpa_cli -i wlan0 reconfigure");
                    }
                  }

                }while(wrong_psw);
                break;
              }
              case APAGAR:
              {
                if(yes_no_choice_menu("Apagar RPI?", 1)){
                  TFTDisplay::print_centered_title("Apagando...", 2);
                  std::this_thread::sleep_for(std::chrono::seconds(1));
                  TFTDisplay::print_centered_title(" ", 1);
                  system("poweroff");
                }
                break;
              }
            }
            std::vector<std::string> sbv(conf_menus.begin() + cnt, conf_menus.begin() + cnt + TFTDisplay::max_conf_menus);
            TFTDisplay::print_conf_menu(true, true, sbv, top_border_conf_menu, -1);
          }
          else if(data.symbol == Buttons::LEFT){
            init_time_fn();
          }
          else if(data.symbol == Buttons::DOWN){
            pulse_action(data, 1, [&](){
              if(bottom_border_conf_menu == 0){
                if(cnt < (int)conf_menus.size() - 1 - top_border_conf_menu){
                  cnt++;
                  std::vector<std::string> sbv(conf_menus.begin() + cnt,
                      conf_menus.begin() + cnt + TFTDisplay::max_conf_menus);
                  TFTDisplay::print_conf_menu(false, true, sbv, top_border_conf_menu, -1);
                }
              }
              else{
                std::vector<std::string> sbv(conf_menus.begin() + cnt,
                    conf_menus.begin() + cnt + TFTDisplay::max_conf_menus);
                TFTDisplay::print_conf_menu(false, false, sbv, top_border_conf_menu + 1, top_border_conf_menu);
                bottom_border_conf_menu--;
                top_border_conf_menu++;
              }
            });
            std::this_thread::sleep_for(std::chrono::milliseconds(wait_time_ms));
          }
          else if(data.symbol == Buttons::UP){
            pulse_action(data, 1, [&](){
              if(top_border_conf_menu == 0){
                if(cnt > 0){
                  cnt--;
                  std::vector<std::string> sbv(conf_menus.begin() + cnt,
                      conf_menus.begin() + cnt + TFTDisplay::max_conf_menus);
                  TFTDisplay::print_conf_menu(false, true, sbv, top_border_conf_menu, -1);
                }
              }
              else{
                std::vector<std::string> sbv(conf_menus.begin() + cnt,
                    conf_menus.begin() + cnt + TFTDisplay::max_conf_menus);
                TFTDisplay::print_conf_menu(false, false, sbv, top_border_conf_menu - 1, top_border_conf_menu);
                bottom_border_conf_menu++;
                top_border_conf_menu--;
              }
            });
            std::this_thread::sleep_for(std::chrono::milliseconds(wait_time_ms));
          }
        }
        break;
      }
    }
  }
  TFTDisplay::end();
  Buttons::buttons_thread_finisher();
  MQTT::client_mqtt_thread_finisher();
  buttons_thread.join();
  mqtt_thread.join();

}



/**
  * @brief End of the program
  */
void finisher(int signal){
  run = false;
  printf("\n\n|----------------------------------------|");
  printf("\n|------------PROGRAM FINISHED------------|");
  printf("\n|----------------------------------------|\n");
}


/**
  * @brief     Obtain the current time as a string with the format "H:M".
  *
  * @return    The current time as a string.
  */
std::string get_time(){
  auto t = std::time(nullptr);
  auto tm = *std::localtime(&t);
  std::stringstream ss;
  ss << std::put_time(&tm, "%H:%M:%S");
  return ss.str();
}

/**
  * @brief Map the supplied value into the range [min_value, max_value] and gets the resulting
  *        percentage.
  *
  * @param[in] min_value Low limit of the range.
  * @param[in] max_value High limit of the range.
  * @param[in] value Value to be mapped.
  *
  * @return The resulting percentage.
  */
float get_percentage(float min_value, float max_value, float value){
  return (value - min_value)*100/(max_value - min_value);
}

/**
  * @brief Find in "tzs_directory_path" all the available time zones in the system and return a list of all of them.
  *        Also, find the current timezone in "timezone_file_path" and returns the index of this timezone in the list.
  *
  * @param[in] timezone_file_path Path to the file where the current timezone is located.
  * @param[in] tzs_directory_path Path to the system directory where the timezones are located.
  * @param[out] index Index in the list of the current timezone.
  *
  * @return The list of all the timezones.
  */
std::vector<std::string> get_time_zone_list_and_index(std::string timezone_file_path, std::string tzs_directory_path,
    uint16_t *index){

  std::vector<std::string> tzs;
  tzs.reserve(256);

  std::string initial_tz;

  // Read the current time zone
  std::ifstream ifs(timezone_file_path);
  std::getline (ifs, initial_tz);
  ifs.close();

  //Obtain all available time zones sorted
  for (auto const& i : std::filesystem::recursive_directory_iterator (tzs_directory_path))
    if (!is_directory(i.path()))
      tzs.push_back(i.path().string().erase(0, tzs_directory_path.length()));
  sort(tzs.begin(), tzs.end());

  *index = std::find(tzs.begin(), tzs.end(), initial_tz) - tzs.begin();

  return tzs;
}


/**
  * @brief This function manages the behavior of the joystick pulses in the main menu.
  *
  * @param[in] right_fn Function to be executed when a right pulse is detected and when the timeout is true.
  * @param[in] left_fn Function to be executed when a left pulse is detected.
  * @param[in] down_fn Function to be executed when a down pulse is detected.
  * @param[in] timeout If true, right_fn will be executed.
  */
void main_menu_handler(std::function<void()> right_fn, std::function<void()> left_fn,
    std::function<void()> down_fn, bool timeout){
  Buttons::BUTTON data;

  if(Buttons::buttonsQueue.size() != 0){
    data = Buttons::buttonsQueue.pop();
    if(data.type != Buttons::LONG_PULSE_RELEASED){
      if(data.symbol == Buttons::RIGHT)
        right_fn();
      else if(data.symbol == Buttons::LEFT)
        left_fn();
      else if(data.symbol == Buttons::DOWN)
        down_fn();
    }
  }
  if(timeout)
    right_fn();
}


/**
  * @brief Manages the behavior of an editable text menu.
  *
  * @param[in] title The title to be printed before the edit menu appears.
  * @param[in] text The text that will be edited.
  * @param[in] long_pulse_us_cycle The delay time in us to execute an action repeatedly while
  *                                a long pulse is detected.
  * @param[in] right_left_fn Function to be executed when a right or left pulse is detected i.e. when
  *                          the user wants to change to another character. This function receives the
  *                          edited text, the currently selected char position and a boolean that indicates
  *                          if the button pulsed is the right (true), or the left (false). It must return
  *                          the char position of the edited char after the button pulse.
  * @param[in] up_down_fn Function to be executed when an up or down pulse is detected i.e. when
  *                       the user wants to modify the selected character. This function receives the
  *                       edited text, the currently selected char position and a boolean that indicates
  *                       if the button pulsed is the up (true), or the down (false). It must return
  *                       the new string.
  *
  * @return The new text.
  */
std::string editable_text_menu(std::string title, std::string text, uint32_t long_pulse_us_cycle,
    std::function<int (std::string *text, int selected, bool right_nleft)> right_left_fn,
    std::function<std::string (std::string text, int selected, bool up_ndown)> up_down_fn){

  Buttons::BUTTON pulsed;

  int init_visible_str = 0, rightmost_border_menu = TFTDisplay::max_chars_edit_menu - 1, leftmost_border_menu = 0;

  TFTDisplay::print_centered_title(title, 2);
  std::this_thread::sleep_for(std::chrono::seconds(1));
  TFTDisplay::print_editable_text(true, true, text, leftmost_border_menu, -1);

  while(true){
    pulsed = Buttons::buttonsQueue.pop();

    //Quit time zone selection
    if(pulsed.symbol == Buttons::CENTER){
      return text;
    }
    else if(pulsed.symbol == Buttons::RIGHT){
      pulse_action(pulsed, long_pulse_us_cycle,
        [&] () {
          int step = right_left_fn(&text, init_visible_str + leftmost_border_menu, true) -
              (init_visible_str + leftmost_border_menu);
          bool text_refresh = false;

          for(int i = 0; i < step; i++){
            if(rightmost_border_menu == 0){
              if(init_visible_str < int(text.size() - 1 - leftmost_border_menu)){
                init_visible_str++;
                text_refresh = true;
              }
            }
            else{
              rightmost_border_menu--;
              leftmost_border_menu++;
            }
          }

          std::string sub_str = text.substr(init_visible_str, TFTDisplay::max_chars_edit_menu);

          if(text_refresh){
            TFTDisplay::print_editable_text(false, true, sub_str, leftmost_border_menu, -1);
          }
          else{
            TFTDisplay::print_editable_text(
                false, false, sub_str, leftmost_border_menu, leftmost_border_menu - step);
          }
        }
      );
    }
    else if(pulsed.symbol == Buttons::LEFT){
      pulse_action(pulsed, long_pulse_us_cycle,
        [&] () {
          int step = (init_visible_str + leftmost_border_menu) -
              right_left_fn(&text, init_visible_str + leftmost_border_menu, false);
          bool text_refresh = false;

          for(int i = 0; i < step; i++){
            if(leftmost_border_menu == 0){
              if(init_visible_str > 0){
                init_visible_str--;
                text_refresh = true;
              }
            }
            else{
              rightmost_border_menu++;
              leftmost_border_menu--;
            }
          }

          std::string sub_str = text.substr(init_visible_str, TFTDisplay::max_chars_edit_menu);

          if(text_refresh){
            TFTDisplay::print_editable_text(false, true, sub_str, leftmost_border_menu, -1);
          }
          else{
            TFTDisplay::print_editable_text(
                false, false, sub_str, leftmost_border_menu, leftmost_border_menu + step);
          }
        }
      );
    }
    else if(pulsed.symbol == Buttons::UP){
      pulse_action(pulsed, long_pulse_us_cycle,
        [&] () {
          text = up_down_fn(text, init_visible_str + leftmost_border_menu, true);
          std::string sub_str = text.substr(init_visible_str, TFTDisplay::max_chars_edit_menu);
          TFTDisplay::print_editable_text(false, true, sub_str, leftmost_border_menu, -1);
        }
      );
    }
    else if(pulsed.symbol == Buttons::DOWN){
      pulse_action(pulsed, long_pulse_us_cycle,
        [&] () {
          text = up_down_fn(text, init_visible_str + leftmost_border_menu, false);
          std::string sub_str = text.substr(init_visible_str, TFTDisplay::max_chars_edit_menu);
          TFTDisplay::print_editable_text(false, true, sub_str, leftmost_border_menu, -1);
        }
      );
    }
  }
  return text;
}



/**
  * @brief Manages the behavior of a vertical cyclic text list.
  *
  * @param[in] title The title to be printed before the text list appears.
  * @param[in] list The list of all the options printed in the menu.
  * @param[in] long_pulse_us_cycle The delay time in us to execute an action repeatedly while
  *                                a long pulse is detected.
  * @param[in] index Index of the currently selected item.
  *
  * @return The index of the new selected item.
  */
int text_list_menu(std::string title, std::vector<std::string> list, uint32_t long_pulse_us_cycle, uint32_t index){

  Buttons::BUTTON pulsed;

  auto exit_fn = [&] (){
    return Buttons::buttonsQueue.size() != 0;
  };

  index = (index + TFTDisplay::max_text_list_elements - TFTDisplay::central_text_list_element) % TFTDisplay::max_text_list_elements;

  std::vector<std::string> sublist;
  sublist.resize(TFTDisplay::max_text_list_elements);
  for(int i = 0; i < TFTDisplay::max_text_list_elements; i++){
    sublist[i] = list[(i + index) % list.size()];
  }

  TFTDisplay::print_centered_title(title, 2);
  std::this_thread::sleep_for(std::chrono::seconds(1));
  TFTDisplay::print_text_list(true, true, sublist, [] (){return 0;});

  while(true){
    if(Buttons::buttonsQueue.size() != 0){

      pulsed = Buttons::buttonsQueue.pop();

      //Quit time zone selection
      if(pulsed.symbol == Buttons::CENTER){
        return (index + TFTDisplay::central_text_list_element) % list.size();
      }
      if(pulsed.symbol == Buttons::UP){
        pulse_action(pulsed, long_pulse_us_cycle,
          [&] () {
            index--;
            std::vector<std::string> sublist;
            sublist.resize(TFTDisplay::max_text_list_elements);
            for(int i = 0; i < TFTDisplay::max_text_list_elements; i++){
              sublist[i] = list[(i + index) % list.size()];
            }

            TFTDisplay::print_text_list(false, true, sublist, [] (){return 0;});
          }
        );
      }
      else if(pulsed.symbol == Buttons::DOWN){
        pulse_action(pulsed, long_pulse_us_cycle,
          [&] () {
            index++;
            std::vector<std::string> sublist;
            sublist.resize(TFTDisplay::max_text_list_elements);
            for(int i = 0; i < TFTDisplay::max_text_list_elements; i++){
              sublist[i] = list[(i + index) % list.size()];
            }

            TFTDisplay::print_text_list(false, true, sublist, [] (){return 0;});
          }
        );
      }
    }
    else{
      std::vector<std::string> sublist;
      sublist.resize(TFTDisplay::max_text_list_elements);
      for(int i = 0; i < TFTDisplay::max_text_list_elements; i++){
        sublist[i] = list[(i + index) % list.size()];
      }
      int cnt = 0;
      while(cnt++ < 10 && Buttons::buttonsQueue.size() == 0){
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
      }
      TFTDisplay::print_text_list(false, false, sublist, exit_fn);
      if(Buttons::buttonsQueue.size() == 0){
        cnt = 0;
        while(cnt++ < 10 && Buttons::buttonsQueue.size() == 0){
          std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
        if(Buttons::buttonsQueue.size() == 0){
          TFTDisplay::print_text_list(false, true, sublist, exit_fn);
          if(Buttons::buttonsQueue.size() == 0){
            cnt = 0;
            while(cnt++ < 10 && Buttons::buttonsQueue.size() == 0){
              std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }
          }
        }
      }
    }
  }
  return (index + TFTDisplay::central_text_list_element) % list.size();
}


/**
  * @brief Prints a yes/no window and returns the selected option by the user.
  *
  * @param[in] title The text represented as the title of the yes/no window.
  * @param[in] size The text size. Can be 1 (small), 2 (medium) or 3 (big). Greater values will
  *                 be truncated to 3.
  *
  * @return True if the user has accepted the message or false if not.
  */
bool yes_no_choice_menu(std::string title, int size){
  TFTDisplay::print_selection_text(title, size);
  Buttons::BUTTON data;

  for(;;){
    data = Buttons::buttonsQueue.pop();
    if(data.type == Buttons::SHORT_PULSE){
      if(data.symbol == Buttons::RIGHT){
        return false;
      }
      else if(data.symbol == Buttons::LEFT){
        return true;
      }
    }
  }
  return false;
}


/**
  * @brief Execute a bash command and return its output as a string vector. Each item represents a
  *        line of the output.
  *
  * @param[in] cmd The bash command to be executed
  *
  * @return The output as a string vector.
  */
std::vector<std::string> exec_cmd_and_get_lines(const char* cmd) {
    std::array<char, 128> buffer;
    std::string result;
    std::vector<std::string> lines;

    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd, "r"), pclose);
    if (!pipe) {
        return lines;
    }
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        result += buffer.data();
    }

    std::stringstream ss(result);
    std::string item;

    lines.reserve(32);
    while(std::getline(ss, item,'\n'))
      lines.push_back(item);

    return lines;
}



/**
 * @brief This function manages the behavior when a long pulse is detected. It will execute an action every `us_cycle` until
 *        the detection of a release of the button.
 *
 * @param[in] previous_symbol  The Button symbol that is currently pressed. This will be used to compare with the buttons
 *                             released.
 * @param[in] us_cycle  The time in microseconds to check if the button has been released.
 * @param[in] action  The function that will be executed in every cycle.
 *
 */
void pulse_action(Buttons::BUTTON previous_pulsed, uint32_t us_cycle, std::function<void ()> action){
  bool pressed = true;
  Buttons::BUTTON aux_pulsed;

  //Execute action until button release.

  if(previous_pulsed.type == Buttons::SHORT_PULSE)
    action();
  else if(previous_pulsed.type == Buttons::LONG_PULSE)
    do{
      action();
      usleep(us_cycle);
      if(Buttons::buttonsQueue.size() > 0){
        aux_pulsed = Buttons::buttonsQueue.pop();
        pressed = aux_pulsed.symbol != previous_pulsed.symbol ||
                  aux_pulsed.type != Buttons::LONG_PULSE_RELEASED;
      }
    }while(pressed);
}

#endif
