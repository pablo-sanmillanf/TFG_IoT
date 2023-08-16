/**
  ******************************************************************************
  * @file   TFTDisplay.h
  * @author Pablo San Millán Fierro (pablo.sanmillanf@alumnos.upm.es)
  * @brief  LCD printer handler Module Header.
  *
  * @note   End-of-degree work.
  *         This module is the top abstraction layer of the LCD.
  ******************************************************************************
*/

#ifndef __TFTDISPLAY_H__
#define __TFTDISPLAY_H__


/* Includes ------------------------------------------------------------------*/
#include <string>
#include <vector>
#include <functional>

namespace TFTDisplay{


/* Exported types ------------------------------------------------------------*/
/* Exported variables --------------------------------------------------------*/
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

extern const int max_conf_menus;
extern const int max_chars_edit_menu;
extern const int max_text_list_elements;
extern const int central_text_list_element;


/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported Functions --------------------------------------------------------*/
/**
 * @brief Initializes the display with a black background.
 */
void start();


/**
 * @brief Ends SPI communications and release all the related resources.
 */
void end();


/**
 * @brief Prints in the Display all the elements associated to the time main menu
 *
 * @param[in] full_refresh True if all the window have to be reloaded, False if only the time
 *                         has to be reloaded.
 * @param[in] time The time as a string.
 */
void print_main_menu_time(bool full_refresh, std::string time);

/**
 * @brief Prints in the Display all the elements associated to the temperature main menu
 *
 * @param[in] full_refresh True if all the window have to be reloaded, False if only the temperature,
 *                         bar and color has to be reloaded.
 * @param[in] temp The temperature as a float. This value will be rounded to 2 decimals.
 * @param[in] percentage The bar percentage. Percentages greater than 100 will be truncated to 100.
 */
void print_main_menu_temp(bool full_refresh, float temp, uint8_t percentage);

/**
 * @brief Prints in the Display all the elements associated to the humidity main menu
 *
 * @param[in] full_refresh True if all the window have to be reloaded, False if only the humidity,
 *                         bar and color has to be reloaded.
 * @param[in] hum The humidity percentage as a float. This value will be rounded to 2 decimals.
 *                Percentages greater than 100 will be truncated to 100.
 */
void print_main_menu_hum(bool full_refresh, float hum);

/**
 * @brief Prints in the Display all the elements associated to the pressure main menu
 *
 * @param[in] full_refresh True if all the window have to be reloaded, False if only the pressure
 *                         has to be reloaded.
 * @param[in] press The pressure as a float. This value will be rounded to 2 decimals.
 */
void print_main_menu_press(bool full_refresh, float press);

/**
 * @brief Prints in the Display all the elements associated to the altitude main menu
 *
 * @param[in] full_refresh True if all the window have to be reloaded, False if only the
 *                         altitude has to be reloaded.
 * @param[in] alt The altitude as a float. This value will be rounded to 2 decimals.
 */
void print_main_menu_alt(bool full_refresh, float alt);

/**
 * @brief Prints in the Display all the elements associated to the IAQ main menu
 *
 * @param[in] full_refresh True if all the window have to be reloaded, False if only the IAQ,
 *                         bar and color has to be reloaded.
 * @param[in] iaq The IAQ as a float. This value will be rounded to 2 decimals.
 */
void print_main_menu_iaq(bool full_refresh, float iaq);


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
void print_conf_menu(bool full_refresh, bool menus_refresh, std::vector<std::string> menus,
                     int selected, int prev_selected);

/**
 * @brief Prints a text in the center of the window. '\n' characters will be takes as breaklines
 *        and a multiline text will be printed.
 *
 * @param[in] title The text to be represented.
 * @param[in] size The text size. Can be 1 (small), 2 (medium) or 3 (big). Greater values will
 *                 be truncated to 3.
 */
void print_centered_title(std::string title, uint8_t size);

/**
 * @brief Prints a yes/no menu with a title.
 *
 * @param[in] title The text represented as the title.
 * @param[in] size The text size. Can be 1 (small), 2 (medium) or 3 (big). Greater values will
 *                 be truncated to 3.
 */
void print_selection_text(std::string title, int size);

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
void print_editable_text(bool full_refresh, bool letters_refresh,
                         std::string editable_text, int selected_char, int prev_selected_char);

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
void print_text_list(bool full_refresh, bool list_refresh, std::vector<std::string> text_list,
    std::function<bool ()> exit);

}

#endif /* __TFTDISPLAY_H__ */
