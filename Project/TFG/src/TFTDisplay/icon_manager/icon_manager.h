/*
 * icon_manager.h
 *
 *  BY NOW, THIS MODULE IS IN ALPHA VERSION
 */

#ifndef __ICON_MANAGER_H__
#define __ICON_MANAGER_H__

#include <cstdint>
#include <vector>

extern std::vector<uint16_t>  unchecked_icon;
extern std::vector<uint16_t>  checked_icon;

extern std::vector<uint16_t> menu_square_icon;
extern std::vector<uint16_t> menu_back_icon;
extern std::vector<uint16_t> menu_ok_icon;
extern std::vector<uint16_t> menu_up_icon;
extern std::vector<uint16_t> menu_down_icon;
extern std::vector<uint16_t> yes_no_menu_icon;

extern std::vector<uint16_t> edit_menu_square;
extern std::vector<uint16_t> edit_menu_left;
extern std::vector<uint16_t> edit_menu_right;
extern std::vector<uint16_t> edit_menu_up;
extern std::vector<uint16_t> edit_menu_down;

const int edit_menu_square_width = 188;
const int edit_menu_left_width = 25;
const int edit_menu_right_width = 25;
const int edit_menu_up_width = 36;
const int edit_menu_down_width = 36;

extern std::vector<uint16_t> pressure_icon;
extern std::vector<uint16_t> altitude_icon;
extern std::vector<uint16_t> main_conf_icon;
extern std::vector<uint16_t> celsius_icon;
extern std::vector<uint16_t> percentage_icon;

const int pressure_icon_width = 128;
const int altitude_icon_width = 128;
const int main_conf_icon_width = 120;
const int celsius_icon_width = 90;
const int percentage_icon_width = 70;
const int main_menu_indicator_width = 80;


uint16_t* get_temperature_icon(unsigned int *size, unsigned int *width, uint8_t bar_percentage);
uint16_t* get_humidity_icon(unsigned int *size, unsigned int *width, uint8_t bar_percentage);
uint16_t* get_iaq_icon(unsigned int *size, unsigned int *width, uint16_t bar_percentage);

std::vector<uint16_t> get_main_menu_indicator_icon(uint8_t selected);

#endif /* __ICON_MANAGER_H__ */
