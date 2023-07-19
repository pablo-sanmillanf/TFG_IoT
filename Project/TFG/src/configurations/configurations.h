/**
  ******************************************************************************
  * @file   configurations.h
  * @author Pablo San Millán Fierro (pablo.sanmillanf@alumnos.upm.es)
  * @brief  Configuration functions handler Module Header. Requires the button
  *         thread to be in operation for this module to work.
  *
  * @note   End-of-degree work.
  *         This module handles all the configuration functions that will have
  *         the final app.
  ******************************************************************************
*/
#ifndef __CONFIGURATIONS_H__
#define __CONFIGURATIONS_H__


/* Includes ------------------------------------------------------------------*/
#include <string>

namespace Configurations{

/* Exported types ------------------------------------------------------------*/
/* Exported variables --------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported Functions --------------------------------------------------------*/


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
int handle_time_zone(std::string timezone_file_path = "/etc/timezone",
                     std::string localtime_symlink_path = "/etc/localtime",
                     std::string tzs_directory_path = "/usr/share/zoneinfo/posix/");


/**
 * @brief This function will handle the selection of a MQTT token for the thingsboard device. To change from
 *        one character to another, use the LEFT and RIGHT joystick buttons. To change the value of a certain
 *        character, use the UP and DOWN joystick buttons. To add a new character, use the RIGTH joystick button
 *        on the rightmost part of the string. To delete the last character, set it to a space and use the LEFT
 *        joystick button.
 *
 * @param[in,out] mqtt_token   The current token. Also, when the function finishes, this parameter will maintain
 *                             the new selected token.
 *
 * @return 0 if success, -1 if error.
 */
int handle_server_mqtt_token(std::string *mqtt_token);

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

int handle_server_ip(std::string *initial_ip_str);


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
int handle_server_port(int *port);

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
int handle_temp_offset(float *offset);

}

#endif /* __CONFIGURATIONS_H__ */
