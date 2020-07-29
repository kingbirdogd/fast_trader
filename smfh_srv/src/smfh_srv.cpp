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

static std::thread* pDecode = nullptr;


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
		flush_printf("Decode, tm:%s, :%s\n", dbp::tools::srv::get_time_str().c_str(), line.c_str());
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
		else if (cmd == "get_warrent_underlying")
		{
			auto code = j["code"].get<unsigned int>();
			auto it = warrantToUnderlying.find(code);
			if (warrantToUnderlying.end() == it)
			{
				j["error"] = std::string("underlying code not found for warrent:") + std::to_string(code);
				output(j);
				return;
			}
			else
			{
				j["underlying"] = it->second;
				output(j);
				return;
			}
		}
		else if (cmd == "get_omdd_tradable")
		{
			unsigned long long code = 0;
			try
			{
				code = j["code"].get<unsigned int>();
				auto itName = codeToName.find(code);
				if (codeToName.end() == itName)
				{
					j["error"] = "omdd code map to name not found";
					output(j);
					return;
				}
				j["code"] = itName->second;
			}
			catch(...)
			{
				auto str_code = j["code"].get<std::string>();
				auto itCode = nameToCode.find(str_code);
				if (nameToCode.end() == itCode)
				{
					j["error"] = "omdd code name not found";
					output(j);
					return;
				}
				code = itCode->second;
			}
			auto it = omddMap.find(code);
			if (omddMap.end() == it)
			{
				j["error"] = "omdd code not found";
				output(j);
				return;
			}
			else
			{
				j["omdd_tradable"] = it->second.to_json();
				j["omdd_tradable"]["m_Symbol"] =  codeToName.find(code)->second;
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
			else if (cmd == "get_top_buy_power")
			{
				auto ref = j["ref"].get<std::string>();
				auto algo_name = j["algo_name"].get<std::string>();
				auto result = u.get_top_buy_power(ref, algo_name);
				if (!result)
				{
					j["error"] = "get_top_buy_power client not ready";
					output(j);
					return;
				}
			}
			else if (cmd == "order_list")
			{
				std::string algo_name = "";
				auto it = j.find("algo_name");
				if (j.end() != it)
				{
					if (it.value().type() == json::value_t::string)
					{
						algo_name = it.value().get<std::string>();
					}
				}
				algo* al = nullptr;
				if (algo_name != "")
				{
					al = u.get_algo(algo_name);
					if (nullptr == al)
					{
						j["error"] = "algo name not found";
						output(j);
						return;
					}
				}
				unsigned long long max_display = std::numeric_limits<unsigned long long>::max();
				it = j.find("max_display");
				if (j.end() != it)
				{
					if (it.value().type() == json::value_t::number_unsigned)
					{
						max_display = it.value().get<unsigned long long>();
					}
				}
				user::user_order_list* list_msg = user::user_order_list_pool.get_obj();
				list_msg->max_display = max_display;
				algo_msg_base* msg = list_msg;
				msg->algo_name = algo_name;
				msg->id = id;
				msg->al = al;
				msg->ref = j["ref"].get<std::string>();
				Tradable t;
				t.m_MsgType = MsgType::ORDER_LIST;
				t.m_LastTradeQuantity = reinterpret_cast<unsigned long long>(msg);
				broadcastQueue.enqueue(t);
			}
			else if (cmd == "get_algo_names")
			{
				auto algos = u.get_algos();
				j["algos"] = json::object();
				for (const auto& algo : algos)
					j["algos"][algo.first] = algo.second;
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
				msg->id = id;
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
	output(msg->to_json());
	msg->release();
}

inline void startDecode()
{
	pDecode = new std::thread
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
				dequeueOutput();
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
		sch.sched_priority = SCHED_PRIORITY;
		pthread_setschedparam(iThread, SCHED_TYPE, &sch);
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
	itActivate = mActivateChannel.find("PriceTableChannel");
	if(itActivate != mActivateChannel.end())
	{
		flush_printf("tm:%llu, Start PriceTableChannel \n", dbp::tools::srv::current());
		if (!startPriceTableChannel())
		{
			return false;
		}
	}
	itActivate = mActivateChannel.find("S1Channel");
	if(itActivate != mActivateChannel.end())
	{
		flush_printf("tm:%llu, Start S1Channel \n", dbp::tools::srv::current());
		if (!startS1Channel())
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
			close(omddStreams[i].m_iRefresh);
			omddStreams[i].m_iRefresh = 0;
		}
		if (0 != omddStreams[i].m_iEpoll)
		{
			close(omddStreams[i].m_iEpoll);
			omddStreams[i].m_iEpoll = 0;
		}
	}
	for (size_t i = 0; i < pricetableStreams.size(); ++i)
	{
		if (0 != pricetableStreams[i].m_iHot)
		{
			close(pricetableStreams[i].m_iHot);
			pricetableStreams[i].m_iHot = 0;
		}
		if (0 != pricetableStreams[i].m_iRefresh)
		{
			close(pricetableStreams[i].m_iRefresh);
			pricetableStreams[i].m_iRefresh = 0;
		}
		if (0 != pricetableStreams[i].m_iEpoll)
		{
			close(pricetableStreams[i].m_iEpoll);
			pricetableStreams[i].m_iEpoll = 0;
		}
	}
	for (size_t i = 0; i < s1Streams.size(); ++i)
	{
		if (0 != s1Streams[i].m_iHot)
		{
			close(s1Streams[i].m_iHot);
			s1Streams[i].m_iHot = 0;
		}
		if (0 != s1Streams[i].m_iRefresh)
		{
			close(s1Streams[i].m_iRefresh);
			s1Streams[i].m_iRefresh = 0;
		}
		if (0 != s1Streams[i].m_iEpoll)
		{
			close(s1Streams[i].m_iEpoll);
			s1Streams[i].m_iEpoll = 0;
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

	flush_printf("tm:%llu, loadIvFile \n", dbp::tools::srv::current());
	loadIvFile(j);
	flush_printf("tm:%llu, loadOrderbookFile \n", dbp::tools::srv::current());
	loadOrderbookFile(j);

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
	startOutput();
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
	itActivate = mActivateChannel.find("PriceTableChannel");
	if(itActivate != mActivateChannel.end())
	{
		flush_printf("tm:%llu, loadChannel = PriceTableChannel \n", dbp::tools::srv::current());
		if (!loadChannel(j, "PriceTableChannel", pricetableStreams))
		{
			std::cerr << "loadChannel PriceTableChannel" << std::endl;
			return false;
		}
	}
	itActivate = mActivateChannel.find("S1Channel");
	if(itActivate != mActivateChannel.end())
	{
		flush_printf("tm:%llu, loadChannel = S1Channel \n", dbp::tools::srv::current());
		if (!loadChannel(j, "S1Channel", s1Streams))
		{
			std::cerr << "loadChannel S1Channel" << std::endl;
			return false;
		}
	}
	startUsers();
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
	INFO("info msg:%d\n", 0);
	DEBUG("debug msg:%d\n", 1);
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
	auto rt = start_run(config);
	pDecode->join();
	return rt;
}



