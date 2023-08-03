/*
 * display_driver.cpp
 *
 *  Created on: May 7, 2023
 *      Author: Alejandro Hontanilla
 */

#include "display_driver.h"
#include <algorithm>

//Write in memory

uint8_t mem_data_cmd[] = { 0x2C };

//Software reset

uint8_t soft_reset[] = {
	0x01
};

//Get out of sleep mode

uint8_t sleep_out[] = {
	0x11
};

//Color mode command

uint8_t color_mode[] = {
	0x3A
};

//Color mode data:

uint8_t color_mode_data[] = {
	0x55
};

//Memory data access command

uint8_t mem_dat_access[] = {
	0x36
};

//Memory data access data

uint8_t mem_dat_access_mode[] = {
	0x00
};

//Set column address command

uint8_t col_set_addr[] = {
	0x2A
};

//Set row address command

uint8_t row_set_addr[] = {
	0x2B
};


uint8_t invert_on[] = {
	0x21
};

//Set normal mode of operation

uint8_t normal_mode[] = {
	0x13
};

//Power on the display

uint8_t display_on[] = {
	0x29
};

//Dummy array for received data

uint8_t default_rx[5] = {0, };

//SPI speed

static uint32_t speed = 62500000;

//Provate helper function to set the column and row addresses

void prep_write(uint8_t column_data[], uint8_t row_data[]);

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

	int reset_size = 57600 * 2;
	int msg_size = 32;

	uint8_t msg[msg_size];

	std::fill_n(msg, msg_size, BACKGROUND);
	long i;

	for(i = 1; i < reset_size; i += msg_size){
		SPI_Master::send_spi_msg(msg, default_rx, 0, sizeof(msg), 0, speed);
	}

	if(i != reset_size){
		SPI_Master::send_spi_msg(msg, default_rx, 0, i % msg_size, 0, speed);
	}


}

int Display_driver::write_char(int start_x, int start_y, char letter, uint8_t color[], GFXfont font){

	uint8_t char_bytes[40];

	memset(char_bytes, 0, sizeof(char_bytes));

	GFXglyph glyph = font.glyph[letter - font.first];
	int k;

	for(k=0; k<= (glyph.height*glyph.width)/8; k++){
		char_bytes[k] = font.bitmap[glyph.bitmapOffset+k];
	}

	uint8_t col_data[4] = {0x00,};
	uint8_t row_data[4] = {0x00,};

	col_data[1] = start_x + glyph.xOffset;
	row_data[1] = start_y + glyph.yOffset;

	col_data[3] = col_data[1] + glyph.width - 1;
	row_data[3] = row_data[1] + glyph.height - 1;

	k = 0;

	prep_write(col_data,row_data);

	int msg_size = 32, e = 0;
	uint8_t msg[msg_size];

	for(long i = 0; i <= (glyph.height*glyph.width)/8; i++){

		for(long j = 0; (j < 8) && k < glyph.height*glyph.width; j++){

			if(((char_bytes[i] >> (7-j)) & 1) == 1){
				msg[e++] = color[0];
				msg[e++] = color[1];
			}else{
				msg[e++] = BACKGROUND;
				msg[e++] = BACKGROUND;
			}
			k++;

		}

	    if(e == msg_size){
	      e = 0;
	      SPI_Master::send_spi_msg(msg, default_rx, 0, sizeof(msg), 0, speed);
	    }

	}

	if(e != 0){
		SPI_Master::send_spi_msg(msg, default_rx, 0, e, 0, speed);
	}

	return glyph.xAdvance;
}


void Display_driver::write_string(int start_x, int start_y, char string[], uint8_t color[], GFXfont font){
	int cursor = start_x;

	for(size_t i = 0; i < strlen(string); i++){
		if(cursor > (240 - 11)){
			start_y += font.yAdvance;
			cursor = start_x;
		}
		cursor += write_char(cursor, start_y, string[i], color, font);
	}
}


