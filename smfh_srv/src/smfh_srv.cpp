#include <sys/time.h>
#include <sys/wait.h>
#include <signal.h>
#include <time.h>
#include <json.hpp>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <algo.hpp>
#include "smfh_srv_channel.h"
using namespace std;
using namespace dbp;
using namespace dbp::tools::srv;
using namespace dbp::omd;
using namespace dbp::omd;
using namespace dbp::net::srv;
static char stdOutBuffer[65536] = {0};

using json = nlohmann::json;


inline std::string input()
{
	return dbp::tools::srv::get_line(input_stream);
}

inline void output(const std::string& str)
{
	dbp::tools::srv::output(output_stream, str);
}

inline void output(const json& j)
{
	output(j.dump());
}

inline void decode()
{
	auto line = input();
	try
	{
		json j = json::parse(line);
		auto cmd = j["cmd"].get<std::string>();
		if (cmd == "get_omdc_tradable")
		{
			auto code = j["code"].get<unsigned int>();
			auto it = omdcMap.find(code);
			if (omdcMap.end() == it)
			{
				j["error"] = "omdc code not found";
				output(j);
				return;
			}
			else
			{
				j["omdc_tradable"] = it->second.to_json();
				output(j);
				return;
			}
		}
		else if (cmd == "get_omdd_tradable")
		{
			auto code = j["code"].get<unsigned int>();
			auto it = omddMap.find(code);
			if (omddMap.end() == it)
			{
				j["error"] = "omdc code not found";
				output(j);
				return;
			}
			else
			{
				j["omdd_tradable"] = it->second.to_json();
				output(j);
				return;
			}
		}
		else
		{
			auto id = j["id"].get<unsigned long long>();
			auto uit = userMap.find(id);
			if (userMap.end() == uit)
			{
				j["error"] = "user not found";
				output(j);
				return;
			}
			auto& u = (*(uit->second));
			if (cmd == "get_buy_power")
			{
				j["buy_power"] = u.get_buy_power();
				output(j);
				return;
			}
			else if (cmd == "set_buy_power")
			{
				auto buy_power = j["buy_power"].get<unsigned long long>();
				u.set_buy_power(buy_power);
				j["set_buy_power"] = "success";
				output(j);
				return;
			}
			else
			{
				auto algo_name  = j["algo_name"].get<std::string>();
				auto al = u.get_algo(algo_name);
				if (nullptr == al)
				{
					j["error"] = "algo name not found";
					output(j);
					return;
				}
				algo_msg_base* msg = al->json_to_msg(j);
				msg->algo_name = algo_name;
				msg->user_id = id;
				msg->al = al;
				msg->ref = j["ref"].get<std::string>();
				Tradable t;
				t.m_MsgType = MsgType::COMMAND;
				t.m_LastTradeQuantity = reinterpret_cast<unsigned long long>(msg);
				broadcastQueue.enqueue(t);
			}
		}
	}
	catch(const std::exception& e)
	{
		json j;
		j["error"] = "parse error";
		j["exception"] = e.what();
		j["line"] = line;
		output(j);
	}
}

inline void dequeueOutput()
{
	algo_msg_base* msg;
	ouputQueue.deque(msg);
	output(msg->al->msg_to_json(msg));
}

inline void startDecode()
{
	new std::thread
	(
		[&]
		()
		{
			while (true)
				decode();
		}
	);
}

inline void startOutput()
{
	new std::thread
	(
		[&]
		()
		{
			while (true)
				startDecode();
		}
	);
}

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

inline static bool initJson(const char* _pszJsonPath)
{
	json j;
	std::ifstream ifs(_pszJsonPath);
	if (!ifs)
	{
		std::cerr << "Load Binary Json Error" << std::endl;
		return false;
	}
	std::string strJson((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
	ifs.close();
	try
	{
		j = json::parse(strJson);
	}
	catch(...)
	{
		std::cerr << "Parse Json Error" << std::endl;
		return false;
	}
	flush_printf("tm:%llu, loadUser \n", dbp::tools::srv::current());
	if (!loadUsers(j))
	{
		std::cerr << "loadUsers fail" << std::endl;
		return false;
	}
	flush_printf("tm:%llu, loadInOut \n", dbp::tools::srv::current());
	if (!loadInOut(j))
	{
		std::cerr << "loadInOut fail" << std::endl;
		return false;
	}
	flush_printf("tm:%llu, initSOL_SOCKET \n", dbp::tools::srv::current());
	if (!dbp::net::srv::initSOL_SOCKET(j))
	{
		std::cerr << "initSOL_SOCKET fail" << std::endl;
		return false;
	}
	flush_printf("tm:%llu, initSOL_TCP \n", dbp::tools::srv::current());
	if (!dbp::net::srv::initSOL_TCP(j))
	{
		std::cerr << "initSOL_TCP fail" << std::endl;
		return false;
	}
	flush_printf("tm:%llu, initIPPROTO_TCP \n", dbp::tools::srv::current());
	if (!dbp::net::srv::initIPPROTO_TCP(j))
	{
		std::cerr << "initIPPROTO_TCP fail" << std::endl;
		return false;
	}
	flush_printf("tm:%llu, loadCpu \n", dbp::tools::srv::current());
	if (!loadCpu(j))
	{
		std::cerr << "loadCpu fail" << std::endl;
		return false;
	}
	startDecode();
	//startOutput();
	//startUsers();
	flush_printf("tm:%llu, loadRetran \n", dbp::tools::srv::current());
	if (!loadRetran(j))
	{
		std::cerr << "loadRetran fail" << std::endl;
		return false;
	}
	flush_printf("tm:%llu, loadActivateChannel \n", dbp::tools::srv::current());
	if (!loadActivateChannel(j))
	{
		std::cerr << "loadActivateChannel fail" << std::endl;
		return false;
	}
	flush_printf("tm:%llu, loadDefinition \n", dbp::tools::srv::current());

	if (!loadDefinition(j))
	{
		std::cerr << "loadDefinition fail" << std::endl;
		return false;
	}
	auto itActivate = mActivateChannel.find("OmdcChannel");
	if(itActivate != mActivateChannel.end())
	{
		flush_printf("tm:%llu, loadChannel = OmdcChannel \n", dbp::tools::srv::current());
		if (!loadChannel(j, "OmdcChannel", omdcStreams))
		{
			std::cerr << "loadChannel OmdcChannel" << std::endl;
			return false;
		}
	}
	itActivate = mActivateChannel.find("OmddChannel");
	if(itActivate != mActivateChannel.end())
	{
		flush_printf("tm:%llu, loadChannel = OmddChannel \n", dbp::tools::srv::current());
		if (!loadChannel(j, "OmddChannel", omddStreams))
		{
			std::cerr << "loadChannel OmddChannel" << std::endl;
			return false;
		}
	}
	return true;
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
	bool is_fork = false;
	if (3 == _iArgc)
	{
		if (std::string(_pszArgv[1]) == "fork")
		{
			is_fork = true;
			config = _pszArgv[2];
		}
		else
		{
			cerr << "usage: [fork] <config json>" << endl;
			return -1;
		}
	}
	else if (2 == _iArgc)
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
	if (is_fork)
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



