/*
 * TFTDisplay.cpp
 *
 *  Created on: Aug 6, 2023
 *      Author: ubuntu
 */

#include "TFTDisplay.h"
#include <iomanip>
#include <sstream>
#include <algorithm>
#include <chrono>
#include <thread>
#include <iostream>

uint8_t TFTDisplay::background_color[] = {0x00, 0x00};
uint8_t TFTDisplay::main_color[] = {0x84, 0x10};

uint8_t TFTDisplay::range_0_25_temp_color[] = {0x04, 0x1F};
uint8_t TFTDisplay::range_25_50_temp_color[] = {0x07, 0xE8};
uint8_t TFTDisplay::range_50_75_temp_color[] = {0xFC, 0x00};
uint8_t TFTDisplay::range_75_100_temp_color[] = {0xF8, 0x00};

uint8_t TFTDisplay::range_0_25_hum_color[] = {0xE0, 0x00};
uint8_t TFTDisplay::range_25_50_hum_color[] = {0x04, 0x00};
uint8_t TFTDisplay::range_50_75_hum_color[] = {0x06, 0xDB};
uint8_t TFTDisplay::range_75_100_hum_color[] = {0x29, 0x7F};

uint8_t TFTDisplay::range_0_50_iaq_color[] = {0x07, 0xE0};
uint8_t TFTDisplay::range_51_100_iaq_color[] = {0x4F, 0xEF};
uint8_t TFTDisplay::range_101_150_iaq_color[] = {0xFF, 0xE0};
uint8_t TFTDisplay::range_151_200_iaq_color[] = {0xFC, 0x00};
uint8_t TFTDisplay::range_201_250_iaq_color[] = {0xF8, 0x00};
uint8_t TFTDisplay::range_251_350_iaq_color[] = {0x98, 0x13};
uint8_t TFTDisplay::range_351_plus_iaq_color[] = {0x51, 0x40};

static const int max_conf_menus = 4;
static const int max_chars_edit_menu = 5;
static const int conf_menu_y_offset = 68;


void TFTDisplay::print_main_menu_temp(bool full_refresh, float temp, uint8_t percentage){

  Display_driver::init_display();

  if(full_refresh){
    std::vector<uint16_t> icon = get_main_menu_indicator_icon(1);
    Display_driver::draw_icon_col(
        80, 16, main_menu_indicator_width, icon.data(), icon.size(), background_color, main_color);
    Display_driver::draw_icon_col(
        60, 198, main_conf_icon_width, main_conf_icon.data(), main_conf_icon.size(), background_color, main_color);
  }

  unsigned int size, width;
  uint16_t *temp_icon = get_temperature_icon(&size, &width, percentage);

  std::stringstream stream;
  stream << std::fixed << std::setprecision(2) << temp;
  std::string temp_string = stream.str();
  std::replace(temp_string.begin(), temp_string.end(), '.', ',');

  if(percentage < 25){
    Display_driver::draw_icon_col(25, 60, width, temp_icon, size, background_color, range_0_25_temp_color);
    Display_driver::draw_icon_col(
        110, 115, celsius_icon_width, celsius_icon.data(), celsius_icon.size(), background_color, range_0_25_temp_color);
    Display_driver::write_fast_string(
        100, 60,(char*) temp_string.c_str(), range_0_25_temp_color, background_color, FreeSansBold24pt7b);
  }
  else if(percentage < 50){
    Display_driver::draw_icon_col(25, 60, width, temp_icon, size, background_color, range_25_50_temp_color);
    Display_driver::draw_icon_col(
        110, 115, celsius_icon_width, celsius_icon.data(), celsius_icon.size(), background_color, range_25_50_temp_color);
    Display_driver::write_fast_string(
        100, 60,(char*) temp_string.c_str(), range_25_50_temp_color, background_color, FreeSansBold24pt7b);
  }
  else if(percentage < 75){
    Display_driver::draw_icon_col(25, 60, width, temp_icon, size, background_color, range_50_75_temp_color);
    Display_driver::draw_icon_col(
        110, 115, celsius_icon_width, celsius_icon.data(), celsius_icon.size(), background_color, range_50_75_temp_color);
    Display_driver::write_fast_string(
        100, 60,(char*) temp_string.c_str(), range_50_75_temp_color, background_color, FreeSansBold24pt7b);
  }
  else{
    Display_driver::draw_icon_col(25, 60, width, temp_icon, size, background_color, range_75_100_temp_color);
    Display_driver::draw_icon_col(
        110, 115, celsius_icon_width, celsius_icon.data(), celsius_icon.size(), background_color, range_75_100_temp_color);
    Display_driver::write_fast_string(
        100, 60,(char*) temp_string.c_str(), range_75_100_temp_color, background_color, FreeSansBold24pt7b);
  }


  Display_driver::uninit();
}



