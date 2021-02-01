#ifndef STATIC_ALGOS_INC_EMPTY_HPP_
#define STATIC_ALGOS_INC_EMPTY_HPP_
#include <msg.hpp>
#include <json.hpp>
#include <string>
#include <vector>
#include <tools.h>
#include <DateUtil.h>
#include <global_memory.hpp>
#include <algo.hpp>
#include <user.hpp>
#include "ThreadLogger.h"


class empty : public algo
{
public:
	ThreadLogger* logger;

private:
	struct algo_err_msg: public algo_msg_base
	{
		std::string action;
		std::string result;
		std::string reason;
		algo_err_msg():
			algo_msg_base(),
			action(""),
			result(""),
			reason("")
		{
		}
		virtual nlohmann::json to_json() const
		{
			auto j = algo_msg_base::to_json();
			j["action"] = action;
			j["result"] = result;
			j["reason"] = reason;
			return j;
		}
		virtual void on_command()
		{
			ouputQueue.enqueue(this);
		}
		virtual void release()
		{
			algo_err_msg_pool.release_obj(this);
		}
		virtual ~algo_err_msg() = default;
	};
public:
	empty() = delete;
	empty(user& u, const std::string& name);
	empty(const algo&) = delete;
	empty(algo&&) = delete;
	empty& operator= (const algo&) = delete;
	empty& operator= (algo&&) = delete;

	virtual void on_omdc_book(const Tradable&);
	virtual void on_omdd_book(const Tradable&);
	virtual void on_tcp_book(const Tradable&);
	virtual void on_omdc_trade(const Tradable&);
	virtual void on_omdd_trade(const Tradable&);
	virtual void on_tcp_trade(const Tradable&);
	virtual void handler_order(const dbp::top::enhance_order&);

	virtual void handle_command(algo_msg_base&);
	virtual algo_msg_base* json_to_msg(json& msg);
	virtual std::string get_lib_name();

	virtual void Log(std::string msg);

public:
	static rapid_ring::spmc_ring_buffer_object_pool<algo_err_msg, 8192> algo_err_msg_pool;
};





#endif /* STATIC_ALGOS_INC_EMPTY_HPP_ */
