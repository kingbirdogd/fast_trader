/*
 * DateUtil.h
 *
 *  Created on: 2017¦~5¤ë17¤é
 *      Author: leo
 */

#ifndef DATEUTIL_H_
#define DATEUTIL_H_

#include <time.h>
#include <chrono>
#include <string>
#include <iomanip>
#include <ctime>
#include <iostream>
#include <sstream>
#include <ctime>

using namespace std::chrono;



class DateUtil {
public:
	DateUtil();
	virtual ~DateUtil();
	static char* getToday();
	static char* getCurrentTime();
	static char* getTicketTime(unsigned long long _t);
	static char* getTicketTimeHHMM(unsigned long long _t);
	static std::time_t getTodayTime(std::string str_time);
	static std::time_t getCurrentSystemTime();
};

 /* namespace easywsclient */

#endif /* DATEUTIL_H_ */