void Display_driver::write_fast_string(int start_x, int start_y, char str[], uint8_t color[], GFXfont font){

  int most_neg_offset = 0, width = 0, largest_last_row = 0;

  for(size_t i=0; i < strlen(str); i++){
    GFXglyph glyph = font.glyph[str[i] - font.first];

    if(glyph.yOffset < most_neg_offset)
      most_neg_offset = glyph.yOffset;

    int last_row = glyph.yOffset + glyph.height;
    if(largest_last_row < last_row)
      largest_last_row = last_row;

    width += glyph.xAdvance;
  }
  int height = largest_last_row - most_neg_offset;

  uint16_t string_bytes[height * width];

  std::fill_n(string_bytes, height * width, BACKGROUND | (BACKGROUND << 8));

  int string_byte_index = 0;
  uint16_t color16 = (((uint16_t)color[1]) << 7) | color[0];

  for(int i=0; i < height; i++){
    for(size_t e=0; e < strlen(str); e++){

      GFXglyph glyph = font.glyph[str[e] - font.first];

      if(most_neg_offset + i >= glyph.yOffset && glyph.height + glyph.yOffset - most_neg_offset > i){
        int btmp_offset_bits = (most_neg_offset + i - glyph.yOffset) * glyph.width;
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


  draw_icon(start_x, start_y, width, string_bytes, width * height);
}


void Display_driver::draw_icon_col(int start_x, int start_y, int width, uint16_t bit_map[],int size, uint8_t color[]){

	uint8_t height = size/(width);

	uint8_t col_data[4] = {0x00,};
	uint8_t row_data[4] = {0x00,};

	col_data[1] = start_x;
	row_data[1] = start_y;

	col_data[3] = col_data[1] + width - 1;
	row_data[3] = row_data[1] + height - 1;

	prep_write(col_data,row_data);

	uint8_t byte_sep[2];
	int msg_size = 32, e = 0;
	uint8_t msg[msg_size];

	std::fill_n(msg, 2, BACKGROUND);

	for(long i = 0; i <= size; i++){
		byte_sep[1] = bit_map[i] >> 7;
		byte_sep[0] = bit_map[i] & 255;
		if(byte_sep[1] == BACKGROUND && byte_sep[0] == BACKGROUND){
		      msg[e++] = byte_sep[0];
		      msg[e++] = byte_sep[1];
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

void Display_driver::draw_icon(int start_x, int start_y, int width, uint16_t bit_map[],int size){

	uint8_t height = size/(width);

	uint8_t col_data[4] = {0x00,};
	uint8_t row_data[4] = {0x00,};

	col_data[1] = start_x;
	row_data[1] = start_y;

	col_data[3] = col_data[1] + width - 1;
	row_data[3] = row_data[1] + height - 1;

	prep_write(col_data,row_data);

	int msg_size = 32, e = 0;
	uint8_t msg[msg_size];

	for(long i = 0; i <= size; i++){
		msg[e++] = bit_map[i] & 255;
		msg[e++] = bit_map[i] >> 7;

		if(e == msg_size){
			e = 0;
			SPI_Master::send_spi_msg(msg, default_rx, 0, sizeof(msg), 0, speed);
		}
	}

	if(e != 0){
		SPI_Master::send_spi_msg(msg, default_rx, 0, e, 0, speed);
	}

}

void Display_driver::uninit(){

	SPI_Master::spi_end();

}

void prep_write(uint8_t column_data[], uint8_t row_data[]){

	SPI_Master::send_spi_msg(col_set_addr, default_rx, 1, sizeof(col_set_addr), 10000, speed);
	SPI_Master::send_spi_msg(column_data, default_rx, 0, 4, 0, speed);
	SPI_Master::send_spi_msg(row_set_addr, default_rx, 1, sizeof(row_set_addr), 10000, speed);
	SPI_Master::send_spi_msg(row_data, default_rx, 0, 4, 0, speed);
	SPI_Master::send_spi_msg(mem_data_cmd, default_rx, 1, sizeof(mem_data_cmd), 10000, speed);

}


