#include <sys/time.h>
#include <sys/wait.h>
#include <signal.h>
#include <time.h>
#include <iostream>
#include <sstream>
#include <algorithm>
#include "smfh_srv_channel.h"
using namespace std;
using namespace dbp;
using namespace dbp::tools::srv;
using namespace dbp::omd;
using namespace dbp::omd::srv;
using namespace dbp::net::srv;
using namespace dbp::shm;
static char stdOutBuffer[65536] = {0};
inline static bool start()
{
	auto itActivate = mActivateChannel.find("OmdcChannel");
	if(itActivate != mActivateChannel.end())
	{
		flush_printf("tm:%llu, Start OmdcChannel \n", dbp::tools::srv::current());
		if (!startOmdcChannel())
		{
			return false;
		}
	}
	itActivate = mActivateChannel.find("OmddChannel");
	if(itActivate != mActivateChannel.end())
	{
		flush_printf("tm:%llu, Start OmddChannel \n", dbp::tools::srv::current());
		if (!startOmddChannel())
		{
			return false;
		}
	}

	itActivate = mActivateChannel.find("BrokerQueueChannel");
	if(itActivate != mActivateChannel.end())
	{
		flush_printf("tm:%llu, Start BrokerQueueChannel \n", dbp::tools::srv::current());
		if (!startBrokerQueueChannel())
		{
			return false;
		}
	}
	return true;
}
inline static void closeAll()
{
	for (size_t i = 0; i < omdcStreams.size(); ++i)
	{
		if (0 != omdcStreams[i].m_iHot)
		{
			close(omdcStreams[i].m_iHot);
			omdcStreams[i].m_iHot = 0;
		}
		if (0 != omdcStreams[i].m_iRefresh)
		{
			close(omdcStreams[i].m_iRefresh);
			omdcStreams[i].m_iRefresh = 0;
		}
		if (0 != omdcStreams[i].m_iEpoll)
		{
			close(omdcStreams[i].m_iEpoll);
			omdcStreams[i].m_iEpoll = 0;
		}
	}
	for (size_t i = 0; i < omddStreams.size(); ++i)
	{
		if (0 != omddStreams[i].m_iHot)
		{
			close(omddStreams[i].m_iHot);
			omddStreams[i].m_iHot = 0;
		}
		if (0 != omddStreams[i].m_iRefresh)
		{
			close(omdcStreams[i].m_iRefresh);
			omdcStreams[i].m_iRefresh = 0;
		}
		if (0 != omdcStreams[i].m_iEpoll)
		{
			close(omddStreams[i].m_iEpoll);
			omddStreams[i].m_iEpoll = 0;
		}
	}
	if (0 != brokerStream.m_Channel.m_iHot)
	{
		close(brokerStream.m_Channel.m_iHot);
		brokerStream.m_Channel.m_iHot = 0;
	}
	if (0 != brokerStream.m_Channel.m_iRefresh)
	{
		close(brokerStream.m_Channel.m_iRefresh);
		brokerStream.m_Channel.m_iRefresh = 0;
	}
	if (0 != brokerStream.m_Channel.m_iEpoll)
	{
		close(brokerStream.m_Channel.m_iEpoll);
		brokerStream.m_Channel.m_iEpoll = 0;
	}
}
inline static bool init(const char* _pszJsonPath)
{
	if (!initJson(_pszJsonPath))
	{
		return false;
	}
	flush_printf("tm:%llu, initGlobal \n", dbp::tools::srv::current());
	return true;
}
int main(int _iArgc, char** _pszArgv)
{
	blockSigPipe();
	setbuffer(stdout, stdOutBuffer, 65536);
	pid_t iPid = fork();
	if (0 == iPid)
	{
		iPid = fork();
		if (0 > iPid)
		{
			cerr << "fork error" << endl;
			return -1;
		}
		else if (0 < iPid)
		{
			return 0;
		}
		else
		{
			blockSigPipe();
			setbuffer(stdout, stdOutBuffer, 65536);
		}
	}
	else if (0 > iPid)
	{
		cerr << "fork error" << endl;
		return -1;
	}
	else
	{
		int iStatus = 0;
		waitpid(iPid, &iStatus, 0);
		return 0;
	}
	if (2 != _iArgc)
	{
		cerr << "Usage: smfh_srv <cfg json>" << endl;
		return -1;
	}
	if (!init(_pszArgv[1]))
	{
		cerr << "init error:" << strerror(errno) << endl;
		closeAll();
		return -1;
	}
	if (!start())
	{
		cerr << "start error:" << strerror(errno) << endl;
		closeAll();
		return -1;
	}
	return 0;
}



