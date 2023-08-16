/**
  ******************************************************************************
  * @file    display_driver.h
  * @version 2
  * @author  Pablo San Millán Fierro (pablo.sanmillanf@alumnos.upm.es)
  * @brief   TFTDisplay driver. Provides a layer of abstraction over SPI comms.
  *
  * @note   End-of-degree work.
  *         Based on the code from Alejandro Hontanilla Belinchón
  *         (a.hontanillab@alumnos.upm.es)
  ******************************************************************************
*/

#ifndef __DISPLAY_DRIVER_H__
#define __DISPLAY_DRIVER_H__

/* Includes ------------------------------------------------------------------*/
#include <functional>
#include "../spi_master/spi_master.h"
#include "../fonts/gfxfont.h"
#include "../fonts/FreeSansBold12pt7b.h"
#include "../fonts/FreeSansBold18pt7b.h"
#include "../fonts/FreeSansBold24pt7b.h"

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
 * @brief Reset a region with an uniform color.
 *
 * @param[in] start_x X coordinate of the top left corner of the bounding box containing the icon
 * @param[in] start_y Y coordinate of the top left corner of the bounding box containing the icon
 * @param[in] end_x X coordinate of the bottom right corner of the bounding box containing the icon
 * @param[in] end_y Y coordinate of the bottom fight corner of the bounding box containing the icon
 * @param[in] reset_color Color of the region.
 */
void reset_region(int start_x, int start_y, int end_x, int end_y, uint8_t reset_color[]);

/**
 * @brief Writes a text string in the Display
 *
 * @param[in] start_x X coordinate of the start of the line where the text will be written. Letters like "p"
 *                    or "y" will trespass this line and write its bottom part below it.
 * @param[in] start_y Y coordinate of the start of the line where the text will be written. Letters like "p"
 *                    or "y" will trespass this line and write its bottom part below it.
 * @param[in] str string to write.
 * @param[in] color color of the text to write.
 * @param[in] background_color color of the background color of the box where the text will be located.
 * @param[in] font font of the text.
 */
void write_string(int start_x, int start_y, char str[], uint8_t color[], uint8_t background_color[], GFXfont font);

/**
 * @brief Writes a text string in the Display centered in the X-axis
 *
 * @param[in] start_y Y coordinate of the start of the line where the text will be written. Letters like "p"
 *                    or "y" will trespass this line and write its bottom part below it.
 * @param[in] str string to write.
 * @param[in] color color of the text to write.
 * @param[in] background_color color of the background color of the box where the text will be located.
 * @param[in] font font of the text.
 */
void write_string_centered(int start_y, char str[], uint8_t color[], uint8_t background_color[], GFXfont font);

/**
 * @brief Writes a text string in the Display inside a bounding box with a supplied minimum width. If the text
 *        is greater than this minimum width, the bounding box will resize to the demanded width.
 *
 * @param[in] start_x X coordinate of the start of the line where the text will be written. Letters like "p"
 *                    or "y" will trespass this line and write its bottom part below it.
 * @param[in] start_y Y coordinate of the start of the line where the text will be written. Letters like "p"
 *                    or "y" will trespass this line and write its bottom part below it.
 * @param[in] min_width Minimum width of the bounding box where the text will be located.
 * @param[in] str string to write.
 * @param[in] color color of the text to write.
 * @param[in] background_color color of the background color of the box where the text will be located.
 * @param[in] font font of the text.
 */
void write_string_with_min_width(int start_x, int start_y, int min_width, char str[], uint8_t color[],
    uint8_t background_color[], GFXfont font);

/**
 * @brief Writes a text string in the Display centered in the X-axis and inside a bounding box with a supplied
 *        minimum width. If the text is greater than this minimum width, the bounding box will resize to the
 *        demanded width.
 *
 * @param[in] start_y Y coordinate of the start of the line where the text will be written. Letters like "p"
 *                    or "y" will trespass this line and write its bottom part below it.
 * @param[in] min_width Minimum width of the bounding box where the text will be located.
 * @param[in] str string to write.
 * @param[in] color color of the text to write.
 * @param[in] background_color color of the background color of the box where the text will be located.
 * @param[in] font font of the text.
 */
void write_string_centered_with_min_width(int start_y, int min_width, char str[], uint8_t color[],
    uint8_t background_color[], GFXfont font);

