#ifndef USER_ALGO_INC_USER_HPP_
#define USER_ALGO_INC_USER_HPP_

#include <top_client.hpp>
#include <top_shared_client.hpp>
#include <top_tcp_client.hpp>
#include <global_memory.hpp>
#include <msg.hpp>
#include <json.hpp>
#include <string>
#include <vector>
#include <unordered_map>
#include <rapid_ring/ring_buffer_object_poll.hpp>

class algo;
class user
{
public:
	struct algo_order: public dbp::top::enhance_order
	{
		std::string algo_name;
		algo_order():
			enhance_order(),
			algo_name("")
		{
		}
		algo_order(const enhance_order& report):
			enhance_order(report),
			algo_name("")
		{
		}
		algo_order(enhance_order&& report):
			enhance_order(std::move(report)),
			algo_name("")
		{
		}
		algo_order& operator= (const enhance_order& report)
		{
			algo_order& self = *this;
			self = report;
			return *this;
		}
		algo_order& operator= (enhance_order&& report)
		{
			algo_order& self = *this;
			self = std::move(report);
			return *this;
		}
		~algo_order() = default;
		nlohmann::json to_json() const
		{
			const enhance_order& self = *this;
			nlohmann::json j = self.to_json();
			j["algo_name"] = algo_name;
			return j;
		}
	};
public:
	struct user_order_list: public algo_msg_base
	{
		std::vector<algo_order> orders;
		user_order_list():
			algo_msg_base(),
			orders()
		{
		}
		virtual nlohmann::json to_json() const
		{
			auto j = algo_msg_base::to_json();
			j["orders"] = nlohmann::json::array();
			std::sort(orders.begin(), orders.end(),
					[](const algo_order& x, const algo_order& y) ->bool
					{
						return x.order_id < y.order_id;
					});
			for (const auto& odr : orders)
			{
				j["orders"].push_back(odr.to_json());
			}
			return j;
		}
		virtual void on_command()
		{
			ouputQueue.enqueue(this);
		}
		virtual void release()
		{
			user_order_list_pool.release_obj(this);
		}
		virtual ~user_order_list() = default;
	};
private:
	using json = nlohmann::json;
	using comsumer = typename CBroadCastQueue::comsumer_st;
private:
	unsigned long long _id;
	top_client* _client;
	comsumer _md;
	std::unordered_map<std::string, algo*> _algos;
	std::unordered_map<unsigned long long, algo*> _odr_map;
public:
	user() = delete;
	template <typename TCfg>
	user
	(
		TCfg& cfg,
		unsigned long long id,
		const std::string& user,
		const std::string& pass,
		unsigned long long buy_power = std::numeric_limits<unsigned long long>::max()
	):
	_id(id),
	_client(new top_shared_client(user, pass, buy_power)),
	_md(broadcastQueue),
	_algos(),
	_odr_map()
	{
		_client->set_on_order([&](const dbp::top::enhance_order& odr){handler_order(odr);});
		cfg.x_depends_y(_md, broadcastQueue);
	}
	template <typename TCfg>
	user
	(
		TCfg& cfg,
		unsigned long long id,
		const std::string& host,
		unsigned short int port,
		const std::string& user,
		const std::string& pass,
		unsigned long long buy_power = std::numeric_limits<unsigned long long>::max()
	):
	_id(id),
	_client(new top_tcp_client(host, port, user, pass, buy_power)),
	_md(broadcastQueue),
	_algos(),
	_odr_map()
	{
		_client->set_on_order([&](const dbp::top::enhance_order& odr){handler_order(odr);});
		cfg.x_depends_y(_md, broadcastQueue);
	}
	user(const user&) = delete;
	user(user&&) = delete;
	user& operator= (const user&) = delete;
	user& operator= (user&&) = delete;
	~user();
	dbp::top::enhance_order new_order
	(
			algo* algo,
			unsigned long long quantity,
			unsigned long long price,
			unsigned int code,
			dbp::top::order_side side,
			dbp::top::order_type type = dbp::top::order_type::pl,
			dbp::top::aon_type aon = dbp::top::aon_type::non_ano,
			dbp::top::ignore_price_type ignore = dbp::top::ignore_price_type::non_ignore,
			unsigned int broker_id = 0
	);
	bool modify_order(unsigned long long order_id, unsigned long long new_quantity, unsigned long long new_price);
	bool cancel_order(unsigned long long order_id);
	bool add_algo(const std::string& name, const std::string lib, json& cfg);
	void run();
	void set_buy_power(unsigned long long buy_power);
	unsigned long long get_buy_power();
	unsigned long long get_id();
	algo* get_algo(const std::string& name);
private:
	void handler_order(const dbp::top::enhance_order& odr);
public:
	static rapid_ring::spsc_ring_buffer_object_pool<user_order_list, 8192> user_order_list_pool;
};



#endif /* USER_ALGO_INC_USER_HPP_ */
