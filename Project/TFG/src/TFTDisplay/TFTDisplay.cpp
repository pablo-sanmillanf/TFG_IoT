/**
  ******************************************************************************
  * @file   TFTDisplay.cpp
  * @author Pablo San Millán Fierro (pablo.sanmillanf@alumnos.upm.es)
  * @brief  LCD printer handler Module.
  *
  * @note   End-of-degree work.
  *         This module is the top abstraction layer of the LCD.
  ******************************************************************************
*/

/* Includes ------------------------------------------------------------------*/
#include "TFTDisplay.h"
#include "display_driver/display_driver.h"
#include "icon_manager/icon_manager.h"
#include <iomanip>
#include <sstream>


/* External variables---------------------------------------------------------*/
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

const int TFTDisplay::max_conf_menus = 4;
const int TFTDisplay::max_chars_edit_menu = 5;
const int TFTDisplay::max_text_list_elements = 3;
const int TFTDisplay::central_text_list_element = 1;
/* Private defines -----------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private variables----------------------------------------------------------*/
static const int conf_menu_y_offset = 68;
/* Private function prototypes -----------------------------------------------*/
/* Functions -----------------------------------------------------------------*/

/**
 * @brief Initializes the display with a black background.
 */
void TFTDisplay::start(){
  Display_driver::init_display();
}

/**
 * @brief Ends SPI communications and release all the related resources.
 */
void TFTDisplay::end(){
  Display_driver::uninit();
}

/**
 * @brief Prints in the Display all the elements associated to the time main menu
 *
 * @param[in] full_refresh True if all the window have to be reloaded, False if only the time
 *                         has to be reloaded.
 * @param[in] time The time as a string.
 */
void TFTDisplay::print_main_menu_time(bool full_refresh, std::string time){

  if(full_refresh){
    Display_driver::reset_region(0, 0, 240, 240, background_color);
    std::vector<uint16_t> icon = Icons::get_main_menu_indicator_icon(1);
    Display_driver::draw_icon_col(
        72, 16, Icons::main_menu_indicator_width, icon.data(), icon.size(), background_color, main_color);
    Display_driver::draw_icon_col(
        60, 198, Icons::main_conf_icon_width, Icons::main_conf_icon.data(), Icons::main_conf_icon.size(),
        background_color, main_color);
    Display_driver::draw_icon_col(
        70, 40, Icons::clock_icon_width, Icons::clock_icon.data(), Icons::clock_icon.size(),
        background_color, main_color);
  }

  Display_driver::write_string_centered_with_min_width(
      185, 200,(char*) time.c_str(), main_color, background_color, FreeSansBold24pt7b);
}


/**
 * @brief Prints in the Display all the elements associated to the temperature main menu
 *
 * @param[in] full_refresh True if all the window have to be reloaded, False if only the temperature,
 *                         bar and color has to be reloaded.
 * @param[in] temp The temperature as a float. This value will be rounded to 2 decimals.
 * @param[in] percentage The bar percentage. Percentages greater than 100 will be truncated to 100.
 */
void TFTDisplay::print_main_menu_temp(bool full_refresh, float temp, uint8_t percentage){

  if(full_refresh){
    Display_driver::reset_region(0, 0, 240, 240, background_color);
    std::vector<uint16_t> icon = Icons::get_main_menu_indicator_icon(2);
    Display_driver::draw_icon_col(
        72, 16, Icons::main_menu_indicator_width, icon.data(), icon.size(), background_color, main_color);
    Display_driver::draw_icon_col(
        60, 198, Icons::main_conf_icon_width, Icons::main_conf_icon.data(), Icons::main_conf_icon.size(),
        background_color, main_color);
  }

  std::vector<uint16_t> temp_icon = Icons::get_temperature_icon(percentage);

  std::stringstream stream;
  stream << std::fixed << std::setprecision(2) << temp;
  std::string temp_string = stream.str();
  std::replace(temp_string.begin(), temp_string.end(), '.', ',');

  uint8_t *color;
  if(percentage < 25)
    color = range_0_25_temp_color;
  else if(percentage < 50)
    color = range_25_50_temp_color;
  else if(percentage < 75)
    color = range_50_75_temp_color;
  else
    color = range_75_100_temp_color;

  Display_driver::draw_icon_col(
      25, 60, Icons::temp_icon_width, temp_icon.data(), temp_icon.size(), background_color, color);
  Display_driver::draw_icon_col(
      110, 115, Icons::celsius_icon_width, Icons::celsius_icon.data(), Icons::celsius_icon.size(),
      background_color, color);
  Display_driver::write_string_with_min_width(
      100, 90, 140, (char*) temp_string.c_str(), color, background_color, FreeSansBold24pt7b);

}


