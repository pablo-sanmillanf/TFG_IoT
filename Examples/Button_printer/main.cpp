/***
 * EXAMPLE OF MULTITHREAD WITH BUTTONS CLASS
 */

#include <thread>
#include "buttons.h"
#include <iostream>
#include <unistd.h>


void printer_thread(){

  for(;;){
    switch(Buttons::buttonsQueue.pop()){
      case Buttons::UP:
        std::cout << "UP Pulse" << std::endl;
        break;
      case Buttons::DOWN:
        std::cout << "DOWN Pulse" << std::endl;
        break;
      case Buttons::LEFT:
        std::cout << "LEFT Pulse" << std::endl;
        break;
      case Buttons::RIGHT:
        std::cout << "RIGHT Pulse" << std::endl;
        break;
      case Buttons::CENTER:
        std::cout << "CENTER Pulse" << std::endl;
        break;
    }
  }
}

int main(){
  std::thread first (Buttons::buttons_thread, 1, 12, 18, 23, 24, 25);

  usleep(100000);
  std::thread third (printer_thread);  // spawn new thread that calls printer_thread()


  std::cout << "Main wait\n" << std::endl;
  // synchronize threads:
  first.join();                // pauses until first finishes
  third.join();               // pauses until third finishes
  return 0;
}


