#ifndef CORE_SERVER_INC_SERVER_HPP_
#define CORE_SERVER_INC_SERVER_HPP_
#include <core/service.hpp>
#include <unordered_set>
#include <thread>

namespace core
{
	class server
	{
	public:
		enum Policy : unsigned char
		{
			POLICY_SCHED_OTHER = 0x00,
			POLICY_SCHED_RR = 0x01,
			POLICY_SCHED_FIFO = 0x02
		};
	private:
		using services = std::unordered_set<service*>;
	private:
		std::thread _th;
		services _srvs;
		int _cpu_core;
		int _priority;
		Policy _policy;
		bool _running;
	public:
		server(int cpu_core = -1, int priority = 0, Policy policy = Policy::POLICY_SCHED_OTHER);
		server(server&&) = default;
		server& operator= (server&&) = default;
		server(const server&) = delete;
		server& operator= (const server&) = delete;
		~server();
		void start();
		void stop();
		void add_service(service* s);
		void remove_service(service* s);
	private:
		void run();
	};
}



#endif /* CORE_SERVER_INC_SERVER_HPP_ */