/**
 * @brief Prints in the Display all the elements associated to the humidity main menu
 *
 * @param[in] full_refresh True if all the window have to be reloaded, False if only the humidity,
 *                         bar and color has to be reloaded.
 * @param[in] hum The humidity percentage as a float. This value will be rounded to 2 decimals.
 *                Percentages greater than 100 will be truncated to 100.
 */
void TFTDisplay::print_main_menu_hum(bool full_refresh, float hum){

  int precision, percentage = hum;

  if(hum >= 100)
    precision = 1;
  else
    precision = 2;

  if(full_refresh){
    Display_driver::reset_region(0, 0, 240, 240, background_color);
    std::vector<uint16_t> icon = Icons::get_main_menu_indicator_icon(3);
    Display_driver::draw_icon_col(
        72, 16, Icons::main_menu_indicator_width, icon.data(), icon.size(), background_color, main_color);
    Display_driver::draw_icon_col(
        60, 198, Icons::main_conf_icon_width, Icons::main_conf_icon.data(), Icons::main_conf_icon.size(),
        background_color, main_color);
  }

  std::vector<uint16_t> hum_icon = Icons::get_humidity_icon(percentage);

  std::stringstream stream;
  stream << std::fixed << std::setprecision(precision) << hum;
  std::string hum_string = stream.str();
  std::replace(hum_string.begin(), hum_string.end(), '.', ',');

  uint8_t *color;
  if(percentage < 25)
    color = range_0_25_hum_color;
  else if(percentage < 50)
    color = range_25_50_hum_color;
  else if(percentage < 75)
    color = range_50_75_hum_color;
  else
    color = range_75_100_hum_color;

  Display_driver::draw_icon_col(
      20, 60, Icons::hum_icon_width, hum_icon.data(), hum_icon.size(), background_color, color);
  Display_driver::draw_icon_col(
      130, 115, Icons::percentage_icon_width, Icons::percentage_icon.data(), Icons::percentage_icon.size(),
      background_color, color);
  Display_driver::write_string_with_min_width(
      110, 100, 130, (char*) hum_string.c_str(), color, background_color, FreeSansBold24pt7b);
}


/**
 * @brief Prints in the Display all the elements associated to the pressure main menu
 *
 * @param[in] full_refresh True if all the window have to be reloaded, False if only the pressure
 *                         has to be reloaded.
 * @param[in] press The pressure as a float. This value will be rounded to 2 decimals.
 */
void TFTDisplay::print_main_menu_press(bool full_refresh, float press){

  if(full_refresh){
    Display_driver::reset_region(0, 0, 240, 240, background_color);
    std::vector<uint16_t> icon = Icons::get_main_menu_indicator_icon(4);
    Display_driver::draw_icon_col(
        72, 16, Icons::main_menu_indicator_width, icon.data(), icon.size(), background_color, main_color);
    Display_driver::draw_icon_col(
        60, 198, Icons::main_conf_icon_width, Icons::main_conf_icon.data(), Icons::main_conf_icon.size(),
        background_color, main_color);
    Display_driver::draw_icon_col(
        56, 45, Icons::pressure_icon_width, Icons::pressure_icon.data(), Icons::pressure_icon.size(),
        background_color, main_color);
  }

  std::stringstream stream;
  stream << std::fixed << std::setprecision(2) << press << " Pa";
  std::string press_string = stream.str();
  std::replace(press_string.begin(), press_string.end(), '.', ',');

  Display_driver::write_string_centered_with_min_width(
      165, 220, (char*)press_string.c_str(), main_color, background_color, FreeSansBold18pt7b);
}


