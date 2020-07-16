#include "Logger.h"
#include "Utilities.h"

const string CLogger::m_sFileName = "Log.txt";
CLogger* CLogger::m_pThis = NULL;
ofstream CLogger::m_Logfile;
const size_t initialSize = 256;

CLogger::CLogger()
{

}
CLogger* CLogger::GetLogger(){
	if (m_pThis == NULL){
		m_pThis = new CLogger();
		m_Logfile.open(m_sFileName.c_str(), ios::out | ios::app);
	}
	return m_pThis;
}

CLogger* CLogger::GetLogger(string logfile){
	if (m_pThis == NULL){
		m_pThis = new CLogger();
		m_Logfile.open(logfile.c_str(), ios::out | ios::app);
	}
	return m_pThis;
}

void CLogger::Log(const char * format, ...)
{

	char sMessage[initialSize];
	int nLength = 0;
	va_list args;
	va_start(args, format);
	{
		nLength = vsnprintf(sMessage, initialSize, format, args);
	}
	//  Return the number of characters in the string referenced the list of arguments.
	// _vscprintf doesn't count terminating '\0' (that's why +1)
	va_end(args);

	//nLength = vsnprintf(format, args) + 1;
	char correctsMessage[nLength];
	va_start(args, format);
	{
		vsnprintf(correctsMessage, nLength+1, format, args);
	}
	//vsprintf(sMessage, format, args);
	//m_Logfile << Util::CurrentDateTime() << ":\t";
	m_Logfile << correctsMessage << "\n";
	m_Logfile.flush();
	va_end(args);

	//delete [] sMessage;
}

void CLogger::Log(const string& sMessage)
{
	mcount++;

	//m_Logfile <<  Util::CurrentDateTime() << ":\t";
	m_Logfile << sMessage << "\n";

	//if(mcount%10 == 0){
		m_Logfile.flush();
	//}
}

CLogger& CLogger::operator<<(const string& sMessage)
{
	mcount++;

	//m_Logfile << "\n" << Util::CurrentDateTime() << ":\t";
	m_Logfile << sMessage << "\n";

	//if(mcount%10 == 0){
		m_Logfile.flush();
	//}

	return *this;
}

void CLogger::close()
{
	m_Logfile.close();
}


