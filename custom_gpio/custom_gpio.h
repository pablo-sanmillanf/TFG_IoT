/**
  ******************************************************************************
  * @file   custom_gpio.h
  * @author Pablo San Millán Fierro (pablo.sanmillanf@alumnos.upm.es)
  * @brief  GPIO Handler Module Header.
  *
  * @note   End-of-degree work.
  *         This module handles a GPIO using sysfs interface.
  ******************************************************************************
*/

#ifndef __CUSTOM_GPIO_H__
#define __CUSTOM_GPIO_H__

namespace CustomGPIO{

/* Includes ------------------------------------------------------------------*/
/* Exported variables --------------------------------------------------------*/
const int GPIO_IN  = 1;
const int GPIO_OUT = 0;

const int GPIO_INT_NONE    = 0;
const int GPIO_INT_RISING  = 1;
const int GPIO_INT_FALLING = 2;
const int GPIO_INT_BOTH    = 3;
/* Exported types ------------------------------------------------------------*/

class GPIO{
  int pin;
  int fd;
  bool gpio_in;
  int interrupt_mode;
  bool initialized;

  /**
   * @brief Initialize the GPIO pin with the desired direction.
   *
   * @param[in] gpio_direction The desired direction. If is equal to GPIO_IN, the GPIO will be an input GPIO.
   *                           Otherwise, will be an output GPIO.
   *
   * @return 0 if success, -1 if error.
   */
  int init(int gpio_direction);
public:
  /**
   * @brief Class constructor. Sets the pin of the GPIO.
   */
  GPIO(int pin);

  /**
   * @brief Set the GPIO as an output. This function and setInput() are mutually exclusive and can be called only once.
   *
   * @return 0 if success, -1 if error.
   */
  int setOutput();

  /**
   * @brief Set the GPIO as an input. This function and setOutput() are mutually exclusive and can be called only once.
   *        This function is a shortcut of setInput(GPIO_INT_NONE)
   *
   * @return 0 if success, -1 if error.
   */
  int setInput();

  /**
   * @brief Set the GPIO as an input. This function and setOutput() are mutually exclusive and can be called only once.
   *        The input can be configured as an interrupt pin with the modes CustomGPIO::GPIO_INT_RISING,
   *        CustomGPIO::GPIO_INT_FALLING or CustomGPIO::GPIO_INT_BOTH. If the normal functionality is desired, the interrupt
   *        mode must be CustomGPIO::GPIO_INT_NONE. Other values will cause an error.
   *
   * @return 0 if success, -1 if error.
   */
  int setInput(unsigned int interrupt_mode);

  /**
   * @brief Read the GPIO value.
   *
   * @return The pin value (1 or 0) if success, -1 if error.
   */
  int read();

  /**
   * @brief Write a value in the GPIO pin.
   *
   * @param[in] value The desired pin value.
   *
   * @return 0 if success, -1 if error.
   */
  int write(bool value);

  /**
   * @brief If the GPIO pin is in an interrupt mode, this function will block the thread until an interrupt-trigger or the
   *        timeout.
   *
   * @param[in] gpios An array with CustomGPIO::GPIO objects to wait for an interrupt.
   * @param[in] gpios_len The length of gpios array.
   * @param[in] time_ms The maximum time the thread will wait until an interrupt. If this value is negative, the maximum
   *                    wait time is infinite.
   *
   * @return The index of GPIO changed in gpios array if success, -1 if error and -2 if timed out before any interrupt.
   */
  static int waits(GPIO *gpios, int gpios_len, int time_ms);

  /**
   * @brief Class destructor. Free all the related resources.
   */
  ~GPIO();
};


/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported Functions --------------------------------------------------------*/
}

#endif /* __CUSTOM_GPIO_H__ */
