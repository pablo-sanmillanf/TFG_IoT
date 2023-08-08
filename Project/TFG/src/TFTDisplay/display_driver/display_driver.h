/**
  ******************************************************************************
  * @file   display_driver.h
  * @author Alejandro Hontanilla Belinchón (a.hontanillab@alumnos.upm.es)
  * @brief  TFTDisplay driver. Provides a layer of abstraction over SPI comms.
  *
  * @note   End-of-degree work.
  ******************************************************************************
*/

#ifndef DISPLAY_DRIVER_H_
#define DISPLAY_DRIVER_H_

#include "../spi_master/spi_master.h"
#include "../fonts/gfxfont.h"
#include "../fonts/FreeMono12pt7b.h"
#include "../fonts/FreeSansBold12pt7b.h"
#include "../fonts/FreeSansBold18pt7b.h"
#include "../fonts/FreeSansBold24pt7b.h"
#include "../fonts/FreeMonoBoldOblique12pt7b.h"

#ifdef __cplusplus
extern "C" {
#endif
  /* Exported variables --------------------------------------------------------*/
  /* Exported types ------------------------------------------------------------*/
  /* Exported constants --------------------------------------------------------*/
  /* Exported macro ------------------------------------------------------------*/

	//Background color

	#define BACKGROUND 0XFF

  /* Exported Functions --------------------------------------------------------*/

#ifdef __cplusplus
    namespace Display_driver{
#endif
      /**
       * @brief Initializes the display with a black background.
       */
      void init_display();

      void reset_region(int start_x, int start_y, int end_x, int end_y, uint8_t reset_color[]);

      /**
       * @brief Writes a text string to the Display
       *
       * @param[in] start_x X coordinate of the top left corner of the bounding box containing the first character
       *
       * @param[in] start_y Y coordinate of the top left corner of the bounding box containing the first character
       *
       * @param[in] string string to write
       *
       * @param[in] color color of the text to write
       *
       * @param[in] font font of the text
       */
      void write_fast_string(int start_x, int start_y, char str[], uint8_t color[], uint8_t background_color[], GFXfont font);

      void write_fast_string_centered(int start_y, char str[], uint8_t color[], uint8_t background_color[], GFXfont font);

      void write_panning_fast_string(int start_x, int start_y, int panning_width, int panning_step,
                char str[], uint8_t color[], uint8_t background_color[], GFXfont font);

      void write_fast_string_truncated(int start_x, int start_y, int truncated_width, char str[],
          uint8_t color[], uint8_t background_color[], GFXfont font);
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
      void draw_icon_col(int start_x, int start_y, int width, uint16_t bit_map[],int size, uint8_t color[], uint8_t background_color[]);

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

      void draw_panning_icon(int start_x, int start_y, int icon_width, int panning_width,
                                             uint16_t bit_map[], int size, int panning_step);

      void uninit();

#ifdef __cplusplus
    }
#endif

#ifdef __cplusplus
}
#endif

#endif /* DISPLAY_DRIVER_H_ */
