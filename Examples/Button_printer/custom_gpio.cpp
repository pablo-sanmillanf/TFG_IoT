/**
  ******************************************************************************
  * @file   custom_gpio.cpp
  * @author Pablo San Millán Fierro (pablo.sanmillanf@alumnos.upm.es)
  * @brief  GPIO Handler Module.
  *
  * @note   End-of-degree work.
  *         This module handles a GPIO using sysfs interface.
  ******************************************************************************
*/

/* Includes ------------------------------------------------------------------*/
#include "custom_gpio.h" // Module header
#include <fcntl.h>
#include <unistd.h>
#include <string>
#include <iomanip>
#include <poll.h>
#include <iostream>

/* Private defines -----------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private variables----------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Functions -----------------------------------------------------------------*/


/**
 * @brief Class constructor. Sets the pin of the GPIO. If there is already GPIO with this pin, the
 *        behavior is undefined.
 */
CustomGPIO::GPIO::GPIO(int pin){
  this->pin = pin;
  gpio_in = true;
  fd = 0;
  interrupt_mode = GPIO_INT_NONE;
  initialized = false;
}


/**
 * @brief Initialize the GPIO pin with the desired direction.
 *
 * @param[in] gpio_direction The desired direction. If is equal to GPIO_IN, the GPIO will be an input GPIO.
 *                           Otherwise, will be an output GPIO.
 *
 * @return 0 if success, -1 if error.
 */
int CustomGPIO::GPIO::init(int dir){

  //Import the gpio file to user space
  int exp_fd;
  exp_fd = open("/sys/class/gpio/export", O_WRONLY);
  if (exp_fd == -1)
    return -1;
  std::string pin_string = std::to_string(pin);
  ::write(exp_fd, pin_string.c_str(), pin_string.size());
  close(exp_fd);


  //Set gpio direction: in or out
  int dir_fd;
  std::string dir_path = "/sys/class/gpio/gpio" + std::to_string(pin) + "/direction";
  dir_fd = open(dir_path.c_str(), O_WRONLY);
  if (dir_fd == -1)
    return -1;

  if(dir == CustomGPIO::GPIO_IN){
    if(::write(dir_fd, "in", 2) == -1)
      return -1;
  }
  else{
    if(::write(dir_fd, "out", 3) == -1)
      return -1;
  }
  close(dir_fd);


  //Open read/write file descriptor.
  std::string rdwr_path = "/sys/class/gpio/gpio" + std::to_string(pin) + "/value";
  fd = open(rdwr_path.c_str(), O_RDWR);
  if (fd == -1)
    return -1;


  initialized = true;
  return 0;
}


/**
 * @brief Set the GPIO as an output. This function and setInput() are mutually exclusive and can be called only once.
 *
 * @return 0 if success, -1 if error.
 */
int CustomGPIO::GPIO::setOutput(){
  if(initialized)
    return -1;
  if(this->init(GPIO_OUT) == -1)
    return -1;
  gpio_in = false;
  return 0;
}


/**
 * @brief Set the GPIO as an input. This function and setOutput() are mutually exclusive and can be called only once.
 *        This function is a shortcut of setInput(GPIO_INT_NONE)
 *
 * @return 0 if success, -1 if error.
 */
int CustomGPIO::GPIO::setInput(){
  return this->setInput(GPIO_INT_NONE);
}


/**
 * @brief Set the GPIO as an input. This function and setOutput() are mutually exclusive and can be called only once.
 *        The input can be configured as an interrupt pin with the modes CustomGPIO::GPIO_INT_RISING,
 *        CustomGPIO::GPIO_INT_FALLING or CustomGPIO::GPIO_INT_BOTH. If the normal functionality is desired, the interrupt
 *        mode must be CustomGPIO::GPIO_INT_NONE. Other values will cause an error.
 *
 * @return 0 if success, -1 if error.
 */
int CustomGPIO::GPIO::setInput(unsigned int interrupt_mode){
  if(interrupt_mode > GPIO_INT_BOTH)
    return -1;

  if(!initialized && this->init(GPIO_IN) == -1)
    return -1;


  int int_fd;
  std::string int_path = "/sys/class/gpio/gpio" + std::to_string(pin) + "/edge";

  int_fd = open(int_path.c_str(), O_WRONLY);
  if (int_fd == -1)
    return -1;

  if(interrupt_mode == CustomGPIO::GPIO_INT_RISING){
    if(::write(int_fd, "rising", 6) == -1)
      return -1;
  }
  else if(interrupt_mode == CustomGPIO::GPIO_INT_FALLING){
    if(::write(int_fd, "falling", 7) == -1)
      return -1;
  }
  else if(interrupt_mode == CustomGPIO::GPIO_INT_BOTH){
    if(::write(int_fd, "both", 4) == -1)
      return -1;
  }
  else{                       //(interrupt_mode == GPIO_INT_NONE)
    if(::write(int_fd, "none", 4) == -1)
      return -1;
  }
  close(int_fd);

  gpio_in = true;
  this->interrupt_mode = interrupt_mode;

  return 0;
}


/**
 * @brief Read the GPIO value.
 *
 * @return The pin value (1 or 0) if success, -1 if error.
 */
int CustomGPIO::GPIO::read(){
  char reg_value[3];

  if(lseek(fd, 0, SEEK_SET) == -1)
    return -1;

  if (::read(fd, reg_value, 3) == -1)
    return -1;

  return atoi(reg_value);
}


/**
 * @brief Write a value in the GPIO pin.
 *
 * @param[in] value The desired pin value.
 *
 * @return 0 if success, -1 if error.
 */
