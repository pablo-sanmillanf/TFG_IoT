/**
  ******************************************************************************
  * @file   buttons.h
  * @author Pablo San Millán Fierro (pablo.sanmillanf@alumnos.upm.es)
  * @brief  Buttons tread handler Module Header.
  *
  * @note   End-of-degree work.
  *         This module handles the bottons of a joystick.
  ******************************************************************************
*/

#ifndef __BUTTONS_H__
#define __BUTTONS_H__

/* Includes ------------------------------------------------------------------*/
#include "custom_gpio.h"
#include "sync_queue.h"
#include <unistd.h>


namespace Buttons{




/* Exported types ------------------------------------------------------------*/
enum BUTTON_TYPE {SHORT_PULSE = 0, LONG_PULSE, LONG_PULSE_RELEASED};
enum BUTTON_SYMBOL {UP = 0, DOWN, LEFT, RIGHT, CENTER};

typedef struct{
  BUTTON_SYMBOL symbol;
  BUTTON_TYPE type;
}BUTTON;

/* Exported variables --------------------------------------------------------*/
extern SyncQueue<BUTTON> buttonsQueue;

/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported Functions --------------------------------------------------------*/

/**
 * @brief The main function of the buttons handler thread.
 *
 * @param[in] active_state The active state to compare with. If this value is true, the buttons will active with high
 *                         level. If it is false, with low level.
 * @param[in] long_pulse Time in ms to consider a pulse as long pulse. A -1 will disable this feature.
 * @param[in] up_pin The pin of the up gesture of the joystick.
 * @param[in] down_pin The pin of the down gesture of the joystick.
 * @param[in] left_pin The pin of the left gesture of the joystick.
 * @param[in] right_pin The pin of the right gesture of the joystick.
 * @param[in] center_pin The pin of the center gesture of the joystick.
 */
void buttons_thread(bool active_state, int long_pulse, int up_pin, int down_pin, int left_pin, int right_pin, int center_pin);

}


#endif /* __BUTTONS_H__ */