/**
 * @brief Prints in the Display all the elements associated to the altitude main menu
 *
 * @param[in] full_refresh True if all the window have to be reloaded, False if only the
 *                         altitude has to be reloaded.
 * @param[in] alt The altitude as a float. This value will be rounded to 2 decimals.
 */
void TFTDisplay::print_main_menu_alt(bool full_refresh, float alt){

  if(full_refresh){
    Display_driver::reset_region(0, 0, 240, 240, background_color);
    std::vector<uint16_t> icon = Icons::get_main_menu_indicator_icon(5);
    Display_driver::draw_icon_col(
        72, 16, Icons::main_menu_indicator_width, icon.data(), icon.size(), background_color, main_color);
    Display_driver::draw_icon_col(
        60, 198, Icons::main_conf_icon_width, Icons::main_conf_icon.data(), Icons::main_conf_icon.size(),
        background_color, main_color);
    Display_driver::draw_icon_col(
        56, 45, Icons::altitude_icon_width, Icons::altitude_icon.data(), Icons::altitude_icon.size(),
        background_color, main_color);
  }

  std::stringstream stream;
  stream << std::fixed << std::setprecision(2) << alt << " m";
  std::string temp_string = stream.str();
  std::replace(temp_string.begin(), temp_string.end(), '.', ',');

  Display_driver::write_string_centered_with_min_width(
      180, 200, (char*)temp_string.c_str(), main_color, background_color, FreeSansBold18pt7b);
}


/**
 * @brief Prints in the Display all the elements associated to the IAQ main menu
 *
 * @param[in] full_refresh True if all the window have to be reloaded, False if only the IAQ,
 *                         bar and color has to be reloaded.
 * @param[in] iaq The IAQ as a float. This value will be rounded to 2 decimals.
 */
void TFTDisplay::print_main_menu_iaq(bool full_refresh, float iaq){

  if(full_refresh){
    Display_driver::reset_region(0, 0, 240, 240, background_color);
    std::vector<uint16_t> icon = Icons::get_main_menu_indicator_icon(6);
    Display_driver::draw_icon_col(
        72, 16, Icons::main_menu_indicator_width, icon.data(), icon.size(), background_color, main_color);
    Display_driver::draw_icon_col(
        60, 198, Icons::main_conf_icon_width, Icons::main_conf_icon.data(), Icons::main_conf_icon.size(),
        background_color, main_color);
  }

  std::vector<uint16_t> iaq_icon = Icons::get_iaq_icon(iaq);

  std::stringstream stream;
  stream << std::fixed << std::setprecision(2) << iaq;
  std::string iaq_string = stream.str();
  std::replace(iaq_string.begin(), iaq_string.end(), '.', ',');
  uint8_t *color;
  if(iaq < 51)
    color = range_0_50_iaq_color;
  else if(iaq < 101)
    color = range_51_100_iaq_color;
  else if(iaq < 151)
    color = range_101_150_iaq_color;
  else if(iaq < 201)
    color = range_151_200_iaq_color;
  else if(iaq < 251)
    color = range_201_250_iaq_color;
  else if(iaq < 351)
    color = range_251_350_iaq_color;
  else
    color = range_351_plus_iaq_color;

  Display_driver::draw_icon_col(
      120 - Icons::iaq_icon_width/2, 45, Icons::iaq_icon_width, iaq_icon.data(), iaq_icon.size(),
      background_color, color);
  Display_driver::write_string_centered_with_min_width(
      180, 200, (char*) iaq_string.c_str(), color, background_color, FreeSansBold18pt7b);
}


