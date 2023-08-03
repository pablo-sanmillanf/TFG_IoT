/*
 * icon_manager.h
 *
 *  BY NOW, THIS MODULE IS IN ALPHA VERSION
 */

#ifndef __ICON_MANAGER_H__
#define __ICON_MANAGER_H__

#include <cstdint>
#include <vector>


extern std::vector<uint16_t> menu_back_icon;
extern std::vector<uint16_t> menu_ok_icon;
extern std::vector<uint16_t> menu_up_icon;
extern std::vector<uint16_t> menu_down_icon;


uint16_t* get_temperature_icon(unsigned int *size, unsigned int *width, uint8_t bar_percentage);
uint16_t* get_humidity_icon(unsigned int *size, unsigned int *width, uint8_t bar_percentage);
uint16_t* get_iaq_icon(unsigned int *size, unsigned int *width, uint8_t bar_percentage);


#endif /* __ICON_MANAGER_H__ */
