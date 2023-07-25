/*
 * display_driver.h
 *
 *  Created on: May 7, 2023
 *      Author: root
 */

#ifndef DISPLAY_DRIVER_H_
#define DISPLAY_DRIVER_H_

#include "spi_master.h"

#ifdef __cplusplus
extern "C" {
#endif
  /* Exported variables --------------------------------------------------------*/
  /* Exported types ------------------------------------------------------------*/
  /* Exported constants --------------------------------------------------------*/
  /* Exported macro ------------------------------------------------------------*/
  /* Exported Functions --------------------------------------------------------*/

#ifdef __cplusplus
    namespace Display_driver{
#endif
      /**
       * @brief Initializes the display with a black background.
       */
      void init_display();

      /**
       * @brief Writes a single charachter at the specified position
       *
       * @param[in] start_x X coordinate of the top left corner of the bounding box containing the character
       *
       * @param[in] start_y Y coordinate of the top left corner of the bounding box containing the character
       *
       * @param[in] letter character to write
       */
      void write_char(int start_x, int start_y, char letter, uint8_t color[]);

      /**
       * @brief Writes a text string to the Display
       *
       * @param[in] start_x X coordinate of the top left corner of the bounding box containing the first character
       *
       * @param[in] start_y Y coordinate of the top left corner of the bounding box containing the first character
       *
       * @param[in] string string to write
       */
      void write_string(int start_x, int start_y, char string[], uint8_t color[]);

      /**
       * @brief Draws a given icon to the display overriding the color of the bitmap
       *
       * @param[in] start_x X coordinate of the top left corner of the bounding box containing the icon
       *
       * @param[in] start_y Y coordinate of the top left corner of the bounding box containing the icon
       *
       * @param[in] byte_map bytes of the icon with format 565 of the icon to write
       *
       * @param[in] size size of the bitmap
       *
       * @param[in] color color of the icon
       */
      void draw_icon_col(int start_x, int start_y, int width, uint16_t bit_map[], int size, uint8_t color[]);

      /**
       * @brief Draws a given icon to the display without overriding the color of the bitmap
       *
       * @param[in] start_x X coordinate of the top left corner of the bounding box containing the icon
       *
       * @param[in] start_y Y coordinate of the top left corner of the bounding box containing the icon
       *
       * @param[in] byte_map bytes of the icon with format 565 of the icon to write
       *
       * @param[in] size size of the bitmap
       */
      void draw_icon(int start_x, int start_y, int width, uint16_t bit_map[], int size);

#ifdef __cplusplus
    }
#endif

#ifdef __cplusplus
}
#endif

#endif /* DISPLAY_DRIVER_H_ */