/**
 * @brief Prints the configuration menu.
 *
 * @param[in] full_refresh True if all the window have to be reloaded, False if only the
 *                         menus texts and/or the selected menu has to be reloaded.
 * @param[in] menus_refresh True if the menu texts have to be reloaded, False if only the
 *                          selected menu has to be reloaded.
 * @param[in] menus The menus that have to be printed.
 * @param[in] selected The menu currently selected. The count starts at 0.
 * @param[in] prev_selected The menu that was previously selected. The count starts at 0. If
 *                          there was no different previously selected menu, set it to -1.
 */
void TFTDisplay::print_conf_menu(bool full_refresh, bool menus_refresh, std::vector<std::string> menus,
                                 int selected, int prev_selected){


  if(full_refresh){
    Display_driver::reset_region(0, 0, 240, 240, background_color);
    Display_driver::draw_icon_col(35, 35, 170, Icons::menu_square_icon.data(), Icons::menu_square_icon.size(),
        background_color, main_color);
    Display_driver::draw_icon_col(2, 60, 30, Icons::menu_back_icon.data(), Icons::menu_back_icon.size(),
        background_color, main_color);
    Display_driver::draw_icon_col(208, 60, 30, Icons::menu_ok_icon.data(), Icons::menu_ok_icon.size(),
        background_color, main_color);
    Display_driver::draw_icon_col(60, 2, 120, Icons::menu_up_icon.data(), Icons::menu_up_icon.size(),
        background_color, main_color);
    Display_driver::draw_icon_col(60, 208, 120, Icons::menu_down_icon.data(), Icons::menu_down_icon.size(),
        background_color, main_color);
  }

  int menus_count = std::min(max_conf_menus, int(menus.size()));

  if(full_refresh || menus_refresh){
    for(int i = 0; i < menus_count; i++){
      if(selected != i){
        Display_driver::reset_region(44, 44 + i * 39, 196, 79 + i * 39, background_color);
        Display_driver::write_string(
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
        Display_driver::write_string(
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
      Display_driver::write_string(
            45,
            conf_menu_y_offset + 40 * prev_selected,
            (char*)menus[prev_selected].c_str(),
            main_color,
            background_color,
            FreeSansBold12pt7b
          );
    }

    Display_driver::reset_region(44, 44 + selected * 39, 196, 79 + selected * 39, main_color);
    Display_driver::write_string(
          45,
          conf_menu_y_offset + 40 * selected,
          (char*)menus[selected].c_str(),
          background_color,
          main_color,
          FreeSansBold12pt7b
        );
  }
}


/**
 * @brief Prints a text in the center of the window. '\n' characters will be takes as breaklines
 *        and a multiline text will be printed.
 *
 * @param[in] title The text to be represented.
 * @param[in] size The text size. Can be 1 (small), 2 (medium) or 3 (big). Greater values will
 *                 be truncated to 3.
 */
void TFTDisplay::print_centered_title(std::string title, uint8_t size){

  Display_driver::reset_region(0, 0, 240, 240, background_color);

  std::vector<std::string> lines;

  std::stringstream ss(title);
  std::string item;

  lines.reserve(32);
  while(std::getline(ss, item,'\n'))
    lines.push_back(item);

  GFXfont font;
  if(size <= 1)
    font = FreeSansBold12pt7b;
  else if(size == 2)
    font = FreeSansBold18pt7b;
  else
    font = FreeSansBold24pt7b;

  int init_y = 125 - (lines.size()-1)*font.yAdvance/2;
  for(uint8_t i = 0; i < lines.size(); i++){
    Display_driver::write_string_centered(
        init_y + font.yAdvance * i,(char*) lines[i].c_str(), main_color,
        background_color, font);
  }
}


/**
 * @brief Prints a yes/no menu with a title.
 *
 * @param[in] title The text represented as the title.
 * @param[in] size The text size. Can be 1 (small), 2 (medium) or 3 (big). Greater values will
 *                 be truncated to 3.
 */
void TFTDisplay::print_selection_text(std::string title, int size){

  Display_driver::reset_region(0, 0, 240, 240, background_color);


  if(size <= 1){
    Display_driver::write_string_centered(
        75,(char*) title.c_str(), main_color, background_color, FreeSansBold12pt7b);
  }
  else if(size == 2){
    Display_driver::write_string_centered(
        75,(char*) title.c_str(), main_color, background_color, FreeSansBold18pt7b);
  }
  else{
    Display_driver::write_string_centered(
        75,(char*) title.c_str(), main_color, background_color, FreeSansBold24pt7b);
  }

  Display_driver::draw_icon_col(
      0, 100, Icons::yes_no_icon_width, Icons::yes_no_icon.data(), Icons::yes_no_icon.size(),
      background_color, main_color);
}


/**
 * @brief Prints a menu to edit a string. It is composed by a group of rectangles where the different string
 *        characters are located and four arrows as it can be seen in the figure below where the second char
 *        is currently edited.
 *
 *      ____/_\____________
 *   / |   |   |   |   |   | \
 *   \ |___|___|___|___|___| /
 *          \ /
 *
 * @param[in] full_refresh True if all the window have to be reloaded, False if only the
 *                         characters and/or the selected character has to be reloaded.
 * @param[in] letters_refresh True if the characters have to be reloaded, False if only the
 *                            selected char has to be reloaded.
 * @param[in] editable_text The text that have to be printed.
 * @param[in] selected_char The char currently selected. The count starts at 0.
 * @param[in] prev_selected_char The char that was previously selected. The count starts at 0. If
 *                               there was no different previously selected char, set it to -1.
 */
void TFTDisplay::print_editable_text(bool full_refresh, bool letters_refresh,
                                     std::string editable_text, int selected_char, int prev_selected_char){

  if(full_refresh){
    Display_driver::reset_region(0, 100, 240, 140, background_color);

    Display_driver::draw_icon_col(1, 98, Icons::edit_menu_left_width, Icons::edit_menu_left.data(),
        Icons::edit_menu_left.size(), background_color, main_color);
    Display_driver::draw_icon_col(214, 98, Icons::edit_menu_right_width, Icons::edit_menu_right.data(),
        Icons::edit_menu_right.size(), background_color, main_color);
    Display_driver::draw_icon_col(26, 98, Icons::edit_menu_square_width, Icons::edit_menu_square.data(),
        Icons::edit_menu_square.size(), background_color, main_color);
  }

  if(full_refresh || letters_refresh){
    int size = std::min(max_chars_edit_menu, int(editable_text.size()));
    for(int i = 0; i < size; i++){
      std::string char_string = editable_text.substr(i, 1);
      GFXglyph glyph = FreeSansBold18pt7b.glyph[char(char_string[0]) - FreeSansBold18pt7b.first];

      if(selected_char == i){
        Display_driver::reset_region(28 + 37*i, 100, 64 + 37*i, 140, main_color);

        Display_driver::write_string(
            46 + 37*i - glyph.width/2 - glyph.xOffset, 130,
            (char *)char_string.c_str(), background_color, main_color, FreeSansBold18pt7b);

        Display_driver::draw_icon_col(
            28 + 37*i, 70, Icons::edit_menu_up_width, Icons::edit_menu_up.data(), Icons::edit_menu_up.size(),
            background_color, main_color);
        Display_driver::draw_icon_col(
            28 + 37*i, 146, Icons::edit_menu_down_width, Icons::edit_menu_down.data(), Icons::edit_menu_down.size(),
            background_color, main_color);
      }
      else{
        Display_driver::reset_region(28 + 37*i, 100, 64 + 37*i, 140, background_color);
        Display_driver::write_string(
            46 + 37*i - glyph.width/2 - glyph.xOffset, 130,
            (char *)char_string.c_str(), main_color, background_color, FreeSansBold18pt7b);
      }
    }
  }
  else{
    if(prev_selected_char >= 0 && prev_selected_char < max_chars_edit_menu){
      std::string prev_sel_char = editable_text.substr(prev_selected_char, 1);
      GFXglyph glyph = FreeSansBold18pt7b.glyph[char(prev_sel_char[0]) - FreeSansBold18pt7b.first];

      Display_driver::reset_region(28 + 37*prev_selected_char, 100, 64 + 37*prev_selected_char, 140, background_color);
      Display_driver::reset_region(28 + 37*prev_selected_char, 70, 64 + 37*prev_selected_char, 94, background_color);
      Display_driver::reset_region(28 + 37*prev_selected_char, 146, 64 + 37*prev_selected_char, 170, background_color);
      Display_driver::write_string(
          46 + 37*prev_selected_char - glyph.width/2 - glyph.xOffset, 130,
          (char *)prev_sel_char.c_str(), main_color, background_color, FreeSansBold18pt7b);
    }

    std::string sel_char = editable_text.substr(selected_char, 1);
    GFXglyph glyph = FreeSansBold18pt7b.glyph[char(sel_char[0]) - FreeSansBold18pt7b.first];

    Display_driver::reset_region(28 + 37*selected_char, 100, 64 + 37*selected_char, 140, main_color);

    Display_driver::write_string(
        46 + 37*selected_char - glyph.width/2 - glyph.xOffset, 130,
        (char *)sel_char.c_str(), background_color, main_color, FreeSansBold18pt7b);

    Display_driver::draw_icon_col(
        28 + 37*selected_char, 70, Icons::edit_menu_up_width, Icons::edit_menu_up.data(),
        Icons::edit_menu_up.size(), background_color, main_color);
    Display_driver::draw_icon_col(
        28 + 37*selected_char, 146, Icons::edit_menu_down_width, Icons::edit_menu_down.data(),
        Icons::edit_menu_down.size(), background_color, main_color);
  }
}


/**
 * @brief Prints a list of texts. It is composed by two arrows (one at the bottom and the other at the top)
 *        and several texts displayed as a vertical list. If the texts doesn't fit in the fixed width, the
 *        texts will be panned.
 *
 * @param[in] full_refresh True if all the window have to be reloaded, False if only the
 *                         texts and/or the selected text has to be reloaded.
 * @param[in] list_refresh True if the texts have to be reloaded, False if only the
 *                            selected text has to be reloaded.
 * @param[in] text_list The texts that have to be printed.
 * @param[in] exit An inline function that will be called every panning cycle. If it returns true,
 *                 the panning process will finish.
 */
void TFTDisplay::print_text_list(bool full_refresh, bool list_refresh,
    std::vector<std::string> text_list, std::function<bool ()> exit){

  if(full_refresh){
    Display_driver::reset_region(0, 0, 240, 240, background_color);

    Display_driver::draw_icon_col(60, 22, 120, Icons::menu_up_icon.data(), Icons::menu_up_icon.size(),
        background_color, main_color);
    Display_driver::draw_icon_col(60, 188, 120, Icons::menu_down_icon.data(), Icons::menu_down_icon.size(),
        background_color, main_color);
  }

  if(full_refresh || list_refresh){
    for(int i = 0; i < max_text_list_elements; i++){
      if(central_text_list_element == i){
        Display_driver::reset_region(30, 52 + 47*i, 210, 94 + 47*i, main_color);
        Display_driver::write_string_truncated(
            30, 80 + 47*i, 180, (char *)text_list[i].c_str(), background_color, main_color, FreeSansBold12pt7b);
      }
      else{
        Display_driver::reset_region(30, 52 + 47*i, 210, 94 + 47*i, background_color);
        Display_driver::write_string_truncated(
            30, 80 + 47*i, 180, (char *)text_list[i].c_str(), main_color, background_color, FreeSansBold12pt7b);
      }
    }
  }
  else{
    Display_driver::write_panning_string(
          30, 80 + 47*central_text_list_element, 180, 3,
          (char *)text_list[central_text_list_element].c_str(), background_color, main_color, FreeSansBold12pt7b, exit);
  }
}

