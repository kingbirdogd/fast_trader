#ifndef TOP_CLIENT_INC_TOP_CLIENT_HPP_
#define TOP_CLIENT_INC_TOP_CLIENT_HPP_
#include <time.h>
#include <top_msg.hpp>
#include <string>
#include <functional>
#include <vector>
#include <unordered_map>
#include <limits>
#include <atomic>

class top_client
{
private:
	struct buy_power_info
	{
		std::string ref;
		std::string algo_name;
	};
private:
	using order_event = std::function<void(const dbp::top::enhance_order&)>;
	using login_event = std::function<void(const dbp::top::login_response&)>;
	using top_buy_power_event = std::function<void(const std::string&, const std::string&, long long)>;
	using order_map = std::unordered_map<unsigned long long, dbp::top::enhance_order>;
	using top_buy_power_map = std::unordered_map<unsigned long long, buy_power_info>;
	using buffer = std::vector<char>;
private:
	unsigned long long _client_order_id;
	static std::atomic<unsigned long long> _base_order_id;
private:
	buffer _buffer;
	unsigned char _session_id[sizeof(dbp::top::header::session_id)];
protected:
	order_map _order_map;
	top_buy_power_map _top_buy_power_map;
	dbp::top::login_request _login;
	order_event _on_order;
	login_event _on_login;
	top_buy_power_event _on_top_buy_power;
	unsigned long long _buy_power;
	bool _ready;
protected:
	void handle_msg(const char* ptr, std::size_t size);
public:
	top_client
	(
		const std::string& user,
		const std::string& pass,
		unsigned long long buy_power = std::numeric_limits<unsigned long long>::max()
	);
	top_client(top_client&&);
	top_client& operator= (top_client&&);
	virtual ~top_client() = default;
	dbp::top::enhance_order new_order
	(
			unsigned long long quantity,
			unsigned long long price,
			unsigned int code,
			dbp::top::order_side side,
			dbp::top::order_type type = dbp::top::order_type::pl,
			dbp::top::aon_type aon = dbp::top::aon_type::non_ano,
			dbp::top::ignore_price_type ignore = dbp::top::ignore_price_type::non_ignore,
			unsigned int broker_id = 0
	);
	bool get_top_buy_power(const std::string& ref, const std::string& algo_name);
	bool modify_order(unsigned long long order_id, unsigned long long new_quantity, unsigned long long new_price);
	bool cancel_order(unsigned long long order_id);
	void set_on_order(order_event&& on_order);
	void set_on_login(login_event&& on_login);
	void set_on_top_buy_power(top_buy_power_event&& on_top_buy_power);
	const dbp::top::enhance_order* get_order(unsigned long long order_id);
	void set_buy_power(unsigned long long buy_power);
	unsigned long long get_buy_power();
protected:
	void login();
	void clean();
public:
	virtual void run() = 0;
private:
	virtual void send(const void* ptr, std::size_t size) = 0;
	virtual void close() = 0;
	template <typename T>
	void send(const T& obj)
	{
		send(static_cast<const void*>(&obj), sizeof(obj));
	}
public:
	top_client(const top_client&) = delete;
	top_client& operator= (const top_client&) = delete;
};




#endif /* TOP_CLIENT_INC_TOP_CLIENT_HPP_ */
