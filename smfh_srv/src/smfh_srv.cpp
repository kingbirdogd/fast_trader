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
using namespace dbp::omd;
using namespace dbp::net::srv;
static char stdOutBuffer[65536] = {0};


inline void startUsers()
{
	for (auto& item : userMap)
	{
		auto pUser = item.second;
		std::thread* pThread = new std::thread
		(
			[&, pUser]
			()
			{
				while (true)
					pUser->run();
			}
		);
		pthread_t iThread = pThread->native_handle();
#ifndef __APPLE__
		cpu_set_t cpuset;
		CPU_ZERO(&cpuset);
		CPU_SET(cpuInfo.getCore(), &cpuset);
		pthread_setaffinity_np(iThread, sizeof(cpu_set_t), &cpuset);
#endif
		struct sched_param sch;
		memset (&sch, 0, sizeof(struct sched_param));
		int iPolicy = 0;
		pthread_getschedparam(iThread, &iPolicy, &sch);
		sch.sched_priority = 99;
		pthread_setschedparam(iThread, SCHED_FIFO, &sch);
	}
}

inline static bool start()
{
	startUsers();
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

int start_run(const char* config)
{
	if (!init(config))
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

int main(int _iArgc, char** _pszArgv)
{
	const char* config = nullptr;
	bool fork = false;
	if (3 == _pszArgv)
	{
		if (std::string(_pszArgv[1]) == "fork")
		{
			fork = true;
			config = _pszArgv[2];
		}
		else
		{
			cerr << "usage: [fork] <config json>" << endl;
			return -1;
		}
	}
	else if (2 == _pszArgv)
	{
		config = _pszArgv[1];
	}
	else
	{
		cerr << "usage: [fork] <config json>" << endl;
		return -2;
	}
	blockSigPipe();
	setbuffer(stdout, stdOutBuffer, 65536);
	if (fork)
	{
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
	}
	return start_run(config);
}



