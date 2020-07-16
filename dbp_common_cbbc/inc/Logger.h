#ifndef CUSTOM_CLogger_H
#define CUSTOM_CLogger_H
#include <fstream>
#include <iostream>
#include <cstdarg>
#include <string>
#include <stdarg.h>


using namespace std;
//#define LOGGER CLogger::GetLogger()
/**
*   Singleton Logger Class.
*/

class CLogger
{
public:
	size_t initialSize = 256;
	//m_sFileName = "Log.txt";
	/**
	*   Logs a message
	*   @param sMessage message to be logged.
	*/
	void Log(const std::string& sMessage);
	/**
	*   Variable Length Logger function
	*   @param format string for the message to be logged.
	*/
	void Log(const char * format, ...);
	/**
	*   << overloaded function to Logs a message
	*   @param sMessage message to be logged.
	*/
	CLogger& operator<<(const string& sMessage);
	/**
	*   Funtion to create the instance of logger class.
	*   @return singleton object of Clogger class..
	*/
	CLogger();
	CLogger(string);
	virtual ~CLogger();
	void close();
private:
	/**
	*    Default constructor for the Logger class.
	*/

	/**
	*   copy constructor for the Logger class.
	*/
	CLogger(const CLogger&){};             // copy constructor is private
	/**
	*   assignment operator for the Logger class.
	*/
	CLogger& operator=(const CLogger&){ return *this; };  // assignment operator is private
	/**
	*   Log file name.
	**/
	std::string m_sFileName;
	/**
	*   Singleton logger class object pointer.
	**/

	/**
	*   Log file stream object.
	**/
	ofstream m_Logfile;

	int mcount = 0;
};

#endif
