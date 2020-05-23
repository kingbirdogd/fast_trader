#ifndef USER_ALGO_INC_ALGO_HPP_
#define USER_ALGO_INC_ALGO_HPP_
#include <msg.hpp>
#include <user.hpp>
#include <json.hpp>
#include <string>


class algo
{
public:
	using json = nlohmann::json;
protected:
	user& _u;
	std::string _name;
public:
	algo() = delete;
	algo(user& u, const std::string& name);
	algo(const algo&) = delete;
	algo(algo&&) = delete;
	algo& operator= (const algo&) = delete;
	algo& operator= (algo&&) = delete;
	virtual ~algo() = default;
	virtual void on_omdc_book(const Tradable&) = 0;
	virtual void on_omdd_book(const Tradable&) = 0;
	virtual void on_omdc_trade(const Tradable&) = 0;
	virtual void on_omdd_trade(const Tradable&) = 0;
	virtual void handler_order(const dbp::top::enhance_order&) = 0;
	virtual void handle_command(algo_msg_base& cmd) = 0;
	virtual algo_msg_base* json_to_msg(json& msg) = 0;
	virtual json msg_to_json(algo_msg_base* msg) = 0;
	const std::string& get_name() const;
	static algo* get_algo(user& u, const std::string& name, const std::string& lib, json& cfg);
};



#endif /* USER_ALGO_INC_ALGO_HPP_ */
