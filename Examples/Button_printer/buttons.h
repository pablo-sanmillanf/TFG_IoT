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


/* Exported variables --------------------------------------------------------*/
extern SyncQueue<int> buttonsQueue;

/* Exported types ------------------------------------------------------------*/
enum Buttons{UP = 0, DOWN = 1, LEFT = 2, RIGHT = 3, CENTER = 4};


/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported Functions --------------------------------------------------------*/

/**
 * @brief The main function of the buttons handler thread.
 *
 * @param[in] active_state The active state to compare with. If this value is true, the buttons will active with high
 *                         level. If it is false, with low level.
 * @param[in] up_pin The pin of the up gesture of the joystick.
 * @param[in] down_pin The pin of the down gesture of the joystick.
 * @param[in] left_pin The pin of the left gesture of the joystick.
 * @param[in] right_pin The pin of the right gesture of the joystick.
 * @param[in] center_pin The pin of the center gesture of the joystick.
 */
void buttons_thread(bool active_state, int up_pin, int down_pin, int left_pin, int right_pin, int center_pin);

}


#endif /* __BUTTONS_H__ */

