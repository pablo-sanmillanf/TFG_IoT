/**
  ******************************************************************************
  * @file    display_driver.cpp
  * @version 2
  * @author  Pablo San Millán Fierro (pablo.sanmillanf@alumnos.upm.es)
  * @brief   TFTDisplay driver. Provides a layer of abstraction over SPI comms.
  *
  * @note   End-of-degree work.
  *         Based on the code from Alejandro Hontanilla Belinchón
  *         (a.hontanillab@alumnos.upm.es)
  ******************************************************************************
*/


/* Includes ------------------------------------------------------------------*/
#include "display_driver.h"
#include <algorithm>
#include <vector>

/* External variables---------------------------------------------------------*/
/* Private defines -----------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private variables----------------------------------------------------------*/
static uint8_t mem_data_cmd[] = { 0x2C };      //Write in memory
static uint8_t soft_reset[] = {0x01};          //Software reset
static uint8_t sleep_out[] = {0x11};           //Get out of sleep mode
static uint8_t color_mode[] = {0x3A};          //Color mode command
static uint8_t color_mode_data[] = {0x55};     //Color mode data
static uint8_t mem_dat_access[] = {0x36};      //Memory data access command
static uint8_t mem_dat_access_mode[] = {0x00}; //Memory data access data
static uint8_t col_set_addr[] = {0x2A};        //Set column address command
static uint8_t row_set_addr[] = {0x2B};        //Set row address command
static uint8_t invert_on[] = {0x21};
static uint8_t normal_mode[] = {0x13};         //Set normal mode of operation
static uint8_t display_on[] = {0x29};          //Power on the display
static uint8_t default_rx[5] = {0, };          //Dummy array for received data
static uint32_t speed = 62500000;               //SPI speed
/* Private function prototypes -----------------------------------------------*/
void prep_write(uint8_t start_x, uint8_t start_y, uint8_t end_x, uint8_t end_y);
std::vector<uint16_t> prepare_string(char str[], uint8_t color[], uint8_t background_color[], GFXfont font,
    int *text_y_offset, int *width);
/* Functions -----------------------------------------------------------------*/

/**
 * @brief Initializes the display with a black background.
 */
void Display_driver::init_display(){

	SPI_Master::spi_start(0, 0, 8, 62500000);

	SPI_Master::send_spi_msg(soft_reset, default_rx, 1, sizeof(soft_reset), 0, speed);

	sleep(0.5);

	SPI_Master::send_spi_msg(sleep_out, default_rx, 1, sizeof(sleep_out), 10000, speed);
	SPI_Master::send_spi_msg(color_mode, default_rx, 1, sizeof(color_mode), 10000, speed);
	SPI_Master::send_spi_msg(color_mode_data, default_rx, 0, sizeof(color_mode_data), 10000, speed);
	SPI_Master::send_spi_msg(mem_dat_access, default_rx, 1, sizeof(mem_dat_access), 10000, speed);
	SPI_Master::send_spi_msg(mem_dat_access_mode, default_rx, 0, sizeof(mem_dat_access_mode), 10000, speed);
	SPI_Master::send_spi_msg(invert_on, default_rx, 1, sizeof(invert_on), 10000, speed);
	SPI_Master::send_spi_msg(normal_mode, default_rx, 1, sizeof(normal_mode), 10000, speed);
	SPI_Master::send_spi_msg(display_on, default_rx, 1, sizeof(display_on), 10000, speed);
	SPI_Master::send_spi_msg(mem_data_cmd, default_rx, 1, sizeof(mem_data_cmd), 10000, speed);

	uint8_t background_color[] = {0x00, 0x00};
	reset_region(0, 0, 240, 240, background_color);
}


/**
 * @brief Reset a region with an uniform color.
 *
 * @param[in] start_x X coordinate of the top left corner of the bounding box containing the icon
 * @param[in] start_y Y coordinate of the top left corner of the bounding box containing the icon
 * @param[in] end_x X coordinate of the bottom right corner of the bounding box containing the icon
 * @param[in] end_y Y coordinate of the bottom fight corner of the bounding box containing the icon
 * @param[in] reset_color Color of the region.
 */
