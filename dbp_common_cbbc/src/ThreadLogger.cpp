/*
 * ThreadLogger.cpp
 *
 *  Created on: 2017¦~8¤ë22¤é
 *      Author: leo
 */

#include "ThreadLogger.h"





ThreadLogger::ThreadLogger(string logpath) {
	// TODO Auto-generated constructor stub
	logger = CLogger::GetLogger(logpath);
}

ThreadLogger::~ThreadLogger() {
	// TODO Auto-generated destructor stub
}

void ThreadLogger::Log(string message){
	logQueue.push(message);
}


void ThreadLogger::call_from_thread(){
	while (!isEnd){
		string message = logQueue.pop();
		logger->Log(message);
	}
}

void ThreadLogger::start(){
	t1 = thread(
			[&]
			()
			{
				call_from_thread();
			}

		);
}

bool ThreadLogger::isClose(){
	t1.join();
	return isEnd;
}

void ThreadLogger::close(){
	logger->close();
}


