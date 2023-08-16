/**
  ******************************************************************************
  * @file   icon_manager.h
  * @author Pablo San Millán Fierro (pablo.sanmillanf@alumnos.upm.es)
  * @brief  Application Icon Module Header.
  *
  * @note   End-of-degree work.
  *         This module store all the data and functions related to the LCD
  *         application icons.
  ******************************************************************************
*/

#ifndef __ICON_MANAGER_H__
#define __ICON_MANAGER_H__


/* Includes ------------------------------------------------------------------*/
#include <cstdint>
#include <vector>


namespace Icons{

/* Exported types ------------------------------------------------------------*/
/* Exported variables --------------------------------------------------------*/
extern std::vector<uint16_t>  unchecked_icon;
extern std::vector<uint16_t>  checked_icon;

extern std::vector<uint16_t> menu_square_icon;
extern std::vector<uint16_t> menu_back_icon;
extern std::vector<uint16_t> menu_ok_icon;
extern std::vector<uint16_t> menu_up_icon;
extern std::vector<uint16_t> menu_down_icon;
extern std::vector<uint16_t> save_conf_icon;
extern std::vector<uint16_t> yes_no_icon;

extern std::vector<uint16_t> edit_menu_square;
extern std::vector<uint16_t> edit_menu_left;
extern std::vector<uint16_t> edit_menu_right;
extern std::vector<uint16_t> edit_menu_up;
extern std::vector<uint16_t> edit_menu_down;

extern std::vector<uint16_t> clock_icon;
extern std::vector<uint16_t> pressure_icon;
extern std::vector<uint16_t> altitude_icon;
extern std::vector<uint16_t> main_conf_icon;
extern std::vector<uint16_t> celsius_icon;
extern std::vector<uint16_t> percentage_icon;

extern const int checked_unchecked_icon_width;
extern const int main_menu_metrics;
extern const int yes_no_icon_width;
extern const int edit_menu_square_width;
extern const int edit_menu_left_width;
extern const int edit_menu_right_width;
extern const int edit_menu_up_width;
extern const int edit_menu_down_width;

extern const int clock_icon_width;
extern const int temp_icon_width;
extern const int hum_icon_width;
extern const int pressure_icon_width;
extern const int altitude_icon_width;
extern const int iaq_icon_width;

extern const int main_conf_icon_width;
extern const int celsius_icon_width;
extern const int percentage_icon_width;
extern const int main_menu_indicator_width;

/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported Functions --------------------------------------------------------*/

/**
 * @brief Obtain the temperature icon with its bar icon filled "bar_percentage" percentage.
 *
 * @param[in] bar_percentage The bar percentage. Percentages greater than 100 will be truncated to 100.
 *
 * @return The icon with the bar filled "bar_percentage" percentage.
 */
std::vector<uint16_t> get_temperature_icon(uint8_t bar_percentage);

/**
 * @brief Obtain the humidity icon with its bar icon filled "bar_percentage" percentage.
 *
 * @param[in] bar_percentage The bar percentage. Percentages greater than 100 will be truncated to 100.
 *
 * @return The icon with the bar filled "bar_percentage" percentage.
 */
std::vector<uint16_t> get_humidity_icon(uint8_t bar_percentage);

/**
 * @brief Obtain the IAQ icon with its bar icon filled "bar_percentage" percentage.
 *
 * @param[in] bar_percentage The bar percentage. Percentages greater than 100 will be truncated to 100.
 *
 * @return The icon with the bar filled "bar_percentage" percentage.
 */
std::vector<uint16_t> get_iaq_icon(uint16_t bar_percentage);


/**
 * @brief Obtain the icon that indicates in which menu the user is. The icon indicator will have 6 circles and one
 *        of them will be selected. This is an example with "1" selected:
 *   ______    ______    ______    ______    ______    ______
 *  /      \  /  __  \  /      \  /      \  /      \  /      \
 *  |      |  | |__| |  |      |  |      |  |      |  |      |
 *  \______/  \______/  \______/  \______/  \______/  \______/
 *
 * @param[in] selected Number from 1 to 6 that indicates the selected circle.
 *
 * @return The menu indicator icon.
 */
std::vector<uint16_t> get_main_menu_indicator_icon(uint8_t selected);
}

#endif /* __ICON_MANAGER_H__ */
