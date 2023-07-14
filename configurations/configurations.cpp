/**
  ******************************************************************************
  * @file   configurations.cpp
  * @author Pablo San Millán Fierro (pablo.sanmillanf@alumnos.upm.es)
  * @brief  Configuration functions handler Module. Requires the button thread
  *         to be in operation for this module to work.
  *
  * @note   End-of-degree work.
  *         This module handles all the configuration functions that will have
  *         the final app.
  ******************************************************************************
*/

/* Includes ------------------------------------------------------------------*/
#include "configurations.h" // Module header
#include "buttons.h"
#include <iostream>
#include <filesystem>
#include <vector>
#include <algorithm>
#include <fstream>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sstream>
#include <cmath>

/* External variables---------------------------------------------------------*/
/* Private defines -----------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private variables----------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Functions -----------------------------------------------------------------*/

/**
 * @brief This function will handle the selection of a new timezone and will update the timezone file and the localtime
 * symlink. To change from one timezone to another, use the LEFT and RIGHT joystick buttons.
 *
 * @param[in] timezone_file_path      Path to the timezone file. The default value is "/etc/timezone"
 * @param[in] localtime_symlink_path  Path to the localtime symlink. The default value is "/etc/localtime"
 * @param[in] tzs_directory_path      Path to the parent directory of all the timezone files. The default value is
 *                                    "/usr/share/zoneinfo/posix/".
 *
 * @return 0 if success, -1 if error.
 */
int Configurations::handle_time_zone(
    std::string timezone_file_path,
    std::string localtime_symlink_path,
    std::string tzs_directory_path){

  std::vector<std::string> tzs;
  tzs.reserve(256);

  std::string initial_tz;

  // Read the current time zone
  std::ifstream ifs(timezone_file_path);
  if(!std::getline (ifs, initial_tz))
    return -1;
  ifs.close();

  //Obtain all available time zones sorted
  for (auto const& i : std::filesystem::recursive_directory_iterator (tzs_directory_path))
    if (!is_directory(i.path()))
      tzs.push_back(i.path().string().erase(0, tzs_directory_path.length()));
  sort(tzs.begin(), tzs.end());

  uint16_t index = std::find(tzs.begin(), tzs.end(), initial_tz) - tzs.begin();

  if(index < tzs.size()){
    Buttons::BUTTON pulsed;
    std::cout << initial_tz << std::endl;

    //Action builder to avoid repeat lines.
    auto action_builder = [&]() -> std::function<void ()>{
      if(pulsed.symbol == Buttons::UP)
        return [&]() {index = (index + tzs.size() - 1) % tzs.size();};
      else // if(pulsed.symbol == Buttons::DOWN)
        return [&]() {index = (index + 1) % tzs.size();};
    };


    while(true){
      pulsed = Buttons::buttonsQueue.pop();

      //Quit time zone selection
      if(pulsed.symbol == Buttons::CENTER){

        // Write the new time zone in timezone file
        std::ofstream ofs(timezone_file_path);
        ofs << tzs[index] << std::endl;
        ofs.close();

        //Update symlink localtime
        system(("ln -sf " + tzs_directory_path + tzs[index] + " " + localtime_symlink_path).c_str());
        return 0;
      }

      //Ascend or descend through the list of available time zones
      if(pulsed.symbol == Buttons::UP || pulsed.symbol == Buttons::DOWN){
        auto action = action_builder();

        if(pulsed.type == Buttons::SHORT_PULSE){
          action();
          std::cout << tzs[index] << std::endl;
        }
        else if(pulsed.type == Buttons::LONG_PULSE){
          bool pressed = true;

          //Execute action until button release.
          do{
            action();
            std::cout << tzs[index] << std::endl;
            usleep(100000);
            if(Buttons::buttonsQueue.size() > 0){
              Buttons::BUTTON aux_pulsed = Buttons::buttonsQueue.pop();
              pressed = aux_pulsed.symbol != pulsed.symbol || aux_pulsed.type != Buttons::LONG_PULSE_RELEASED;
            }
          }while(pressed);
        }
      }
    }
  }
  return -1;
}