void Display_driver::reset_region(int start_x, int start_y, int end_x, int end_y, uint8_t reset_color[]){

  prep_write(start_x, start_y, end_x - 1, end_y - 1);

  int reset_size = (end_x - start_x) * (end_y - start_y) * 2;
  int msg_size = 32; //Must be even

  uint8_t msg[msg_size];

  for (int i = 0; i < msg_size; i++) {
    if(i%2 == 0)
      msg[i] = reset_color[0];
    else
      msg[i] = reset_color[1];
  }

  long i;

  for(i = 1; i < reset_size; i += msg_size){
    SPI_Master::send_spi_msg(msg, default_rx, 0, sizeof(msg), 0, speed);
  }

  if(i != reset_size){
    SPI_Master::send_spi_msg(msg, default_rx, 0, i % msg_size, 0, speed);
  }
}


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
void Display_driver::write_string(int start_x, int start_y, char str[], uint8_t color[], uint8_t background_color[], GFXfont font){

  int y_offset, width;
  std::vector<uint16_t> string_bytes = prepare_string(str, color, background_color, font, &y_offset, &width);

  draw_icon(start_x, start_y + y_offset, width, string_bytes.data(), string_bytes.size());
}


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
void Display_driver::write_string_centered(int start_y, char str[], uint8_t color[], uint8_t background_color[], GFXfont font){

  int y_offset, width;
  std::vector<uint16_t> string_bytes = prepare_string(str, color, background_color, font, &y_offset, &width);

  draw_icon(120 - width/2, start_y + y_offset, width, string_bytes.data(), string_bytes.size());
}


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
void Display_driver::write_string_with_min_width(int start_x, int start_y, int min_width, char str[], uint8_t color[],
    uint8_t background_color[], GFXfont font){

  int y_offset, icon_width, total_padding = 0;
  std::vector<uint16_t> string_bytes = prepare_string(str, color, background_color, font, &y_offset, &icon_width);

  if(min_width > icon_width){
    total_padding = min_width - icon_width;
  }

  int height = string_bytes.size()/icon_width;
  int real_width = icon_width + total_padding;
  uint16_t result[real_width*height];

  for(int i = 0; i < height; i++){
    std::copy(
        string_bytes.begin() + icon_width*i,
        string_bytes.begin() + icon_width*(i + 1),
        result + real_width*i);
    std::fill(
        result + real_width*i + icon_width,
        result + real_width*i + icon_width + total_padding,
        background_color[0] | (background_color[1] << 8));
  }
  draw_icon(start_x, start_y + y_offset, real_width, result, real_width * height);

}


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
void Display_driver::write_string_centered_with_min_width(int start_y, int min_width, char str[], uint8_t color[],
    uint8_t background_color[], GFXfont font){

  int y_offset, icon_width, total_padding = 0, left_padding, right_padding;
  std::vector<uint16_t> string_bytes = prepare_string(str, color, background_color, font, &y_offset, &icon_width);

  if(min_width > icon_width){
    total_padding = min_width - icon_width;
  }
  left_padding = total_padding/2;
  right_padding = total_padding - int(total_padding/2);

  int height = string_bytes.size()/icon_width;
  int real_width = icon_width + total_padding;
  uint16_t result[real_width*height];

  for(int i = 0; i < height; i++){
    std::fill(
        result + real_width*i,
        result + real_width*i + left_padding,
        background_color[0] | (background_color[1] << 8)
        );
    std::copy(
        string_bytes.begin() + icon_width*i,
        string_bytes.begin() + icon_width*(i + 1),
        result + real_width*i + left_padding);
    std::fill(
        result + real_width*i + left_padding + icon_width,
        result + real_width*i + left_padding + icon_width + right_padding,
        background_color[0] | (background_color[1] << 8));
  }
  draw_icon(120 - real_width/2, start_y + y_offset, real_width, result, real_width * height);

}


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
 *            process will finish.
 */
