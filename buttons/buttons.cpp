/**
  ******************************************************************************
  * @file   buttons.cpp
  * @author Pablo San Millán Fierro (pablo.sanmillanf@alumnos.upm.es)
  * @brief  Buttons tread handler Module.
  *
  * @note   End-of-degree work.
  *         This module handles the bottons of a joystick.
  ******************************************************************************
*/

/* Includes ------------------------------------------------------------------*/
#include "buttons.h" // Module header

/* External variables---------------------------------------------------------*/
SyncQueue<int> Buttons::buttonsQueue;

/* Private defines -----------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private variables----------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Functions -----------------------------------------------------------------*/

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
void Buttons::buttons_thread(bool active_state, int up_pin, int down_pin, int left_pin, int right_pin, int center_pin){
  CustomGPIO::GPIO gpios[] = {
      CustomGPIO::GPIO(up_pin),
      CustomGPIO::GPIO(down_pin),
      CustomGPIO::GPIO(left_pin),
      CustomGPIO::GPIO(right_pin),
      CustomGPIO::GPIO(center_pin)
  };
  for(int i = 0; i < 5; i++){
    gpios[i].setInput(CustomGPIO::GPIO_INT_BOTH);
  }

  int current_active = -1;
  enum {IDLE, BOUNCES, PIN_ACTIVE} state = IDLE;
  for(;;){
    switch(state){
      case IDLE:

        current_active = CustomGPIO::GPIO::waits(gpios, 5, -1);
        if(current_active != -1){
          state = BOUNCES;
        }
        break;
      case BOUNCES:
        usleep(40000); //40 ms
        if(active_state == gpios[current_active].read()){ //Same value, true positive
          state = PIN_ACTIVE;
        }
        else
          state = IDLE;
        break;
      case PIN_ACTIVE:
        buttonsQueue.push(current_active);
        CustomGPIO::GPIO::waits(gpios, 5, -1);
        state = IDLE;
        break;
    }
  }
}