int CustomGPIO::GPIO::write(bool value){
  if(!gpio_in){
    if(value){
      if(::write(fd, "1", 1) == -1)
        return -1;
    }
    else{
      if(::write(fd, "0", 1) == -1)
        return -1;
    }
    return 0;
  }
  return -1;
}


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
int CustomGPIO::GPIO::waits(GPIO *gpios, int gpios_len, int time_ms){
  pollfd poll_data[gpios_len];
  int result;

  for(int i = 0; i < gpios_len; i++){

    if(gpios[i].gpio_in && gpios[i].interrupt_mode != GPIO_INT_NONE){
      poll_data[i].fd = gpios[i].fd;
      poll_data[i].events = POLLPRI;
      poll_data[i].revents = 0;

      lseek(poll_data[i].fd, 0, SEEK_SET); // Clear pending interrupts.
    }
    else{
      return -1;
    }
  }
  result = poll(poll_data, gpios_len, time_ms);
  if(result == -1)
    return -1;

  if(result == 0)
    return -2;


  for(int i = 0; i < gpios_len; i++){
    if(poll_data[i].revents & POLLPRI){
      return i;
    }
  }
  return -1;
}


/**
 * @brief Class destructor. Free all the related resources.
 */
CustomGPIO::GPIO::~GPIO(){
  int unex_fd;
  unex_fd = open("/sys/class/gpio/unexport", O_WRONLY);
  if (unex_fd != -1){
    std::string pin_string = std::to_string(pin);

    ::write(unex_fd, pin_string.c_str(), pin_string.size());
    close(unex_fd);
  }
  close(fd);
}

/*

static int GPIOExport(int pin);
static int GPIOUnexport(int pin);
static int GPIODirection(int pin, int dir);
static int GPIOInterruptMode(int pin, int interrupt_mode);
static int GPIOBlockingRead(int pin, int time_ms);
static int GPIORead(int pin);
static int GPIOWrite(int pin, bool value);


static int GPIOExport(int pin){
  int exp_fd;

  exp_fd = open("/sys/class/gpio/export", O_WRONLY);
  if (exp_fd == -1)
    return -1;
  std::string pin_string = std::to_string(pin);

  write(exp_fd, pin_string.c_str(), pin_string.size());
  close(exp_fd);

  return 0;
}


static int GPIOUnexport(int pin){
  int fd;

  fd = open("/sys/class/gpio/unexport", O_WRONLY);
  if (fd == -1)
    return -1;
  std::string pin_string = std::to_string(pin);

  write(fd, pin_string.c_str(), pin_string.size());
  close(fd);

  return 0;
}

static int GPIODirection(int pin, int dir){
  int dir_fd;

  std::string dir_path = "/sys/class/gpio/gpio" + std::to_string(pin) + "/direction";

  dir_fd = open(dir_path.c_str(), O_WRONLY);
  if (dir_fd == -1)
    return -1;

  if(dir == CustomGPIO::GPIO_IN){
    if(write(dir_fd, "in", 2) == -1)
      return -1;
  }
  else{
    if(write(dir_fd, "out", 3) == -1)
      return -1;
  }

  close(dir_fd);
  return 0;
}


static int GPIOInterruptMode(int pin, int interrupt_mode){
  int fd;

  std::string result = "/sys/class/gpio/gpio" + std::to_string(pin) + "/edge";

  fd = open(result.c_str(), O_WRONLY);
  if (fd == -1)
    return -1;

  if(interrupt_mode == CustomGPIO::GPIO_INT_RISING){
    if(write(fd, "rising", 6) == -1)
      return -1;
  }
  else if(interrupt_mode == CustomGPIO::GPIO_INT_FALLING){
    if(write(fd, "falling", 7) == -1)
      return -1;
  }
  else if(interrupt_mode == CustomGPIO::GPIO_INT_BOTH){
    if(write(fd, "both", 4) == -1)
      return -1;
  }
  else{                       //(interrupt_mode == GPIO_INT_NONE)
    if(write(fd, "none", 4) == -1)
      return -1;
  }

  close(fd);
  return(0);
}


static int GPIOBlockingRead(int pin, int time_ms){
  char reg_value[3];
  pollfd poll_data;

  std::string result = "/sys/class/gpio/gpio" + std::to_string(pin) + "/value";

  poll_data.fd = open(result.c_str(), O_RDONLY);
  if (poll_data.fd == -1)
    return -1;

  poll_data.events = POLLPRI | POLLERR;
  if(poll(&poll_data, 1, time_ms) == -1)
    return -1;

  if(!(poll_data.revents & POLLPRI))
      return -1;

  if(lseek(poll_data.fd, 0, SEEK_SET) == -1)
    return -1;

  if (read(poll_data.fd, reg_value, 3) == -1)
    return -1;

  close(poll_data.fd);

  return atoi(reg_value);
}


static int GPIORead(int pin){
  char reg_value[3];
  int fd;
  std::string result = "/sys/class/gpio/gpio" + std::to_string(pin) + "/value";

  fd = open(result.c_str(), O_RDWR);
  if (fd == -1)
    return -1;

  if (read(fd, reg_value, 3) == -1)
    return -1;

  close(fd);

  return atoi(reg_value);
}

static int GPIOWrite(int pin, bool value){
  int fd;
  std::string result = "/sys/class/gpio/gpio" + std::to_string(pin) + "/value";

  fd = open(result.c_str(), O_RDWR);
  if (fd == -1)
    return -1;

  if(value){
    if(write(fd, "1", 1) == -1)
      return -1;
  }
  else{
    if(write(fd, "0", 1) == -1)
      return -1;
  }

  close(fd);
  return(0);
}*/
