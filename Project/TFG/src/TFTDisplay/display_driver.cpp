/*
 * display_driver.cpp
 *
 *  Created on: May 7, 2023
 *      Author: root
 */

#include "display_driver.h"
#include "arial_20v2.c"
#include <algorithm>

uint8_t default_tx[] = {
	0x04, 0x00, 0x00, 0x00, 0x00
};

uint8_t mem_data_cmd[] = { 0x2C };

uint8_t mem_data_read[] = { 0x2E, 0x00, 0x00, 0x00, 0x00 };

uint8_t mem_data_tx[] = { 0xFF, 0xFF};

uint8_t mem_data_tx_background[] = { 0xFF, 0xFF};

uint8_t mem_data_tx_trial[] = { 0xF0, 0x0F};

uint8_t soft_reset[] = {
	0x01
};

uint8_t sleep_out[] = {
	0x11
};

uint8_t color_mode[] = {
	0x3A
};

uint8_t color_mode_data[] = {
	0x55
};

uint8_t mem_dat_access[] = {
	0x36
};

uint8_t mem_dat_access_mode[] = {
	0x00
};

uint8_t col_set_addr[] = {
	0x2A
};

uint8_t col_set_data[] = {
	0x00, 0x70, 0x00, 0x80
};

uint8_t row_set_addr[] = {
	0x2B
};

uint8_t row_set_data[] = {
	0x00, 0x80, 0x00, 0x80
};

uint8_t invert_on[] = {
	0x21
};

uint8_t normal_mode[] = {
	0x13
};

uint8_t display_on[] = {
	0x29
};

uint8_t default_rx[5] = {0, };

static uint32_t cmd_speed = 62500000;

static uint32_t data_speed = 62500000;

void Display_driver::init_display(){

	SPI_Master::send_spi_msg(soft_reset, default_rx, 1, sizeof(soft_reset), 0, cmd_speed);

	sleep(0.5);

	SPI_Master::send_spi_msg(sleep_out, default_rx, 1, sizeof(sleep_out), 10000, cmd_speed);
	SPI_Master::send_spi_msg(color_mode, default_rx, 1, sizeof(color_mode), 10000, cmd_speed);
	SPI_Master::send_spi_msg(color_mode_data, default_rx, 0, sizeof(color_mode_data), 10000, cmd_speed);
	SPI_Master::send_spi_msg(mem_dat_access, default_rx, 1, sizeof(mem_dat_access), 10000, cmd_speed);
	SPI_Master::send_spi_msg(mem_dat_access_mode, default_rx, 0, sizeof(mem_dat_access_mode), 10000, cmd_speed);
	SPI_Master::send_spi_msg(invert_on, default_rx, 1, sizeof(invert_on), 10000, cmd_speed);
	SPI_Master::send_spi_msg(normal_mode, default_rx, 1, sizeof(normal_mode), 10000, cmd_speed);
	SPI_Master::send_spi_msg(display_on, default_rx, 1, sizeof(display_on), 10000, cmd_speed);
	SPI_Master::send_spi_msg(mem_data_cmd, default_rx, 1, sizeof(mem_data_cmd), 10000, cmd_speed);

	int reset_size = 57600 * 2;
	int msg_size = 32;

	uint8_t msg[msg_size];

	std::fill_n(msg, msg_size, 0xFF);
	long i;

  for(i = 1; i < reset_size; i += msg_size){
    SPI_Master::send_spi_msg(msg, default_rx, 0, sizeof(msg), 0, data_speed);
  }
  if(i != reset_size){
    SPI_Master::send_spi_msg(msg, default_rx, 0, i % msg_size, 0, data_speed);
  }


}

void Display_driver::write_char(int start_x, int start_y, char letter, uint8_t color[]){

	uint8_t char_bytes[40];
	uint8_t left_pad;
	uint8_t top_pad;
	uint8_t height;
	uint8_t width;

	memset(char_bytes, 0, sizeof(char_bytes));

	get_char_A20(letter, char_bytes, &left_pad, &top_pad, &height, &width);

	printf("Tried to get a character.");

	col_set_data[1] = start_x + left_pad;
	row_set_data[1] = start_y + top_pad;

	col_set_data[3] = col_set_data[1] + width - 1;
	row_set_data[3] = row_set_data[1] + height - 1;

	int k = 0;

	SPI_Master::send_spi_msg(col_set_addr, default_rx, 1, sizeof(col_set_addr), 10000, cmd_speed);
	SPI_Master::send_spi_msg(col_set_data, default_rx, 0, sizeof(col_set_data), 0, cmd_speed);
	SPI_Master::send_spi_msg(row_set_addr, default_rx, 1, sizeof(col_set_addr), 10000, cmd_speed);
	SPI_Master::send_spi_msg(row_set_data, default_rx, 0, sizeof(col_set_data), 0, cmd_speed);
	SPI_Master::send_spi_msg(mem_data_cmd, default_rx, 1, sizeof(mem_data_cmd), 10000, cmd_speed);
	for(long i = 0; i <= (height*width)/8; i++){
		for(long j = 0; (j < 8) && k < height*width; j++){
			if(((char_bytes[i] >> (7-j)) & 1) == 1){
				SPI_Master::send_spi_msg(color, default_rx, 0, 2, 0, data_speed);
			}else{
				SPI_Master::send_spi_msg(mem_data_tx_background, default_rx, 0, sizeof(mem_data_tx_background), 0, data_speed);
			}
			k++;
		}
	}
}