/**
 * @brief This function will handle the selection of a new server IP. To change from one IP byte to another, use
 *        the LEFT and RIGHT joystick buttons. To change the value of a certain IP byte, use the UP and DOWN
 *        joystick buttons.
 *
 * @param[in,out] initial_ip_str   The current IP. Also, when the function finishes, this parameter will maintain
 *                                 the new selected IP.
 *
 * @return 0 if success, -1 if error.
 */
int Configurations::handle_server_ip(std::string *initial_ip_str){

  uint8_t ip_byte_pos = 3, ip_bytes = 4;
  uint32_t initial_ip_bytes;
  uint8_t ip_array[ip_bytes];

  //Transforms to IP array
  inet_pton(AF_INET, initial_ip_str->c_str(), &initial_ip_bytes);
  std::cout << initial_ip_bytes << std::endl;
  ip_array[3] = initial_ip_bytes >> 24;
  ip_array[2] = initial_ip_bytes >> 16;
  ip_array[1] = initial_ip_bytes >> 8;
  ip_array[0] = initial_ip_bytes;


  Buttons::BUTTON pulsed;
  std::cout << *initial_ip_str << std::endl;

  //Action builder to avoid repeat lines.
  auto action_builder = [&]() -> std::function<void ()>{
    if(pulsed.symbol == Buttons::UP)
      return [&]() {ip_array[ip_byte_pos]++;};
    else // if(pulsed.symbol == Buttons::DOWN)
      return [&]() {ip_array[ip_byte_pos]--;};
  };


  while(true){
    pulsed = Buttons::buttonsQueue.pop();

    //Quit time zone selection
    if(pulsed.symbol == Buttons::CENTER){
      std::stringstream sstream;
      sstream << int(ip_array[0]) << "." << int(ip_array[1]) << "." << int(ip_array[2]) << "." << int(ip_array[3]);
      *initial_ip_str = sstream.str();
      return 0;
    }
    if(pulsed.symbol == Buttons::LEFT){
      ip_byte_pos = (ip_byte_pos + ip_bytes - 1) % ip_bytes;
    }
    else if(pulsed.symbol == Buttons::RIGHT){
      ip_byte_pos = (ip_byte_pos + 1) % ip_bytes;
    }

    if(pulsed.symbol == Buttons::UP || pulsed.symbol == Buttons::DOWN){
      auto action = action_builder();
      if(pulsed.type == Buttons::SHORT_PULSE){
        action();
        std::cout << int(ip_array[0]) << "." << int(ip_array[1]) << "." << int(ip_array[2]) << "." << int(ip_array[3]) << std::endl;
      }
      else if(pulsed.type == Buttons::LONG_PULSE){
        bool pressed = true;

        //Execute action until button release.
        do{
          action();
          std::cout << int(ip_array[0]) << "." << int(ip_array[1]) << "." << int(ip_array[2]) << "." << int(ip_array[3]) << std::endl;
          usleep(50000);
          if(Buttons::buttonsQueue.size() > 0){
            Buttons::BUTTON aux_pulsed = Buttons::buttonsQueue.pop();
            pressed = aux_pulsed.symbol != pulsed.symbol || aux_pulsed.type != Buttons::LONG_PULSE_RELEASED;
          }
        }while(pressed);
      }
    }
  }
  return -1;
}


/**
 * @brief This function will handle the selection of a new server port. To change from one digit to another, use
 *        the LEFT and RIGHT joystick buttons. To change the value of a certain digit, use the UP and DOWN
 *        joystick buttons.
 *
 * @param[in,out] port   The current TCP port. Also, when the function finishes, this parameter will maintain
 *                       the new selected port.
 *
 * @return 0 if success, -1 if error.
 */
