#include <pthread.h>
#include <cstring>
#include <vector>
#include <core/server.hpp>

using namespace core;

server::server(int cpu_core, int priority, Policy policy):
		_th(),
		_srvs(),
		_cpu_core(cpu_core),
		_priority(priority),
		_policy(policy),
		_running(false)
{
}

server::~server()
{
	stop();
}

void server::start()
{
	if (!_running)
	{
		_th = std::thread([&](){run();});
		auto hdl = _th.native_handle();
		struct sched_param sch;
		memset (&sch, 0, sizeof(struct sched_param));
		int iPolicy = 0;
		pthread_getschedparam(hdl, &iPolicy, &sch);
		sch.sched_priority = _priority;
		int policy = 0;
		if (Policy::POLICY_SCHED_OTHER == _policy)
		{
			policy = SCHED_OTHER;
		}
		else if (Policy::POLICY_SCHED_RR == _policy)
		{
			policy = SCHED_RR;
		}
		else
		{
			policy = SCHED_FIFO;
		}
		::pthread_setschedparam(hdl, policy, &sch);
		if (-1 != _cpu_core)
		{
#ifndef __APPLE__
			cpu_set_t cpuset;
			CPU_SET(_cpu_core, &cpuset);
			::pthread_setaffinity_np(hdl, sizeof(cpu_set_t), &cpuset);
#endif
		}
	};
}


void server::stop()
{
	if (_running)
	{
		_running = false;
		_th.join();
	}
}

void server::add_service(service* s)
{
	_srvs.insert(s);
}

void server::remove_service(service* s)
{
	_srvs.erase(s);
}

void server::run()
{
	std::vector<service*> v;
	auto size = _srvs.size();
	for (auto it = _srvs.begin(); it != _srvs.end(); ++it)
	{
		v.push_back(*it);
	}
	_running = true;
	while(_running)
	{
		for (std::size_t i = 0; i < size; ++i)
		{
			v[i]->run();
		}
	}
}




