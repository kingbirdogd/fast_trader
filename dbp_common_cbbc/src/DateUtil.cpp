/*
 * DateUtil.cpp
 *
 *  Created on: 2017¦~5¤ë17¤é
 *      Author: leo
 */

#include "DateUtil.h"


DateUtil::DateUtil() {
	// TODO Auto-generated constructor stub

}

DateUtil::~DateUtil() {
	// TODO Auto-generated destructor stub
}

char* DateUtil::getToday(){
	time_t rawtime;
	struct tm * timeinfo;
	char* buffer = new char[10];

	time (&rawtime);
	timeinfo = localtime (&rawtime);

	strftime (buffer,10,"%Y%m%d",timeinfo);
	//puts (buffer);
	return buffer;
}

char* DateUtil::getCurrentTime(){
	time_t rawtime;
	struct tm * timeinfo;
	char* buffer = new char[8];

	time (&rawtime);
	timeinfo = localtime (&rawtime);

	strftime (buffer,8,"%H%M%S",timeinfo);
	//puts (buffer);
	return buffer;
}

char* DateUtil::getTicketTime(unsigned long long _t){
	time_t rawtime = _t / 1000;
	struct tm * timeinfo;
	char* buffer = new char[8];

	//time (&rawtime);
	timeinfo = localtime (&rawtime);

	strftime (buffer,8,"%H%M%S",timeinfo);
	//puts (buffer);
	return buffer;
}

char* DateUtil::getTicketTimeHHMM(unsigned long long _t){
	time_t rawtime = _t / 1000;
	struct tm * timeinfo;
	char* buffer = new char[6];

	//time (&rawtime);
	timeinfo = localtime (&rawtime);

	strftime (buffer,6,"%H%M",timeinfo);
	//puts (buffer);
	return buffer;
}

std::time_t DateUtil::getTodayTime(std::string str_time){
	struct std::tm tm;
	strptime(str_time.c_str(), "%Y%m%d %H%M%S", &tm);
	//auto tp = std::chrono::system_clock::from_time_t(std::mktime(&tm));
	/*
	std::istringstream ss(str_time);
	ss >> std::get_time(&tm,"%Y%m%d %H%M%S");
	*/

	std::time_t time = mktime(&tm);
	return time;
}

std::time_t DateUtil::getCurrentSystemTime(){
	system_clock::time_point p = system_clock::now();
	std::time_t t = system_clock::to_time_t(p);
	return t;
}

 /* namespace easywsclient */