int Configurations::handle_server_port(int *port){

  int max_port = 65535, factor = 0, factor_range = 5;

  Buttons::BUTTON pulsed;
  std::cout << *port << std::endl;

  //Action builder to avoid repeat lines.
  auto action_builder = [&]() -> std::function<void ()>{
    if(pulsed.symbol == Buttons::UP)
      return [&]() {
        int digit = int(*port / std::pow(10, factor)) % 10;
        int value = *port + ((digit + 1) % 10 - digit) * std::pow(10, factor);
        *port = std::min(value, max_port);
      };
    else // if(pulsed.symbol == Buttons::DOWN)
      return [&]() {
        int digit = int(*port / std::pow(10, factor)) % 10;
        int value = *port + ((digit + 9) % 10 - digit) * std::pow(10, factor);
        *port = std::min(value, max_port);
      };
  };


  while(true){
    pulsed = Buttons::buttonsQueue.pop();

    //Quit time zone selection
    if(pulsed.symbol == Buttons::CENTER){
      return 0;
    }
    if(pulsed.symbol == Buttons::LEFT){
      factor = (factor + 1) % factor_range;
    }
    else if(pulsed.symbol == Buttons::RIGHT){
      factor = (factor + factor_range - 1) % factor_range;
    }
    else if(pulsed.symbol == Buttons::UP || pulsed.symbol == Buttons::DOWN){
      auto action = action_builder();
      if(pulsed.type == Buttons::SHORT_PULSE){
        action();
        std::cout << *port << std::endl;
      }
      else if(pulsed.type == Buttons::LONG_PULSE){
        bool pressed = true;

        //Execute action until button release.
        do{
          action();
          std::cout << *port << std::endl;
          usleep(50000);
          if(Buttons::buttonsQueue.size() > 0){
            Buttons::BUTTON aux_pulsed = Buttons::buttonsQueue.pop();
            pressed = aux_pulsed.symbol != pulsed.symbol || aux_pulsed.type != Buttons::LONG_PULSE_RELEASED;
          }
        }while(pressed);
      }
    }
  }
  return -1;
}


/**
 * @brief This function will handle the selection of the offset of the temperature sensor. This will be useful to calibrate
 *        the sensor. The format of this offset will be truncated to one integer digit and two decimals. To change from one
 *        digit to another, use the LEFT and RIGHT joystick buttons. To change the value of a certain digit, use the UP and
 *        DOWN joystick buttons.
 *
 * @param[in,out] offset   The current temperature offset. Also, when the function finishes, this parameter will maintain
 *                         the new selected offset.
 *
 * @return 0 if success, -1 if error.
 */
int Configurations::handle_temp_offset(float *offset){

  int factor = 0, factor_range = 3, offset_int;

  offset_int = int(std::ceil(*offset * 100.0)) % 1000; //Integer version to work only with integers, so the calculus are
                                                       //easier. Also, adapt the original offset to format "0.00" with two
                                                       //decimals and one integer digit.

  Buttons::BUTTON pulsed;
  std::cout << float(offset_int)/100.0 << std::endl;

  //Action builder to avoid repeat lines.
  auto action_builder = [&]() -> std::function<void ()>{
    if(pulsed.symbol == Buttons::UP)
      return [&]() {
        int digit = int(offset_int / std::pow(10, factor)) % 10;
        offset_int += ((digit + 1) % 10 - digit) * std::pow(10, factor);
      };
    else // if(pulsed.symbol == Buttons::DOWN)
      return [&]() {
        int digit = int(offset_int / std::pow(10, factor)) % 10;
        offset_int += ((digit + 9) % 10 - digit) * std::pow(10, factor);
      };
  };


  while(true){
    pulsed = Buttons::buttonsQueue.pop();

    //Quit time zone selection
    if(pulsed.symbol == Buttons::CENTER){
      *offset = float(offset_int)/100.0;
      return 0;
    }
    if(pulsed.symbol == Buttons::LEFT){
      factor = (factor + 1) % factor_range;
    }
    else if(pulsed.symbol == Buttons::RIGHT){
      factor = (factor + factor_range - 1) % factor_range;
    }
    else if(pulsed.symbol == Buttons::UP || pulsed.symbol == Buttons::DOWN){
      auto action = action_builder();
      if(pulsed.type == Buttons::SHORT_PULSE){
        action();
        std::cout << float(offset_int)/100.0 << std::endl;
      }
      else if(pulsed.type == Buttons::LONG_PULSE){
        bool pressed = true;

        //Execute action until button release.
        do{
          action();
          std::cout << float(offset_int)/100.0 << std::endl;
          usleep(50000);
          if(Buttons::buttonsQueue.size() > 0){
            Buttons::BUTTON aux_pulsed = Buttons::buttonsQueue.pop();
            pressed = aux_pulsed.symbol != pulsed.symbol || aux_pulsed.type != Buttons::LONG_PULSE_RELEASED;
          }
        }while(pressed);
      }
    }
  }
  return -1;
}

