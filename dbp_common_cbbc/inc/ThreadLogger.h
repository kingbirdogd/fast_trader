/*
 * ThreadLogger.h
 *
 *  Created on: 2017¦~8¤ë22¤é
 *      Author: leo
 */

#ifndef THREADLOGGER_H_
#define THREADLOGGER_H_

#include <thread>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Logger.h"
#include "Queue.h"
#include "LoggerInterface.h"



class ThreadLogger : public LoggerInterface {
private:
	Queue logQueue;
	thread t1;
	CLogger* logger;
public:
	bool isEnd = false;
	ThreadLogger(string file);
	virtual ~ThreadLogger();
	void Log(string message);
	void call_from_thread();
	void start();
	void close();
	bool isClose();
};


#endif /* THREADLOGGER_H_ */