void TFTDisplay::print_main_menu_hum(bool full_refresh, float hum){

  Display_driver::init_display();

  int precision, percentage = hum;

  if(hum >= 100)
    precision = 1;
  else
    precision = 2;

  if(full_refresh){
    std::vector<uint16_t> icon = get_main_menu_indicator_icon(2);
    Display_driver::draw_icon_col(
        80, 16, main_menu_indicator_width, icon.data(), icon.size(), background_color, main_color);
    Display_driver::draw_icon_col(
        60, 198, main_conf_icon_width, main_conf_icon.data(), main_conf_icon.size(), background_color, main_color);
  }

  unsigned int size, width;
  uint16_t *hum_icon = get_humidity_icon(&size, &width, percentage);

  std::stringstream stream;
  stream << std::fixed << std::setprecision(precision) << hum;
  std::string hum_string = stream.str();
  std::replace(hum_string.begin(), hum_string.end(), '.', ',');

  if(percentage < 25){
    Display_driver::draw_icon_col(25, 60, width, hum_icon, size, background_color, range_0_25_hum_color);
    Display_driver::draw_icon_col(
        130, 115, percentage_icon_width, percentage_icon.data(), percentage_icon.size(), background_color, range_0_25_hum_color);
    Display_driver::write_fast_string(
        100, 60,(char*) hum_string.c_str(), range_0_25_hum_color, background_color, FreeSansBold24pt7b);
  }
  else if(percentage < 50){
    Display_driver::draw_icon_col(25, 60, width, hum_icon, size, background_color, range_25_50_hum_color);
    Display_driver::draw_icon_col(
        130, 115, percentage_icon_width, percentage_icon.data(), percentage_icon.size(), background_color, range_25_50_hum_color);
    Display_driver::write_fast_string(
        100, 60,(char*) hum_string.c_str(), range_25_50_hum_color, background_color, FreeSansBold24pt7b);
  }
  else if(percentage < 75){
    Display_driver::draw_icon_col(25, 60, width, hum_icon, size, background_color, range_50_75_hum_color);
    Display_driver::draw_icon_col(
        130, 115, percentage_icon_width, percentage_icon.data(), percentage_icon.size(), background_color, range_50_75_hum_color);
    Display_driver::write_fast_string(
        100, 60,(char*) hum_string.c_str(), range_50_75_hum_color, background_color, FreeSansBold24pt7b);
  }
  else{
    Display_driver::draw_icon_col(25, 60, width, hum_icon, size, background_color, range_75_100_hum_color);
    Display_driver::draw_icon_col(
        130, 115, percentage_icon_width, percentage_icon.data(), percentage_icon.size(), background_color, range_75_100_hum_color);
    Display_driver::write_fast_string(
        100, 60,(char*) hum_string.c_str(), range_75_100_hum_color, background_color, FreeSansBold24pt7b);
  }

  Display_driver::uninit();
}



