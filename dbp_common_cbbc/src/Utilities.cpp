#include "Utilities.h"
const std::string Util::CurrentDateTime()
{
	time_t rawtime;
	struct tm * timeinfo;
	char* buffer = new char[80];

	time (&rawtime);
	timeinfo = localtime (&rawtime);

	strftime (buffer,sizeof(buffer),"%Y-%m-%d.%X",timeinfo);
	return buffer;
}
