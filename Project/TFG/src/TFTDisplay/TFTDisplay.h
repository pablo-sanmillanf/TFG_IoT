/*
 * TFTDisplay.h
 *
 *  Created on: Aug 6, 2023
 *      Author: ubuntu
 */

#ifndef __TFTDISPLAY_H__
#define __TFTDISPLAY_H__


#include <string>
#include "display_driver/display_driver.h"
#include "icon_manager/icon_manager.h"

namespace TFTDisplay{

extern uint8_t background_color[2];
extern uint8_t main_color[2];

extern uint8_t range_0_25_temp_color[2];
extern uint8_t range_25_50_temp_color[2];
extern uint8_t range_50_75_temp_color[2];
extern uint8_t range_75_100_temp_color[2];

extern uint8_t range_0_25_hum_color[2];
extern uint8_t range_25_50_hum_color[2];
extern uint8_t range_50_75_hum_color[2];
extern uint8_t range_75_100_hum_color[2];

extern uint8_t range_0_50_iaq_color[2];
extern uint8_t range_51_100_iaq_color[2];
extern uint8_t range_101_150_iaq_color[2];
extern uint8_t range_151_200_iaq_color[2];
extern uint8_t range_201_250_iaq_color[2];
extern uint8_t range_251_350_iaq_color[2];
extern uint8_t range_351_plus_iaq_color[2];

void print_main_menu_temp(bool full_refresh, float temp, uint8_t percentage);
void print_main_menu_hum(bool full_refresh, float hum);
void print_main_menu_press(bool full_refresh, float press);
void print_main_menu_alt(bool full_refresh, float alt);
void print_main_menu_iaq(bool full_refresh, float iaq);

void print_conf_menu(bool full_refresh, bool menus_refresh, std::vector<std::string> menus,
                     int selected, int prev_selected);
void print_editable_text(bool full_refresh, bool letters_refresh, std::string title,
                         std::string editable_text, int selected_char, int prev_selected_char);

}

#endif /* __TFTDISPLAY_H__ */
