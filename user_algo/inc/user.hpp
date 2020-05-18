#ifndef USER_ALGO_INC_USER_HPP_
#define USER_ALGO_INC_USER_HPP_

#include <top_client.hpp>
#include <top_shared_client.hpp>
#include <top_tcp_client.hpp>
#include <global_memory.hpp>
#include <msg.hpp>
#include <json.hpp>
#include <string>
#include <unordered_map>

class algo;
class user
{
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
private:
	void handler_order(const dbp::top::enhance_order& odr);
};



#endif /* USER_ALGO_INC_USER_HPP_ */