void Display_driver::write_panning_string(int start_x, int start_y, int panning_width, int panning_step,
    char str[], uint8_t color[], uint8_t background_color[], GFXfont font, std::function<bool ()> exit){

  int y_offset, width;
  std::vector<uint16_t> string_bytes = prepare_string(str, color, background_color, font, &y_offset, &width);

  draw_panning_icon(start_x, start_y + y_offset, width, panning_width, string_bytes.data(), string_bytes.size(),
                    panning_step, exit);
}


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
void Display_driver::write_string_truncated(int start_x, int start_y, int truncated_width, char str[],
    uint8_t color[], uint8_t background_color[], GFXfont font){

  int y_offset, icon_width;
  std::vector<uint16_t> string_bytes = prepare_string(str, color, background_color, font, &y_offset, &icon_width);

  int real_width, total_padding;
  if(icon_width < truncated_width){
    real_width = icon_width;
    total_padding = truncated_width - icon_width;
  }
  else{
    real_width = truncated_width;
    total_padding = 0;
  }

  int height = string_bytes.size()/icon_width;
  uint16_t result[truncated_width*height];

  for(int i = 0; i < height; i++){
    std::copy(
        string_bytes.begin() + i*icon_width,
        string_bytes.begin() + i*icon_width + real_width,
        result + truncated_width*i);
    std::fill(
            result + truncated_width*i + icon_width,
            result + truncated_width*i + icon_width + total_padding,
            background_color[0] | (background_color[1] << 8));
  }
  draw_icon(start_x, start_y + y_offset, truncated_width, result, truncated_width * height);
}


/**
 * @brief Draws a given icon to the display overriding the color of the bitmap
 *
 * @param[in] start_x X coordinate of the top left corner of the bounding box containing the icon
 * @param[in] start_y Y coordinate of the top left corner of the bounding box containing the icon
 * @param[in] byte_map bytes of the icon with format 565 of the icon to write
 * @param[in] size size of the bitmap
 * @param[in] color color of the icon
 */
void Display_driver::draw_icon_col(int start_x, int start_y, int width, uint16_t bit_map[],int size, uint8_t color[], uint8_t background_color[]){

	prep_write(start_x, start_y, start_x + width - 1, start_y + size/width - 1);

	int msg_size = 32, e = 0;
	uint8_t msg[msg_size];

	for(long i = 0; i < size; i++){
		if(!bit_map[i]){
		      msg[e++] = background_color[0];
		      msg[e++] = background_color[1];
		}else{
		      msg[e++] = color[0];
		      msg[e++] = color[1];
		}

	    if(e == msg_size){
	      e = 0;
	      SPI_Master::send_spi_msg(msg, default_rx, 0, sizeof(msg), 0, speed);
	    }
	}

	if(e != 0){
		SPI_Master::send_spi_msg(msg, default_rx, 0, e, 0, speed);
	}

}


/**
 * @brief Draws a given icon to the display without overriding the color of the bitmap
 *
 * @param[in] start_x X coordinate of the top left corner of the bounding box containing the icon
 * @param[in] start_y Y coordinate of the top left corner of the bounding box containing the icon
 * @param[in] byte_map bytes of the icon with format 565 of the icon to write
 * @param[in] size size of the bitmap
 */
void Display_driver::draw_icon(int start_x, int start_y, int width, uint16_t bit_map[], int size){

  prep_write(start_x, start_y, start_x + width - 1, start_y + size/width - 1);

	int msg_size = 32, e = 0;
	uint8_t msg[msg_size];

	for(long i = 0; i < size; i++){
		msg[e++] = bit_map[i] & 255;
		msg[e++] = bit_map[i] >> 8;

		if(e == msg_size){
			e = 0;
			SPI_Master::send_spi_msg(msg, default_rx, 0, sizeof(msg), 0, speed);
		}
	}

	if(e != 0){
		SPI_Master::send_spi_msg(msg, default_rx, 0, e, 0, speed);
	}

}


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
 *            process will finish.
 */
void Display_driver::draw_panning_icon(int start_x, int start_y, int icon_width, int panning_width,
                                       uint16_t bit_map[], int size, int panning_step, std::function<bool ()> exit){

  int panning_lines = icon_width - panning_width, height = size/icon_width;
  uint16_t result[panning_width*height];
  panning_step = panning_lines/(int(panning_lines/panning_step)); //Chose the near-lower-divisible panning step

  for(int e = 0; e < panning_lines; e += panning_step){
    for(int i = 0; i < height; i++){
      std::copy(
          bit_map + i*icon_width + e,
          bit_map + i*icon_width + e + panning_width,
          result + panning_width*i);
      if(exit())
        return;
    }
    draw_icon(start_x, start_y, panning_width, result, panning_width * height);
  }
}