void TFTDisplay::print_main_menu_press(bool full_refresh, float press){

  Display_driver::init_display();

  if(full_refresh){
    std::vector<uint16_t> icon = get_main_menu_indicator_icon(3);
    Display_driver::draw_icon_col(
        80, 16, main_menu_indicator_width, icon.data(), icon.size(), background_color, main_color);
    Display_driver::draw_icon_col(
        60, 198, main_conf_icon_width, main_conf_icon.data(), main_conf_icon.size(), background_color, main_color);
    Display_driver::draw_icon_col(
        56, 45, pressure_icon_width, pressure_icon.data(), pressure_icon.size(), background_color, main_color);
  }

  std::stringstream stream;
  stream << std::fixed << std::setprecision(2) << press << " Pa";
  std::string press_string = stream.str();
  std::replace(press_string.begin(), press_string.end(), '.', ',');

  Display_driver::write_fast_string_centered(
      135,(char*) press_string.c_str(), main_color, background_color, FreeSansBold18pt7b);

  Display_driver::uninit();
}



void TFTDisplay::print_main_menu_alt(bool full_refresh, float alt){

  Display_driver::init_display();

  if(full_refresh){
    std::vector<uint16_t> icon = get_main_menu_indicator_icon(4);
    Display_driver::draw_icon_col(
        80, 16, main_menu_indicator_width, icon.data(), icon.size(), background_color, main_color);
    Display_driver::draw_icon_col(
        60, 198, main_conf_icon_width, main_conf_icon.data(), main_conf_icon.size(), background_color, main_color);
    Display_driver::draw_icon_col(
        56, 45, altitude_icon_width, altitude_icon.data(), altitude_icon.size(), background_color, main_color);
  }

  std::stringstream stream;
  stream << std::fixed << std::setprecision(2) << alt << " m";
  std::string temp_string = stream.str();
  std::replace(temp_string.begin(), temp_string.end(), '.', ',');

  Display_driver::write_fast_string_centered(
      150,(char*) temp_string.c_str(), main_color, background_color, FreeSansBold18pt7b);

  Display_driver::uninit();
}


void TFTDisplay::print_main_menu_iaq(bool full_refresh, float iaq){

  Display_driver::init_display();

  if(full_refresh){
    std::vector<uint16_t> icon = get_main_menu_indicator_icon(5);
    Display_driver::draw_icon_col(
        80, 16, main_menu_indicator_width, icon.data(), icon.size(), background_color, main_color);
    Display_driver::draw_icon_col(
        60, 198, main_conf_icon_width, main_conf_icon.data(), main_conf_icon.size(), background_color, main_color);
  }

  unsigned int size, width;
  uint16_t *iaq_icon = get_iaq_icon(&size, &width, iaq);

  std::stringstream stream;
  stream << std::fixed << std::setprecision(2) << iaq;
  std::string iaq_string = stream.str();
  std::replace(iaq_string.begin(), iaq_string.end(), '.', ',');

  if(iaq < 51){
    Display_driver::draw_icon_col(120 - width/2, 45, width, iaq_icon, size, background_color, range_0_50_iaq_color);
    Display_driver::write_fast_string_centered(
        150,(char*) iaq_string.c_str(), range_0_50_iaq_color, background_color, FreeSansBold18pt7b);
  }
  else if(iaq < 101){
    Display_driver::draw_icon_col(120 - width/2, 45, width, iaq_icon, size, background_color, range_51_100_iaq_color);
    Display_driver::write_fast_string_centered(
        150,(char*) iaq_string.c_str(), range_51_100_iaq_color, background_color, FreeSansBold18pt7b);
  }
  else if(iaq < 151){
    Display_driver::draw_icon_col(120 - width/2, 45, width, iaq_icon, size, background_color, range_101_150_iaq_color);
    Display_driver::write_fast_string_centered(
        150,(char*) iaq_string.c_str(), range_101_150_iaq_color, background_color, FreeSansBold18pt7b);
  }
  else if(iaq < 201){
    Display_driver::draw_icon_col(120 - width/2, 45, width, iaq_icon, size, background_color, range_151_200_iaq_color);
    Display_driver::write_fast_string_centered(
        150,(char*) iaq_string.c_str(), range_151_200_iaq_color, background_color, FreeSansBold18pt7b);
  }
  else if(iaq < 251){
    Display_driver::draw_icon_col(120 - width/2, 45, width, iaq_icon, size, background_color, range_201_250_iaq_color);
    Display_driver::write_fast_string_centered(
        150,(char*) iaq_string.c_str(), range_201_250_iaq_color, background_color, FreeSansBold18pt7b);
  }
  else if(iaq < 351){
    Display_driver::draw_icon_col(120 - width/2, 45, width, iaq_icon, size, background_color, range_251_350_iaq_color);
    Display_driver::write_fast_string_centered(
        150,(char*) iaq_string.c_str(), range_251_350_iaq_color, background_color, FreeSansBold18pt7b);
  }
  else{
    Display_driver::draw_icon_col(120 - width/2, 45, width, iaq_icon, size, background_color, range_351_plus_iaq_color);
    Display_driver::write_fast_string_centered(
        150,(char*) iaq_string.c_str(), range_351_plus_iaq_color, background_color, FreeSansBold18pt7b);
  }

  Display_driver::uninit();
}


