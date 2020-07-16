/*
 * Queue.h
 *
 *  Created on: 2017¦~8¤ë8¤é
 *      Author: leo
 */

#include <queue>
#include <thread>
#include <mutex>
#include <memory>
#include <stdio.h>
#include <condition_variable>

#ifndef QUEUE_H_
#define QUEUE_H_

using namespace std;



class Queue {
private:
	std::queue<string> queue_;
    std::mutex mutex_;
    std::condition_variable cond_;

public:
    string pop();
	//void pop(AlgoMsg* item);
	//void push(const AlgoMsg& item);
	void push(string);
	Queue();
	//Queue(const Queue&) = delete;
	//Queue& operator=(const Queue&) = delete;
	virtual ~Queue();

};



#endif /* QUEUE_H_ */