/**
 * @brief Writes a text string in the Display inside a bounding box with a supplied fixed width (panning_width).
 *        If the text is greater than this fixed width, the text will pan with a movement of "panning_step".
 *        This function do this process synchronously so will block the calling thread. To exit from this
 *        function prematurely the exit function supplied can be used.
 *
 * @param[in] start_x X coordinate of the start of the line where the text will be written. Letters like "p"
 *                    or "y" will trespass this line and write its bottom part below it.
 * @param[in] start_y Y coordinate of the start of the line where the text will be written. Letters like "p"
 *                    or "y" will trespass this line and write its bottom part below it.
 * @param[in] panning_width Fixed width of the bounding box where the text will be located.
 * @param[in] panning_step The number of pixels that the text will be moved in every panning cycle.
 * @param[in] str string to write.
 * @param[in] color color of the text to write.
 * @param[in] background_color color of the background color of the box where the text will be located.
 * @param[in] font font of the text.
 * @param[in] exit An inline function that will be called every panning cycle. If it returns true, the panning
 *                 process will finish.
 */
void write_panning_string(int start_x, int start_y, int panning_width, int panning_step, char str[], uint8_t color[],
    uint8_t background_color[], GFXfont font, std::function<bool ()> exit);

/**
 * @brief Writes a text string in the Display inside a bounding box with a supplied fixed width (truncated_width).
 *        If the text is greater than this fixed width, the text will be truncated.
 *
 * @param[in] start_x X coordinate of the start of the line where the text will be written. Letters like "p"
 *                    or "y" will trespass this line and write its bottom part below it.
 * @param[in] start_y Y coordinate of the start of the line where the text will be written. Letters like "p"
 *                    or "y" will trespass this line and write its bottom part below it.
 * @param[in] truncated_width Fixed width of the bounding box where the text will be located.
 * @param[in] str string to write.
 * @param[in] color color of the text to write.
 * @param[in] background_color color of the background color of the box where the text will be located.
 * @param[in] font font of the text.
 */
void write_string_truncated(int start_x, int start_y, int truncated_width, char str[], uint8_t color[],
    uint8_t background_color[], GFXfont font);

/**
 * @brief Draws a given icon to the display overriding the color of the bitmap
 *
 * @param[in] start_x X coordinate of the top left corner of the bounding box containing the icon
 * @param[in] start_y Y coordinate of the top left corner of the bounding box containing the icon
 * @param[in] byte_map bytes of the icon with format 565 of the icon to write
 * @param[in] size size of the bitmap
 * @param[in] color color of the icon
 */
void draw_icon_col(int start_x, int start_y, int width, uint16_t bit_map[],int size, uint8_t color[],
    uint8_t background_color[]);

/**
 * @brief Draws a given icon to the display without overriding the color of the bitmap
 *
 * @param[in] start_x X coordinate of the top left corner of the bounding box containing the icon
 * @param[in] start_y Y coordinate of the top left corner of the bounding box containing the icon
 * @param[in] byte_map bytes of the icon with format 565 of the icon to write
 * @param[in] size size of the bitmap
 */
void draw_icon(int start_x, int start_y, int width, uint16_t bit_map[], int size);

/**
 * @brief Draws the given icon in the Display inside a bounding box with a supplied fixed width (panning_width).
 *        If the icon's width is greater than this fixed width, the icon will pan with a movement of "panning_step".
 *        This function do this process synchronously so will block the calling thread. To exit from this
 *        function prematurely the exit function supplied can be used.
 *
 * @param[in] start_x X coordinate of the start of the line where the text will be written. Letters like "p"
 *                    or "y" will trespass this line and write its bottom part below it.
 * @param[in] start_y Y coordinate of the start of the line where the text will be written. Letters like "p"
 *                    or "y" will trespass this line and write its bottom part below it.
 * @param[in] icon_width Icon width in pixels.
 * @param[in] panning_width Fixed width of the bounding box where the icon will be located.
 * @param[in] byte_map bytes of the icon with format 565 of the icon to write
 * @param[in] size size of the bitmap
 * @param[in] panning_step The number of pixels that the icon will be moved in every panning cycle.
 * @param[in] exit An inline function that will be called every panning cycle. If it returns true, the panning
 *                 process will finish.
 */
void draw_panning_icon(int start_x, int start_y, int icon_width, int panning_width, uint16_t bit_map[],
    int size, int panning_step, std::function<bool ()> exit);

/**
 * @brief Ends SPI communications and release all the related resources.
 */
void uninit();

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
}
#endif

#endif /* __DISPLAY_DRIVER_H__ */