void TFTDisplay::print_conf_menu(bool full_refresh, bool menus_refresh, std::vector<std::string> menus,
                                 int selected, int prev_selected){


  if(full_refresh){
    Display_driver::draw_icon_col(35, 35, 170, menu_square_icon.data(), menu_square_icon.size(), background_color, main_color);
    Display_driver::draw_icon_col(2, 60, 30, menu_back_icon.data(), menu_back_icon.size(), background_color, main_color);
    Display_driver::draw_icon_col(208, 60, 30, menu_ok_icon.data(), menu_ok_icon.size(), background_color, main_color);
    Display_driver::draw_icon_col(60, 2, 120, menu_up_icon.data(), menu_up_icon.size(), background_color, main_color);
    Display_driver::draw_icon_col(60, 208, 120, menu_down_icon.data(), menu_down_icon.size(), background_color, main_color);
  }

  int menus_count = std::min(max_conf_menus, int(menus.size()));

  if(full_refresh || menus_refresh){
    for(int i = 0; i < menus_count; i++){
      if(selected != i){
        Display_driver::write_fast_string(
              45,
              conf_menu_y_offset + 40 * i,
              (char*)menus[i].c_str(),
              main_color,
              background_color,
              FreeSansBold12pt7b
            );
      }
      else{
        Display_driver::reset_region(44, 44 + i * 39, 196, 79 + i * 39, main_color);
        Display_driver::write_fast_string(
              45,
              conf_menu_y_offset + 40 * i,
              (char*)menus[i].c_str(),
              background_color,
              main_color,
              FreeSansBold12pt7b
            );
      }
    }
  }
  else{
    if(prev_selected >= 0 && prev_selected < menus_count){
      Display_driver::reset_region(44, 44 + prev_selected * 39, 196, 79 + prev_selected * 39, background_color);
      Display_driver::write_fast_string(
            45,
            conf_menu_y_offset + 40 * prev_selected,
            (char*)menus[prev_selected].c_str(),
            main_color,
            background_color,
            FreeSansBold12pt7b
          );
    }

    Display_driver::reset_region(44, 44 + selected * 39, 196, 79 + selected * 39, main_color);
    Display_driver::write_fast_string(
          45,
          conf_menu_y_offset + 40 * selected,
          (char*)menus[selected].c_str(),
          background_color,
          main_color,
          FreeSansBold12pt7b
        );
  }
}