void Display_driver::write_string(int start_x, int start_y, char string[], uint8_t color[]){

	int x = 0;

	for(int i = 0; i < strlen(string); i++){
		if((start_x + (14*x)) < (240 - 11)){
			write_char(start_x + (14*x), start_y, string[i], color);
		}else{
			start_y += 23;
			x = 0;
			write_char(start_x + (14*x), start_y, string[i], color);
		}
		x++;
	}
}

void Display_driver::draw_icon_col(int start_x, int start_y, int width, uint16_t bit_map[],int size, uint8_t color[]){

	uint8_t height = size/(width);

	col_set_data[1] = start_x;
	row_set_data[1] = start_y;

	col_set_data[3] = col_set_data[1] + width - 1;
	row_set_data[3] = row_set_data[1] + height - 1;

	SPI_Master::send_spi_msg(col_set_addr, default_rx, 1, sizeof(col_set_addr), 10, cmd_speed);
	SPI_Master::send_spi_msg(col_set_data, default_rx, 0, sizeof(col_set_data), 0, cmd_speed);
	SPI_Master::send_spi_msg(row_set_addr, default_rx, 1, sizeof(col_set_addr), 10, cmd_speed);
	SPI_Master::send_spi_msg(row_set_data, default_rx, 0, sizeof(col_set_data), 0, cmd_speed);
	SPI_Master::send_spi_msg(mem_data_cmd, default_rx, 1, sizeof(mem_data_cmd), 10, cmd_speed);


	uint8_t byte_sep[2];
	int msg_size = 32, e = 0;
	uint8_t msg[msg_size];


  for(long i = 0; i < size; i++){
    byte_sep[1] = bit_map[i] >> 7;
    byte_sep[0] = bit_map[i] & 255;
    if(mem_data_tx_background[1] == byte_sep[1] && mem_data_tx_background[0] == byte_sep[0]){
      msg[e++] = byte_sep[0];
      msg[e++] = byte_sep[1];
    }
    else{
      msg[e++] = color[0];
      msg[e++] = color[1];
    }
    if(e == msg_size){
      e = 0;
      SPI_Master::send_spi_msg(msg, default_rx, 0, sizeof(msg), 0, data_speed);
    }
  }
  if(e != 0){
    SPI_Master::send_spi_msg(msg, default_rx, 0, e, 0, data_speed);
  }
}

void Display_driver::draw_icon(int start_x, int start_y, int width, uint16_t bit_map[],int size){

	uint8_t height = size/(width);

	col_set_data[1] = start_x;
	row_set_data[1] = start_y;

	col_set_data[3] = col_set_data[1] + width - 1;
	row_set_data[3] = row_set_data[1] + height - 1;

	int k = 0;

	SPI_Master::send_spi_msg(col_set_addr, default_rx, 1, sizeof(col_set_addr), 10000, cmd_speed);
	SPI_Master::send_spi_msg(col_set_data, default_rx, 0, sizeof(col_set_data), 0, cmd_speed);
	SPI_Master::send_spi_msg(row_set_addr, default_rx, 1, sizeof(col_set_addr), 10000, cmd_speed);
	SPI_Master::send_spi_msg(row_set_data, default_rx, 0, sizeof(col_set_data), 0, cmd_speed);
	SPI_Master::send_spi_msg(mem_data_cmd, default_rx, 1, sizeof(mem_data_cmd), 10000, cmd_speed);
/*	for(long i = 0; i <= (height*width)/8; i++){
		for(long j = 0; (j < 8) && k < height*width; j++){
			if(((bit_map[i] >> (7-j)) & 1) == 1){
				SPI_Master::send_spi_msg(color, default_rx, 0, 2, 0, data_speed);
			}else{
				SPI_Master::send_spi_msg(mem_data_tx_background, default_rx, 0, sizeof(mem_data_tx_background), 0, data_speed);
			}
			k++;
		}
	}*/

	uint8_t byte_sep[2];
	uint16_t intended_color;

	for(long i = 0; i <= size; i++){
		intended_color = bit_map[i];
		byte_sep[1] = bit_map[i] >> 7;
		byte_sep[0] = bit_map[i] & 255;
		printf("%x", byte_sep[1]);
		SPI_Master::send_spi_msg(byte_sep, default_rx, 0, 2, 0, data_speed);
	}

}


