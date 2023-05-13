/**
  ******************************************************************************
  * @file   sync_queue.h
  * @author Pablo San Millán Fierro (pablo.sanmillanf@alumnos.upm.es)
  * @brief  Thread-Safe Queue Module header.
  *
  * @note   End-of-degree work.
  *         This module implements a threat-safe queue to communicate
  *         between threads.
  ******************************************************************************
*/
#ifndef __SYNC_QUEUE_H__
#define __SYNC_QUEUE_H__

/* Includes ------------------------------------------------------------------*/
#include <queue>
#include <mutex>
#include <condition_variable>


/* Exported variables --------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
template <class T>
class SyncQueue
{
  std::queue<T> queue;
  std::mutex mutex;
  std::condition_variable cond;
public:

  /**
   * @brief Push an item of type `T` in the queue. Also, notify the insertion to one of the
   *        threads waiting for an item in the queue.
   *
   * @param[in] item The item to be pushed.
   *
   */
  void push(T item){
    std::lock_guard<std::mutex> lock(mutex);
    queue.push(item);
    cond.notify_one();
  }

  /**
   * @brief Obtain the oldest item of type `T` from the queue. If the thread is empty, the thread waits
   *        until another thread push an item to the queue.
   *
   * @return The oldest item of the queue.
   *
   */
  T pop(){
    std::unique_lock<std::mutex> lock(mutex);
    while(queue.empty()){
      cond.wait(lock);
    }
    T item = queue.front();
    queue.pop();
    return item;
  }
};


/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported Functions --------------------------------------------------------*/


#endif /* __SYNC_QUEUE_H__ */