/**
 * @brief Ends SPI communications and release all the related resources.
 */
void Display_driver::uninit(){

	SPI_Master::spi_end();

}


/* Private functions ---------------------------------------------------------*/
/**
 * @brief Prepare the Display to write an icon or a string. This function send the pertinent
 *        commands to do that.
 *
 * @param[in] start_x X coordinate of the top left corner of the bounding box containing the icon
 * @param[in] start_y Y coordinate of the top left corner of the bounding box containing the icon
 * @param[in] end_x X coordinate of the bottom right corner of the bounding box containing the icon
 * @param[in] end_y Y coordinate of the bottom fight corner of the bounding box containing the icon
 */
void prep_write(uint8_t start_x, uint8_t start_y, uint8_t end_x, uint8_t end_y){
  uint8_t col_data[4] = {0x00, start_x, 0x00, end_x};
  uint8_t row_data[4] = {0x00, start_y, 0x00, end_y};

  SPI_Master::send_spi_msg(col_set_addr, default_rx, 1, sizeof(col_set_addr), 1000, speed);
  SPI_Master::send_spi_msg(col_data, default_rx, 0, 4, 0, speed);
  SPI_Master::send_spi_msg(row_set_addr, default_rx, 1, sizeof(row_set_addr), 1000, speed);
  SPI_Master::send_spi_msg(row_data, default_rx, 0, 4, 0, speed);
  SPI_Master::send_spi_msg(mem_data_cmd, default_rx, 1, sizeof(mem_data_cmd), 1000, speed);

}

/**
 * @brief Converts the supplied string into a bitmap using the supplied font.
 *
 * @brief Writes a text string in the Display inside a bounding box with a supplied fixed width (truncated_width).
 *        If the text is greater than this fixed width, the text will be truncated.
 *
 * @param[in] str The string to write.
 * @param[in] color Color of the text to write.
 * @param[in] background_color Background color of the box where the text will be located.
 * @param[in] font font of the text.
 * @param[out] text_y_offset pixels from the line where the imaginary line is written and the uppermost
 *                           border of the bounding box.
 * @param[out] width Width of the resulting icon.
 */
std::vector<uint16_t> prepare_string(char str[], uint8_t color[], uint8_t background_color[], GFXfont font,
    int *text_y_offset, int *width){

  int height, largest_last_row = 0;

  *text_y_offset = 0;
  *width = 0;

  for(size_t i=0; i < strlen(str); i++){
    GFXglyph glyph = font.glyph[str[i] - font.first];

    if(glyph.yOffset < (*text_y_offset))
      *text_y_offset = glyph.yOffset;

    int last_row = glyph.yOffset + glyph.height;
    if(largest_last_row < last_row)
      largest_last_row = last_row;

    *width += glyph.xAdvance;
  }
  height = largest_last_row - (*text_y_offset);

  std::vector<uint16_t> string_bytes(height * (*width), background_color[0] | (background_color[1] << 8));

  int string_byte_index = 0;
  uint16_t color16 = (((uint16_t)color[1]) << 8) | color[0];

  for(int i=0; i < height; i++){
    for(size_t e=0; e < strlen(str); e++){

      GFXglyph glyph = font.glyph[str[e] - font.first];

      if((*text_y_offset) + i >= glyph.yOffset && glyph.height + glyph.yOffset - (*text_y_offset) > i){
        int btmp_offset_bits = ((*text_y_offset) + i - glyph.yOffset) * glyph.width;
        int btmp_offset_bytes = btmp_offset_bits / 8;
        int btmp_offset_remainder = btmp_offset_bits % 8;

        char bitmap_byte = font.bitmap[glyph.bitmapOffset + btmp_offset_bytes];
        int a = btmp_offset_remainder;

        do{
          if(bitmap_byte & (1 << (7 - a%8)))
            string_bytes[string_byte_index + glyph.xOffset + a - btmp_offset_remainder] = color16;

          if((++a) % 8 == 0)
            bitmap_byte = font.bitmap[glyph.bitmapOffset + (++btmp_offset_bytes)];
        }while(a < glyph.width + btmp_offset_remainder);
      }

      string_byte_index += glyph.xAdvance;
    }
  }
  return string_bytes;
}
