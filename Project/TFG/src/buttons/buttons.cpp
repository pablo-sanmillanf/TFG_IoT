/**
  ******************************************************************************
  * @file   buttons.cpp
  * @author Pablo San Millán Fierro (pablo.sanmillanf@alumnos.upm.es)
  * @brief  Buttons thread handler Module.
  *
  * @note   End-of-degree work.
  *         This module handles the bottons of a joystick.
  ******************************************************************************
*/

/* Includes ------------------------------------------------------------------*/
#include "buttons.h" // Module header
#include <atomic>

/* External variables---------------------------------------------------------*/
SyncQueue<Buttons::BUTTON> Buttons::buttonsQueue;

/* Private defines -----------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private variables----------------------------------------------------------*/
static std::atomic_bool run = true;
static int regular_wait_time = 200;
/* Private function prototypes -----------------------------------------------*/
/* Functions -----------------------------------------------------------------*/

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
void Buttons::buttons_thread(bool active_state, int long_pulse, int up_pin, int down_pin, int left_pin,
    int right_pin, int center_pin){
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

  int current_active = -1, aux = -1;
  enum {IDLE, BOUNCES, PIN_ACTIVE, LONG_PULSE} state = IDLE;
  while(run){
    switch(state){
      case IDLE:

        current_active = CustomGPIO::GPIO::waits(gpios, 5, regular_wait_time);
        if(current_active >= 0){
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

        aux = CustomGPIO::GPIO::waits(gpios, 5, long_pulse);
        if(aux == -2){  //If timeout
          buttonsQueue.push((Buttons::BUTTON){(Buttons::BUTTON_SYMBOL)current_active, Buttons::LONG_PULSE});
          state = LONG_PULSE;
        }
        else if(current_active == aux && active_state != gpios[current_active].read()){
          buttonsQueue.push((Buttons::BUTTON){(Buttons::BUTTON_SYMBOL)current_active, Buttons::SHORT_PULSE});
          state = IDLE;
        }
        break;
      case LONG_PULSE:

        if(current_active == CustomGPIO::GPIO::waits(gpios, 5, regular_wait_time) &&
            active_state != gpios[current_active].read()){
          buttonsQueue.push((Buttons::BUTTON){(Buttons::BUTTON_SYMBOL)current_active, Buttons::LONG_PULSE_RELEASED});
          state = IDLE;
        }
        break;
    }
  }
  run = true;
}


/**
 * @brief This function is used to finish the buttons handler thread correctly. The termination is
 *        not instantaneous and after this function the join() function of the thread in question
 *        must be called.
 *
 */
void Buttons::buttons_thread_finisher(){
  run = false;
}
