/*
 * Queue.cpp
 *
 *  Created on: 2017¦~8¤ë8¤é
 *      Author: leo
 */

#include "Queue.h"


Queue::Queue() {
	// TODO Auto-generated constructor stub

}

Queue::~Queue() {
	// TODO Auto-generated destructor stub
}

string Queue::pop()
  {
    std::unique_lock<std::mutex> mlock(mutex_);
    while (queue_.empty())
    {
      cond_.wait(mlock);
    }
    string item = move(queue_.front());
    queue_.pop();
    return item;
  }
/*
  void Queue::pop(T& item)
  {
    std::unique_lock<std::mutex> mlock(mutex_);
    while (queue_.empty())
    {
      cond_.wait(mlock);
    }std::
    item = queue_.front();
    queue_.pop();
  }

  void Queue::push(const T& item)
  {
    std::unique_lock<std::mutex> mlock(mutex_);
    queue_.push(item);
    mlock.unlock();
    cond_.notify_one();
  }
*/
  void Queue::push(string item)
  {
    std::unique_lock<std::mutex> mlock(mutex_);
    queue_.push(std::move(item));
    mlock.unlock();
    cond_.notify_one();
  }