void TFTDisplay::print_editable_text(bool full_refresh, bool letters_refresh, std::string title,
                                     std::string editable_text, int selected_char, int prev_selected_char){

  if(full_refresh){
    Display_driver::write_fast_string_centered(
        125,(char*) title.c_str(), main_color, background_color, FreeSansBold18pt7b);

    using namespace std::chrono_literals; // ns, us, ms, s, h, etc.
    std::this_thread::sleep_for(1s);
    Display_driver::reset_region(0, 100, 240, 140, background_color);


    Display_driver::draw_icon_col(1, 98, edit_menu_left_width, edit_menu_left.data(), edit_menu_left.size(), background_color, main_color);
    Display_driver::draw_icon_col(214, 98, edit_menu_right_width, edit_menu_right.data(), edit_menu_right.size(), background_color, main_color);
    Display_driver::draw_icon_col(26, 98, edit_menu_square_width, edit_menu_square.data(), edit_menu_square.size(), background_color, main_color);
  }

  if(full_refresh && letters_refresh){
    for(int i = 0; i < max_chars_edit_menu; i++){
      std::string char_string = editable_text.substr(i, 1);
      GFXglyph glyph = FreeSansBold18pt7b.glyph[char(char_string[0]) - FreeSansBold18pt7b.first];

      if(selected_char == i){
        Display_driver::reset_region(28 + 37*i, 100, 64 + 37*i, 140, main_color);

        Display_driver::write_fast_string(
            46 + 37*i - glyph.width/2 - glyph.xOffset, 130,
            (char *)char_string.c_str(), background_color, main_color, FreeSansBold18pt7b);

        Display_driver::draw_icon_col(
            28 + 37*i, 70, edit_menu_up_width, edit_menu_up.data(), edit_menu_up.size(),
            background_color, main_color);
        Display_driver::draw_icon_col(
            28 + 37*i, 146, edit_menu_down_width, edit_menu_down.data(), edit_menu_down.size(),
            background_color, main_color);
      }
      else{
        Display_driver::write_fast_string(
            46 + 37*i - glyph.width/2 - glyph.xOffset, 130,
            (char *)char_string.c_str(), main_color, background_color, FreeSansBold18pt7b);
      }
    }
  }
  else{
    std::string sel_char = editable_text.substr(selected_char, 1);
    GFXglyph glyph = FreeSansBold18pt7b.glyph[char(sel_char[0]) - FreeSansBold18pt7b.first];

    Display_driver::reset_region(28 + 37*selected_char, 100, 64 + 37*selected_char, 140, main_color);

    Display_driver::write_fast_string(
        46 + 37*selected_char - glyph.width/2 - glyph.xOffset, 130,
        (char *)sel_char.c_str(), background_color, main_color, FreeSansBold18pt7b);

    Display_driver::draw_icon_col(
        28 + 37*selected_char, 70, edit_menu_up_width, edit_menu_up.data(), edit_menu_up.size(),
        background_color, main_color);
    Display_driver::draw_icon_col(
        28 + 37*selected_char, 146, edit_menu_down_width, edit_menu_down.data(), edit_menu_down.size(),
        background_color, main_color);

    if(prev_selected_char >= 0 && prev_selected_char < max_chars_edit_menu){
      std::string prev_sel_char = editable_text.substr(prev_selected_char, 1);
      GFXglyph glyph = FreeSansBold18pt7b.glyph[char(prev_sel_char[0]) - FreeSansBold18pt7b.first];

      Display_driver::reset_region(28 + 37*prev_selected_char, 100, 64 + 37*prev_selected_char, 140, background_color);
      Display_driver::reset_region(28 + 37*prev_selected_char, 70, 64 + 37*prev_selected_char, 94, background_color);
      Display_driver::reset_region(28 + 37*prev_selected_char, 146, 64 + 37*prev_selected_char, 170, background_color);
      Display_driver::write_fast_string(
          46 + 37*prev_selected_char - glyph.width/2 - glyph.xOffset, 130,
          (char *)prev_sel_char.c_str(), main_color, background_color, FreeSansBold18pt7b);
    }
  }
}

/*
void TFTDisplay::print_multichoice_text(bool full_refresh, bool bytes_refresh, std::string title, std::string text){

  if(full_refresh){
    Display_driver::write_fast_string_centered(
        125,(char*) title.c_str(), main_color, background_color, FreeSansBold18pt7b);

    using namespace std::chrono_literals; // ns, us, ms, s, h, etc.
    std::this_thread::sleep_for(1s);
    Display_driver::reset_region(0, 100, 240, 140, background_color);


    Display_driver::draw_icon_col(1, 98, edit_menu_left_width, edit_menu_left.data(), edit_menu_left.size(), background_color, main_color);
    Display_driver::draw_icon_col(214, 98, edit_menu_right_width, edit_menu_right.data(), edit_menu_right.size(), background_color, main_color);
    Display_driver::draw_icon_col(26, 98, edit_menu_square_width, edit_menu_square.data(), edit_menu_square.size(), background_color, main_color);
  }

  if(full_refresh && letters_refresh){
    for(int i = 0; i < max_chars_edit_menu; i++){
      std::string char_string = editable_text.substr(i, 1);
      GFXglyph glyph = FreeSansBold18pt7b.glyph[char(char_string[0]) - FreeSansBold18pt7b.first];

      if(selected_char == i){
        Display_driver::reset_region(28 + 37*i, 100, 64 + 37*i, 140, main_color);

        Display_driver::write_fast_string(
            46 + 37*i - glyph.width/2 - glyph.xOffset, 130,
            (char *)char_string.c_str(), background_color, main_color, FreeSansBold18pt7b);

        Display_driver::draw_icon_col(
            28 + 37*i, 70, edit_menu_up_width, edit_menu_up.data(), edit_menu_up.size(),
            background_color, main_color);
        Display_driver::draw_icon_col(
            28 + 37*i, 146, edit_menu_down_width, edit_menu_down.data(), edit_menu_down.size(),
            background_color, main_color);
      }
      else{
        Display_driver::write_fast_string(
            46 + 37*i - glyph.width/2 - glyph.xOffset, 130,
            (char *)char_string.c_str(), main_color, background_color, FreeSansBold18pt7b);
      }
    }
  }
  else{
    std::string sel_char = editable_text.substr(selected_char, 1);
    GFXglyph glyph = FreeSansBold18pt7b.glyph[char(sel_char[0]) - FreeSansBold18pt7b.first];

    Display_driver::reset_region(28 + 37*selected_char, 100, 64 + 37*selected_char, 140, main_color);

    Display_driver::write_fast_string(
        46 + 37*selected_char - glyph.width/2 - glyph.xOffset, 130,
        (char *)sel_char.c_str(), background_color, main_color, FreeSansBold18pt7b);

    Display_driver::draw_icon_col(
        28 + 37*selected_char, 70, edit_menu_up_width, edit_menu_up.data(), edit_menu_up.size(),
        background_color, main_color);
    Display_driver::draw_icon_col(
        28 + 37*selected_char, 146, edit_menu_down_width, edit_menu_down.data(), edit_menu_down.size(),
        background_color, main_color);

    if(prev_selected_char >= 0 && prev_selected_char < max_chars_edit_menu){
      std::string prev_sel_char = editable_text.substr(prev_selected_char, 1);
      GFXglyph glyph = FreeSansBold18pt7b.glyph[char(prev_sel_char[0]) - FreeSansBold18pt7b.first];

      Display_driver::reset_region(28 + 37*prev_selected_char, 100, 64 + 37*prev_selected_char, 140, background_color);
      Display_driver::reset_region(28 + 37*prev_selected_char, 70, 64 + 37*prev_selected_char, 94, background_color);
      Display_driver::reset_region(28 + 37*prev_selected_char, 146, 64 + 37*prev_selected_char, 170, background_color);
      Display_driver::write_fast_string(
          46 + 37*prev_selected_char - glyph.width/2 - glyph.xOffset, 130,
          (char *)prev_sel_char.c_str(), main_color, background_color, FreeSansBold18pt7b);
    }
  }
}
*/
